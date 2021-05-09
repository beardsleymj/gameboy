#include "gb.h"
#include "cart.h"

void gb_init(gameboy* gb, char* rom_path)
{
	cart_load(gb, rom_path);

	// DMG mode
	gb->cpu_clock = DMG_CLOCK_FREQ;

	gb->last_sync_cycle = 0;
	gb->last_div_cycle = 0;
	gb->input_clock_select[0] = gb->cpu_clock / 1024;
	gb->input_clock_select[1] = gb->cpu_clock / 16;
	gb->input_clock_select[2] = gb->cpu_clock / 64;
	gb->input_clock_select[3] = gb->cpu_clock / 256;
	gb->last_tima_cycle = 0;
	gb->ime_delay = 0;
	gb->halt = 0;
	gb->stop = 0;
	gb->dma_transfer = 0;
	gb->mem[TIMA] = 0x00; // TIMA
	gb->mem[TMA] = 0x00; // TMA
	gb->mem[TAC] = 0x00; // TAC
	if (!load_bootrom(gb))
	{
		gb->bootrom_disabled = 0;
		gb->PC = 0;
		gb->SP = 0;
		gb->r.AF = 0;
		gb->r.BC = 0;
		gb->r.DE = 0;
		gb->r.HL = 0;
	}
	else // initialize registers
	{
		gb->bootrom_disabled = 1;
		gb->PC = 0x100;
		gb->SP = 0xFFFE;
		gb->r.AF = 0x01B0;
		gb->r.BC = 0x0013;
		gb->r.DE = 0x00D8;
		gb->r.HL = 0x014D;
		gb->mem[0xFF10] = 0x80; // NR10
		gb->mem[0xFF11] = 0xBF; // NR11
		gb->mem[0xFF12] = 0xF3; // NR12
		gb->mem[0xFF14] = 0xBF; // NR14
		gb->mem[0xFF16] = 0x3F; // NR21
		gb->mem[0xFF17] = 0x00; // NR22
		gb->mem[0xFF19] = 0xBF; // NR24
		gb->mem[0xFF1A] = 0x7F; // NR30
		gb->mem[0xFF1B] = 0xFF; // NR31
		gb->mem[0xFF1C] = 0x9F; // NR32
		gb->mem[0xFF1E] = 0xBF; // NR34
		gb->mem[0xFF20] = 0xFF; // NR41
		gb->mem[0xFF21] = 0x00; // NR42
		gb->mem[0xFF22] = 0x00; // NR43
		gb->mem[0xFF23] = 0xBF; // NR44
		gb->mem[0xFF24] = 0x77; // NR50
		gb->mem[0xFF25] = 0xF3; // NR51
		gb->mem[0xFF26] = 0xF1; // NR52
		gb->mem[LCDC] = 0x91; // LCDC
		gb->mem[0xFF42] = 0X00; // SCY
		gb->mem[0xFF43] = 0x00; // SCX
		//gb->mem[0xFF44] = 0x90; // LY (LCDC Y-Coordinate) hack - delete this line later
		gb->mem[0xFF45] = 0x00; // LYC
		gb->mem[0xFF47] = 0xFC; // BGP
		gb->mem[0xFF48] = 0xFF; // OBP0
		gb->mem[0xFF49] = 0xFF; // OBP1
		gb->mem[0xFF4A] = 0x00; // WY
		gb->mem[0xFF4B] = 0x00; // WX
		gb->mem[INTERRUPT_ENABLE] = 0x00; // IE
	}
	gb->lcdc = &gb->mem[LCDC];
	gb->stat = &gb->mem[STAT];
	gb->interrupt_enable = &gb->mem[INTERRUPT_ENABLE];
	gb->interrupt_flag = &gb->mem[INTERRUPT_FLAG];

	gb->last_sync_time = get_time_nano();
	gb->cycles = 0;
	gb->ppu_cycles = 0;
	gb->ppu_next_mode = 2;
	gb->ppu_line_cycles = 0;

	// ppu
	gb->oam_buffer_size = 0;
	gb->oam_fifo_pos = 0;
	gb->oam_fifo_size = 0;
	gb->background_fifo_pos = 0;
	gb->background_fifo_size = 0;
	gb->tile_number = 0;
	gb->window_internal_line_counter = 0;
	gb->window_draw_flag = 0;
	gb->vblanks = 0;
	gb->draw_frame = 0;


	// joypad
	gb->mem[JOYP] = 0XFF;
	gb->joypad = &gb->mem[JOYP];
	gb->up = 1;
	gb->down = 1;
	gb->left = 1;
	gb->right = 1;
	gb->a = 1;
	gb->b = 1;
	gb->start = 1;
	gb->select = 1;
}

int load_bootrom(gameboy* gb)
{
	FILE* bootrom = NULL;
	bootrom = fopen("bootrom\\dmg01.gb", "rb");
	if (bootrom == NULL)
	{
		printf("No boot rom detected\n");
		return 1;
	}
	else
	{
		fread(&gb->bootrom, 0x100, 1, bootrom);
		fclose(bootrom);
		return 0;
	}
}
