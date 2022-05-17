#include "bus.h"
#include "mbc/cart.h"
#include "ppu.h"
#include "apu.h"
#include "cpu.h"
#include "timers.h"
#include "event.h"

u8 read_byte(u16 address) 
{
	gb.cycles += 4;

	if (gb.bootrom_disabled == 0 && address < 0x100) return gb.bootrom[address];
	if (address >= 0x0000 && address <= 0x7FFF)	return cart_read_byte(address);
	if (address >= 0x8000 && address <= 0x9FFF)	return ppu_read_byte(address);
	if (address >= 0xA000 && address <= 0xBFFF)	return cart_read_byte(address);
	if (address >= 0xC000 && address <= 0xFDFF)	return cpu_read_wram_byte(address);
	if (address >= 0xFE00 && address <= 0xFE9F)	return ppu.oam[address & 0xFF];
	if (address >= 0xFF00 && address <= 0xFF7F)	return read_io(address);
	if (address >= 0xFF80 && address <= 0xFFFE)	return cpu.hram[address & 0x7F];
	if (address == INTERRUPT_ENABLE)			return cpu.interrupt_enable.raw;
	if (address == 0xFF7F)						return 0xFF; // UNUSED Memory Mapped IO returns OxFF, add more
	printf("Unimplemented Read Address: %X\n", address);
	return 0xFF;
}

