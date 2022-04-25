#include "bus.h"
#include "timers.h"
#include "mbc/cart.h"

u8 read_byte(u16 address) 
{
	gb.cycles += 4;

	if (gb.bootrom_disabled == 0 && address < 0x100)	return gb.bootrom[address];
	else if (address <= 0x7FFF)							return cart_read_byte(address);
	else if (address >= 0x8000 && address <= 0x9FFF)	return gb.vram[address & 0x1FFF];
	else if (address >= 0xA000 && address <= 0xBFFF)	return cart_read_byte(address);
	else if (address >= 0xC000 && address <= 0xCFFF)	return gb.wram[address & 0x1FFF];
	else if (address >= 0xD000 && address <= 0xDFFF)	return gb.wram[address & 0x1FFF];
	else if (address >= 0xE000 && address <= 0xFDFF)	return gb.wram[address & 0x1FFF];
	else if (address >= 0xFE00 && address <= 0xFE9F)	return gb.oam[address & 0xFF];
	else if (address >= 0xFF00 && address <= 0xFF7F)	return read_io(address);
	else if (address >= 0xFF80 && address <= 0xFFFE)	return gb.hram[address & 0x7F];
	else if (address == INTERRUPT_ENABLE)				return gb.interrupt_enable.raw;
	else if (address == 0xFF7F)							return 0xFF; // UNUSED Memory Mapped IO returns OxFF, add more
	else
	{
		printf("Unimplemented Read Address: %X\n", address);
		return 0xFF;
	}
}

u8 read_io(u16 address)
{
	switch (address)
	{
		case JOYP:
			return read_joyp();
			break;

		case SB:
			return gb.sb;
			break;

		case SC:
			return gb.sc | 0b01111110;
			break;

		case DIV:
			return read_div();
			break;

		case TIMA:
			return read_tima();
			break;

		case TMA:
			return gb.tma;
			break;

		case TAC:
			return gb.tac.raw | 0b11111000;
			break;

		case INTERRUPT_FLAG:
			return gb.interrupt_flag.raw | 0b11100000;
			break;

		case LCDC:
			return gb.lcdc.raw;
			break;

		case STAT:
			return gb.stat.raw | 0b10000000;
			break;

		case SCY:
			return gb.scy;
			break;

		case SCX:
			return gb.scx;
			break;

		case LYC:
			return gb.lyc;
			break;

		case DMA:
			return gb.dma;
			break;

		case BGP:
			return gb.bgp;
			break;

		case OBP0:
			return gb.obp0;
			break;

		case OBP1:
			return gb.obp1;
			break;

		case WY:
			return gb.wy;
			break;

		case WX:
			return gb.wx;
			break;

		case LY:
			return gb.ly;
			break;

		case 0xFF4D:
			return 0xFF; // color gb only
			break;

		default:
			if (address >= 0xFEA0 && address <= 0xFEFF)
				return 0xFF; // prohibited
			else if (address >= 0xFF10 && address <= 0xFF3F)
				return gb.audio[address - 0xFF10];
			break;
	}
}



void write_byte(u16 address, u8 value)
{
	gb.cycles += 4;

	if (address <= 0x7FFF)								write_cart_byte(address, value);
	else if (address >= 0x8000 && address <= 0x9FFF)	gb.vram[address - 0x8000] = value;
	else if (address >= 0xA000 && address <= 0xBFFF)	write_cart_byte(address, value);
	else if (address >= 0xC000 & address <= 0xCFFF)		gb.wram[address - 0xC000] = value;
	else if (address >= 0xD000 & address <= 0xDFFF)		gb.wram[address - 0xC000] = value;
	else if (address >= 0xE000 && address <= 0xFDFF)	gb.wram[address - 0xE000] = value;
	else if (address >= 0xFE00 && address <= 0xFE9F)	gb.oam[address - 0xFE00] = value;
	else if (address >= 0xFEA0 && address <= 0xFEFF)	printf("write: illegal write to prohibited area: %x\n", address);
	else if (address >= 0xFF00 && address <= 0xFF7F)	write_io(address, value);
	else if (address >= 0xFF80 && address <= 0xFFFE)	gb.hram[address - 0xFF80] = value;
	else if (address == INTERRUPT_ENABLE)				gb.interrupt_enable.raw = value;
	else if (address >= 0xE000 && address <= 0xFDFF)	printf("Illegal write to echo ram: %x\n", address);
	else
		printf("Unimplemented Write Address: %X\n", address);
}

void write_io(u16 address, u8 value)
{
	switch (address)
	{
		case JOYP:
			gb.joyp.raw = (value & 0b00110000) | (gb.joyp.raw & 0b11001111);
			break;

		case SB:
			gb.sb = value; 
			break;

		case SC:
			gb.sc = value; printf("%c", gb.sb);
			break;

		case DIV:
			write_div();
			break;

		case TIMA:
			write_tima(value);
			//gb.tima = value;
			break;

		case TMA:
			gb.tma = value;
			break;

		case TAC:
			write_tac(value);
			break;

		case INTERRUPT_FLAG:
			gb.interrupt_flag.raw = value;
			break;

		case LCDC:
			gb.lcdc.raw = value;
			break;

		case STAT:
			gb.stat.raw = (value & 0b11111000) | (gb.stat.raw & 0b00000111);
			break;

		case SCY:
			gb.scy = value;
			break;

		case SCX:
			gb.scx = value;
			break;

		case LYC:
			gb.lyc = value;
			break;

		case DMA:
			gb.dma = value;
			dma_transfer();
			break;

		case BGP:
			gb.bgp = value;
			break;

		case OBP0:
			gb.obp0 = value;
			break;

		case OBP1:
			gb.obp1 = value;
			break;

		case 0xFF50:
			gb.bootrom_disabled = 1;
			break;

		case WY:
			gb.wy = value;
			break;

		case WX:
			gb.wx = value;
			break;

		case LY: // read only
			break;

		case 0xFF4D: // color gb only
			break;

		case 0xFF7F: // unimplemented MMIO
			break;

		default:
			if (address >= 0xFF10 && address <= 0xFF3F)
				gb.audio[address - 0xFF10] = value;
			else
				printf("Write to unimplemented MMIO: %X\n", address);
			break;
	}
}

u16 read_word(u16 address)
{
	return (read_byte(address + 1) << 8) |  read_byte(address);
}

void dma_transfer()
{
	u16 address = gb.dma * 0x100;
	if (address >= 0xE000)
		address -= 0x2000;
	for (u8 i = 0; i < 0xA0; i++)
	{
		gb.oam[i] = read_byte(address + i);
	}
	gb.cycles -= 4 * 160;
}

u8 read_joyp()
{
		update_gb_keystate();

		if (!gb.joyp.action && gb.joyp.direction)
		{
			gb.joyp.down_start = gb.start;
			gb.joyp.up_select = gb.select;
			gb.joyp.left_b = gb.b;
			gb.joyp.right_a = gb.a;
		}
		else if (gb.joyp.action && !gb.joyp.direction)
		{
			gb.joyp.down_start = gb.down;
			gb.joyp.up_select = gb.up;
			gb.joyp.left_b = gb.left;
			gb.joyp.right_a = gb.right;
		}

		return gb.joyp.raw | 0b11110000;
}