#include "gb_system.h"
#include "mbc/cart.h"
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
		apu.nr10.raw = 0x80;
		apu.nr11.raw = 0xBF;
		apu.nr12.raw = 0xF3;
		apu.nr1314.lo = 0xFF;
		apu.nr1314.hi = 0xBF;
		apu.nr21.raw = 0x3F;
		apu.nr22.raw = 0xFF;
		apu.nr2324.lo = 0xFF;
		apu.nr2324.hi = 0xBF;
		apu.nr30.raw = 0x7F;
		apu.nr31 = 0xFF;
		apu.nr32.raw = 0x9F;
		apu.nr3334.lo = 0xFF;
		apu.nr3334.hi = 0xBF;
		apu.nr41.raw = 0xFF;
		apu.nr42.raw = 0x00;
		apu.nr43.raw = 0x00;
		apu.nr44.raw = 0xBF;
		apu.nr50.raw = 0xFF;
		apu.nr51.raw = 0xF3;
		apu.nr52.raw = 0xF1;
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
	gb.ppu_cycles = 0;
	gb.ppu_next_mode = 2;

	// ppu
	ppu.oam_buffer_size = 0;
	ppu.window_internal_line_counter = 0;
	ppu.window_draw_flag = 0;
	ppu.vblanks = 0;
	ppu.draw_frame = 0;

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