// unmapped io bits / regs will return 1 for now
u8 read_io(u16 address)
{
	switch (address)
	{
		case NR10:
			return ((apu.square1.sweep_shift & 0b111) << 0)
				 | ((apu.square1.sweep_direction & 1) << 3)
				 | ((apu.square1.sweep_period & 0b111) << 4)
				 | 0b10000000;
			break;

		case NR11:
			return ((apu.square1.duty & 0b11) << 6)
				 | 0b00111111;
			break;

		case NR12:
			return ((apu.square1.envelope_period & 0b111) << 0)
				 | ((apu.square1.envelope_direction & 1) << 3)
				 | ((apu.square1.envelope_volume & 0b1111) << 4);
			break;

		case NR13:
			return 0xFF;
			break;

		case NR14:
			return ((apu.square1.counter << 6) & 1)
				 | 0b10111111;
			break;

		case NR21:
			return ((apu.square2.duty & 0b11) << 6)
				 | 0b00111111;
			break;

		case NR22:
			return ((apu.square2.envelope_period & 0b111) << 0)
				 | ((apu.square2.envelope_direction & 1) << 3)
				 | ((apu.square2.envelope_volume & 0b1111) << 4);
			break;

		case NR23:
			return 0xFF;
			break;

		case NR24:
			return ((apu.square2.counter & 1) << 6)
				 | 0b10111111;
			break;

		case NR30:
			return ((apu.wave.dac_enable & 1) << 7)
				 | 0b01111111;
			break;

		case NR31:
			return 0xFF;
			break;

		case NR32:
			return ((apu.wave.volume & 0B11) << 5)
				 | 0b10011111;
			break;

		case NR33:
			return 0xFF;
			break;

		case NR34:
			return ((apu.wave.counter & 1) << 6)
				 | 0b10111000;
			break;

		case 0xFF30: case 0xFF31: case 0xFF32: case 0xFF33:
		case 0xFF34: case 0xFF35: case 0xFF36: case 0xFF37:
		case 0xFF38: case 0xFF39: case 0xFF3A: case 0xFF3B:
		case 0xFF3C: case 0xFF3D: case 0xFF3E: case 0xFF3F:
			return apu.wave.ram[(address & 0xF) * 2] << 4
				| apu.wave.ram[(address & 0xF) * 2 + 1] << 0;

		case NR41:
			return 0xFF;
			break;

		case NR42:
			return ((apu.noise.envelope_period & 0b111) << 0)
				 | ((apu.noise.envelope_direction & 1) << 3)
				 | ((apu.noise.envelope_volume & 0b1111) << 4);
			break;

		case NR43:
			return ((apu.noise.divisor & 0b111) << 0)
				 | ((apu.noise.counter & 1) << 3)
				 | ((apu.noise.frequency & 0b1111) << 4);
			break;

		case NR44:
			return ((apu.noise.counter & 1) << 6)
				 | 0b10111111;
			break;

		case NR50:
			return ((apu.sequencer.right_volume & 0b111) << 0)
				 | ((apu.sequencer.right_enable & 1) << 3)
				 | ((apu.sequencer.left_volume & 0b111) << 4)
				 | ((apu.sequencer.left_enable & 1) << 7);
			break;

		case NR51:
			return ((apu.square1.enable_right & 1) << 0)
				 | ((apu.square2.enable_right & 1) << 1)
				 | ((apu.wave.enable_right	  & 1) << 2)
				 | ((apu.noise.enable_right	  & 1) << 3)
				 | ((apu.square1.enable_left  & 1) << 4)
				 | ((apu.square2.enable_left  & 1) << 5)
				 | ((apu.wave.enable_left     & 1) << 6)
				 | ((apu.noise.enable_left    & 1) << 7);
			break;

		case NR52:
			return ((apu.square1.enable   & 1) << 0)
				 | ((apu.square2.enable   & 1) << 1)
				 | ((apu.wave.enable      & 1) << 2)
				 | ((apu.noise.enable     & 1) << 3)
				 | ((apu.sequencer.enable & 1) << 7)
				 | 0b01110000;
			break;

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
			return cpu.tima;
			break;

		case TAC:
			return cpu.tac.raw | 0b11111000;
			break;

		case INTERRUPT_FLAG:
			return cpu.interrupt_flag.raw | 0b11100000;
			break;

		case LCDC:
			return ppu.lcdc.raw;
			break;

		case STAT:
			return ppu.stat.raw | 0b10000000;
			break;

		case SCY:
			return ppu.scy;
			break;

		case SCX:
			return ppu.scx;
			break;

		case LYC:
			return ppu.lyc;
			break;

		case DMA:
			return ppu.dma;
			break;

		case BGP:
			return ppu.bgp;
			break;

		case OBP0:
			return ppu.obp0;
			break;

		case OBP1:
			return ppu.obp1;
			break;

		case WY:
			return ppu.wy;
			break;

		case WX:
			return ppu.wx;
			break;

		case LY:
			return ppu.ly;
			break;

		case 0xFF68:
			return 0xFF;
			break;
		
		case 0xFF4D:
			return cpu.double_speed << 7 | 0b01111110 | cpu.prepare_speed_switch;
			break;

		case 0xFF4F:
			return ppu.vram_bank;
			break;

		case 0xFF55:
			return ppu.transfer_length | ppu.hdma_transfer_is_active << 7;
			break;

		case 0xFF6C:
			return ppu.obj_prio_mode;
			break;

		case 0xFF70:
			return cpu.wram_bank;
			break;

		default:
			if (address >= 0xFEA0 && address <= 0xFEFF)
				return 0xFF; // prohibited
			return 0xFF;
			break;
	}
}

void write_byte(u16 address, u8 value)
{
	gb.cycles += 4;

	if		(address <= 0x7FFF)							cart_write_byte(address, value);
	else if (address >= 0x8000 && address <= 0x9FFF)	ppu_write_byte(address, value);
	else if (address >= 0xA000 && address <= 0xBFFF)	cart_write_byte(address, value);
	else if (address >= 0xC000 && address <= 0xFDFF)	cpu_write_wram_byte(address, value);
	else if (address >= 0xFE00 && address <= 0xFE9F)	ppu.oam[address - 0xFE00] = value;
	else if (address >= 0xFEA0 && address <= 0xFEFF)	printf("write: illegal write to prohibited area: %x\n", address);
	else if (address >= 0xFF00 && address <= 0xFF7F)	write_io(address, value);
	else if (address >= 0xFF80 && address <= 0xFFFE)	cpu.hram[address - 0xFF80] = value;
	else if (address == INTERRUPT_ENABLE)				cpu.interrupt_enable.raw = value;
	else
		printf("Unimplemented Write Address: %X\n", address);
}

