#include "gb_system.h"
#include "mbc/cart.h"
#include "cpu.h"
#include "ppu.h"

void gb_init(char* rom_path, bool bootrom)
{
	cart_load(rom_path);

	gb.last_sync_cycle = 0;
	gb.last_div_cycle = 0;
	gb.last_tima_cycle = 0;
	gb.ime_delay = 0;
	gb.halt = 0;
	gb.stop = 0;
	gb.tma = 0x00;
	gb.tima = 0x00;
	gb.tac.raw = 0x00;
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
		gb.audio[0xFF10 - 0xFF10] = 0x80; // NR10
		gb.audio[0xFF11 - 0xFF10] = 0xBF; // NR11
		gb.audio[0xFF12 - 0xFF10] = 0xF3; // NR12
		gb.audio[0xFF14 - 0xFF10] = 0xBF; // NR14
		gb.audio[0xFF16 - 0xFF10] = 0x3F; // NR21
		gb.audio[0xFF17 - 0xFF10] = 0x00; // NR22
		gb.audio[0xFF19 - 0xFF10] = 0xBF; // NR24
		gb.audio[0xFF1A - 0xFF10] = 0x7F; // NR30
		gb.audio[0xFF1B - 0xFF10] = 0xFF; // NR31
		gb.audio[0xFF1C - 0xFF10] = 0x9F; // NR32
		gb.audio[0xFF1E - 0xFF10] = 0xBF; // NR34
		gb.audio[0xFF20 - 0xFF10] = 0xFF; // NR41
		gb.audio[0xFF21 - 0xFF10] = 0x00; // NR42
		gb.audio[0xFF22 - 0xFF10] = 0x00; // NR43
		gb.audio[0xFF23 - 0xFF10] = 0xBF; // NR44
		gb.audio[0xFF24 - 0xFF10] = 0x77; // NR50
		gb.audio[0xFF25 - 0xFF10] = 0xF3; // NR51
		gb.audio[0xFF26 - 0xFF10] = 0xF1; // NR52
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
	gb.ppu_line_cycles = 0;

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