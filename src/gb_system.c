#include "gb_system.h"
#include "mbc/cart.h"
#include "bus.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"

void gb_init(char* rom_path, bool bootrom)
{
	cart_load(rom_path);

	gb.last_sync_cycle = 0;
	gb.last_div_cycle = 0;
	gb.last_tima_cycle = 0;
	gb.ime_delay = 0;
	gb.halt = 0;
	gb.stop = 0;
	cpu.tima = 0x00;
	cpu.tima = 0x00;
	cpu.tac.raw = 0x00;
	if (bootrom && !load_bootrom())
	{
		gb.bootrom_disabled = 0;
		cpu.PC = 0;
		cpu.SP = 0;
		cpu.r.AF = 0;
		cpu.r.BC = 0;
		cpu.r.DE = 0;
		cpu.r.HL = 0;
	}
	else // initialize registers
	{
		gb.bootrom_disabled = 1;
		cpu.PC = 0x100;
		cpu.SP = 0xFFFE;
		cpu.r.AF = 0x01B0;
		cpu.r.BC = 0x0013;
		cpu.r.DE = 0x00D8;
		cpu.r.HL = 0x014D;

		// Causes horrible screeching noise
		//write_io(NR10, 0x80);
		//write_io(NR11, 0xBF);
		//write_io(NR12, 0xF3);
		//write_io(NR13, 0xFF);
		//write_io(NR14, 0xBF);
		//write_io(NR21, 0x3F);
		//write_io(NR22, 0xFF);
		//write_io(NR23, 0xFF);
		//write_io(NR24, 0xBF);
		//write_io(NR30, 0x7F);
		//write_io(NR31, 0xFF);
		//write_io(NR32, 0x9F);
		//write_io(NR33, 0xFF);
		//write_io(NR34, 0xBF);
		//write_io(NR41, 0xFF);
		//write_io(NR42, 0x00);
		//write_io(NR43, 0x00);
		//write_io(NR44, 0xBF);
		//write_io(NR50, 0xFF);
		//write_io(NR51, 0xF3);
		//write_io(NR52, 0xF1);

		ppu.lcdc.raw = 0x91;
		ppu.scy = 0x00;
		ppu.scx = 0x00;
		ppu.ly = 0x91;
		ppu.lyc = 0x00;
		ppu.dma = 0xFF;
		ppu.bgp = 0xFC;
		ppu.obp0 = 0xFF;
		ppu.obp1 = 0xFF;
		ppu.wx = 0x00;
		ppu.wy = 0x00;
		
		cpu.interrupt_enable.raw = 0x00;
	}

	gb.cycles = 0;
	ppu.cycles = 0;
	ppu.next_mode = 2;

	cpu.wram_bank = 1;

	gb.cbg_mode = cart.cgb_flag;

	// ppu
	ppu.oam_buffer_size = 0;
	ppu.window_internal_line_counter = 0;
	ppu.window_draw_flag = 0;
	ppu.vblanks = 0;
	ppu.draw_frame = 0;

	// apu
	apu_init();

	// joypad
	gb.joyp.raw = 0XFF;
	gb.up = 1;
	gb.down = 1;
	gb.left = 1;
	gb.right = 1;
	gb.a = 1;
	gb.b = 1;
	gb.start = 1;
	gb.select = 1;
}

int load_bootrom()
{
	FILE* bootrom = NULL;
	//bootrom = fopen("bootrom\\bootix_dmg.bin", "rb");
	bootrom = fopen("bootrom\\dmg01.gb", "rb");
	if (bootrom == NULL)
	{
		printf("No boot rom detected\n");
		return 1;
	}
	else
	{
		fread(&gb.bootrom, 0x100, 1, bootrom);
		fclose(bootrom);
		return 0;
	}
}