void write_io(u16 address, u8 value)
{
	switch (address)
	{
		case NR10:
			apu.square1.sweep_shift = (value >> 0) & 0b111;
			apu.square1.sweep_direction = (value >> 3) & 1;
			apu.square1.sweep_period = (value >> 4) & 0b111;
			break;

		case NR11:
			apu.square1.length_timer = 64 - (value >> 0) & 0b111111;
			apu.square1.duty = (value >> 6) & 0b11;
			break;

		case NR12:
			apu.square1.dac_enable = (value & 0b11111000) != 0; // clear top 5 bits enable dac
			if (!apu.square1.dac_enable) apu.square1.enable = false;
			apu.square1.envelope_period = (value >> 0) & 0b111;
			apu.square1.envelope_direction = (value >> 3) & 1;
			apu.square1.envelope_volume = (value >> 4) & 0b1111;
			break;

		case NR13:
			apu.square1.frequency = (apu.square1.frequency & 0b11100000000) | value;
			break;

		case NR14:
			apu.square1.frequency = (apu.square1.frequency & 0xFF) | ((value & 0b111) << 8);
			apu.square1.counter = (value >> 6) & 0b1;
			if ((value >> 7) & 1 == 1)
				square1_trigger();
			break;

		case NR21:
			apu.square2.length_timer = 64 - (value >> 0) & 0b111111;
			apu.square2.duty = (value >> 6) & 0b11;
			break;

		case NR22:
			apu.square2.dac_enable = (value & 0b11111000) != 0; // clear top 5 bits enable dac
			if (!apu.square2.dac_enable) apu.square2.enable = false;
			apu.square2.envelope_period = (value >> 0) & 0b111;
			apu.square2.envelope_direction = (value >> 3) & 1;
			apu.square2.envelope_volume = (value >> 4) & 0b1111;
			break;

		case NR23:
			apu.square2.frequency = (apu.square2.frequency & 0b11100000000) | value;
			break;

		case NR24:
			apu.square2.frequency = (apu.square2.frequency & 0xFF) | ((value & 0b111) << 8);
			apu.square2.counter = (value >> 6) & 1;
			if ((value >> 7) & 1 == 1)
				square2_trigger();			
			break;

		case NR30:
			apu.wave.dac_enable = (value >> 7);
			break;

		case NR31:
			apu.wave.length_timer = 256 - value;
			break;

		case NR32:
			apu.wave.volume = (value >> 5) & 0b11;
			break;

		case NR33:
			apu.wave.frequency = (apu.wave.frequency & 0b11100000000) | value;
			break;

		case NR34:
			apu.wave.frequency = (apu.wave.frequency & 0xFF) | ((value & 0b111) << 8);
			apu.wave.counter = (value >> 6) & 1;
			if ((value >> 7) & 1 == 1)
				wave_trigger();
			break;

		case 0xFF30: case 0xFF31: case 0xFF32: case 0xFF33:
		case 0xFF34: case 0xFF35: case 0xFF36: case 0xFF37:
		case 0xFF38: case 0xFF39: case 0xFF3A: case 0xFF3B:
		case 0xFF3C: case 0xFF3D: case 0xFF3E: case 0xFF3F:
			apu.wave.ram[(address & 0xF) * 2] = value >> 4;
			apu.wave.ram[(address & 0xF) * 2 + 1] = value & 0b1111;
			break;

		case NR41:
			apu.noise.length_timer = 64 - (value & 0b111111);
			break;

		case NR42:
			apu.noise.dac_enable = (value & 0b11111000) != 0; // clear top 5 bits enable dac
			if (!apu.noise.dac_enable) apu.noise.enable = false;
			apu.noise.envelope_period = (value >> 0) & 0b111;
			apu.noise.envelope_direction = (value >> 3) & 1;
			apu.noise.envelope_volume = (value >> 4) & 0b1111;
			break;

		case NR43:
			apu.noise.divisor = (value >> 0) & 0b11;
			apu.noise.lfsr_width = (value >> 3) & 0b1;
			apu.noise.frequency = (value >> 4) & 0b1111;
			break;

		case NR44:
			apu.noise.counter = (value >> 6) & 1;
			if ((value >> 7) & 1 == 1)
				noise_trigger();
			break;

		case NR50:
			apu.sequencer.right_volume = (value >> 0) & 0b111;
			apu.sequencer.right_enable = (value >> 3) & 1;
			apu.sequencer.left_volume  = (value >> 4) & 0b111;
			apu.sequencer.left_enable  = (value >> 7) & 1;
			break;

		case NR51:
			apu.square1.enable_right = (value >> 0) & 1;
			apu.square2.enable_right = (value >> 1) & 1;
			apu.wave.enable_right	 = (value >> 2) & 1;
			apu.noise.enable_right	 = (value >> 3) & 1;
			apu.square1.enable_left  = (value >> 4) & 1;
			apu.square2.enable_left	 = (value >> 5) & 1;
			apu.wave.enable_left	 = (value >> 6) & 1;
			apu.noise.enable_left	 = (value >> 7) & 1;
			break;

		case NR52:
			if (apu.sequencer.enable != (value >> 7) & 1)
			{
				apu.sequencer.enable = (value >> 7) & 1;

				// add code here to reset the channels / sequencer
			}
			break;

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
			break;

		case TMA:
			cpu.tima = value;
			break;

		case TAC:
			write_tac(value);
			break;

		case INTERRUPT_FLAG:
			cpu.interrupt_flag.raw = value;
			break;

		case LCDC:
			ppu.lcdc.raw = value;
			break;

		case STAT:
			ppu.stat.raw = (value & 0b11111000) | (ppu.stat.raw & 0b00000111);
			break;

		case SCY:
			ppu.scy = value;
			break;

		case SCX:
			ppu.scx = value;
			break;

		case LYC:
			ppu.lyc = value;
			break;

		case DMA:
			ppu.dma = value;
			dma_transfer();
			break;

		case BGP:
			ppu.bgp = value;
			break;

		case OBP0:
			ppu.obp0 = value;
			break;

		case OBP1:
			ppu.obp1 = value;
			break;

		case 0xFF50:
			gb.bootrom_disabled = 1;
			break;

		case WY:
			ppu.wy = value;
			break;

		case WX:
			ppu.wx = value;
			break;

		case LY: // read only
			break;

		case 0xFF4D:
			cpu.prepare_speed_switch = value & 1;
			break;

		case 0xFF4F:
			ppu.vram_bank = value & 1;
			break;

		case 0xFF51:
			ppu.hdma1_2 = (value << 8) | (ppu.hdma1_2 & 0xFF);
			break;

		case 0xFF52: // lower 4 bits of address are ignored
			ppu.hdma1_2 = (ppu.hdma1_2 & 0xFF00) | (value & 0xF0);
			break;

		case 0xFF53:
			ppu.hdma3_4 = (value << 8) | (ppu.hdma3_4 & 0xFF);
			break;

		case 0xFF54: // lower 4 bits of address are ignored
			ppu.hdma3_4 = (ppu.hdma3_4 & 0xFF00) | (value & 0xF0);
			break;

		case 0xFF55:
			ppu.transfer_length = value & 0x7F;
			ppu.transfer_mode = value >> 7;
			break;

		case 0xFF6C:
			ppu.obj_prio_mode = value & 1;
			break;

		case 0xFF70:
			if (gb.cbg_mode = true)
				cpu.wram_bank = value == 0 ? 1 : value & 0x07;
			else
				cpu.wram_bank = 1;
			break;

		default:
				printf("Write to unimplemented MMIO: %X\n", address);
			break;
	}
}

u16 read_word(u16 address)
{
	return (read_byte(address + 1) << 8) |  read_byte(address);
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