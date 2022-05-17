#include "cpu.h"
#include "cpu_instr.h"
#include "gb_system.h"
#include "bus.h"

#include <string.h>

cpu_t cpu;

void cpu_run()
{
	// Enabling IME is delayed 1 cycle
	if (gb.ime_delay == 1) 
	{
		gb.IME = false;
		gb.ime_delay++;
	}
	else if (gb.ime_delay == 2) 
	{
		gb.IME = true;
		gb.ime_delay = 0;
	}
	
	// halt
	if (gb.halt) 
	{
		if (cpu.interrupt_enable.raw && cpu.interrupt_flag.raw)
			gb.halt = 0;
		else 
		{
			gb.cycles += 4;
			return;
		}
	}

	handle_interrupts();

	execute_instruction();
}

void handle_interrupts()
{
	if (gb.IME && (cpu.interrupt_enable.raw & cpu.interrupt_flag.raw))
	{
		write_byte(--cpu.SP, (cpu.PC >> 8));
		write_byte(--cpu.SP, cpu.PC & 0xFF);
		gb.cycles += 12;
		gb.IME = false;

		if (cpu.interrupt_enable.vblank && cpu.interrupt_flag.vblank)
		{
			cpu.PC = 0x40;
			cpu.interrupt_flag.vblank = 0;
			return;
		}
		
		if (cpu.interrupt_enable.stat && cpu.interrupt_flag.stat)
		{
			cpu.PC = 0x48;
			cpu.interrupt_flag.stat = 0;
			return;
		}
		
		if (cpu.interrupt_enable.timer && cpu.interrupt_flag.timer)
		{
			cpu.PC = 0x50;
			cpu.interrupt_flag.timer = 0;
			return;
		}
		
		if (cpu.interrupt_enable.serial && cpu.interrupt_flag.serial)
		{
			cpu.PC = 0x58;
			cpu.interrupt_flag.serial = 0;
			return;
		}
		
		if (cpu.interrupt_enable.joypad && cpu.interrupt_flag.joypad)
		{
			cpu.PC = 0x60;
			cpu.interrupt_flag.joypad = 0;
			return;
		}
	}
}

void execute_instruction()
{
	u8 opcode = read_byte(cpu.PC++);

	switch (opcode)
	{
		// 8-bit Load Instructions
		case 0x40: cpu.r.B = cpu.r.B; break;
		case 0x41: cpu.r.B = cpu.r.C; break;
		case 0x42: cpu.r.B = cpu.r.D; break;
		case 0x43: cpu.r.B = cpu.r.E; break;
		case 0x44: cpu.r.B = cpu.r.H; break;
		case 0x45: cpu.r.B = cpu.r.L; break;
		case 0x46: cpu.r.B = read_byte(cpu.r.HL); break;
		case 0x47: cpu.r.B = cpu.r.A; break;

		case 0x48: cpu.r.C = cpu.r.B; break;
		case 0x49: cpu.r.C = cpu.r.C; break;
		case 0x4A: cpu.r.C = cpu.r.D; break;
		case 0x4B: cpu.r.C = cpu.r.E; break;
		case 0x4C: cpu.r.C = cpu.r.H; break;
		case 0x4D: cpu.r.C = cpu.r.L; break;
		case 0x4E: cpu.r.C = read_byte(cpu.r.HL); break;
		case 0x4F: cpu.r.C = cpu.r.A; break;

		case 0x50: cpu.r.D = cpu.r.B; break;
		case 0x51: cpu.r.D = cpu.r.C; break;
		case 0x52: cpu.r.D = cpu.r.D; break;
		case 0x53: cpu.r.D = cpu.r.E; break;
		case 0x54: cpu.r.D = cpu.r.H; break;
		case 0x55: cpu.r.D = cpu.r.L; break;
		case 0x56: cpu.r.D = read_byte(cpu.r.HL); break;
		case 0x57: cpu.r.D = cpu.r.A; break;

		case 0x58: cpu.r.E = cpu.r.B; break;
		case 0x59: cpu.r.E = cpu.r.C; break;
		case 0x5A: cpu.r.E = cpu.r.D; break;
		case 0x5B: cpu.r.E = cpu.r.E; break;
		case 0x5C: cpu.r.E = cpu.r.H; break;
		case 0x5D: cpu.r.E = cpu.r.L; break;
		case 0x5E: cpu.r.E = read_byte(cpu.r.HL); break;
		case 0x5F: cpu.r.E = cpu.r.A; break;

		case 0x60: cpu.r.H = cpu.r.B; break;
		case 0x61: cpu.r.H = cpu.r.C; break;
		case 0x62: cpu.r.H = cpu.r.D; break;
		case 0x63: cpu.r.H = cpu.r.E; break;
		case 0x64: cpu.r.H = cpu.r.H; break;
		case 0x65: cpu.r.H = cpu.r.L; break;
		case 0x66: cpu.r.H = read_byte(cpu.r.HL); break;
		case 0x67: cpu.r.H = cpu.r.A; break;

		case 0x68: cpu.r.L = cpu.r.B; break;
		case 0x69: cpu.r.L = cpu.r.C; break;
		case 0x6A: cpu.r.L = cpu.r.D; break;
		case 0x6B: cpu.r.L = cpu.r.E; break;
		case 0x6C: cpu.r.L = cpu.r.H; break;
		case 0x6D: cpu.r.L = cpu.r.L; break;
		case 0x6E: cpu.r.L = read_byte(cpu.r.HL); break;
		case 0x6F: cpu.r.L = cpu.r.A; break;

		case 0x70: write_byte(cpu.r.HL, cpu.r.B); break;
		case 0x71: write_byte(cpu.r.HL, cpu.r.C); break;
		case 0x72: write_byte(cpu.r.HL, cpu.r.D); break;
		case 0x73: write_byte(cpu.r.HL, cpu.r.E); break;
		case 0x74: write_byte(cpu.r.HL, cpu.r.H); break;
		case 0x75: write_byte(cpu.r.HL, cpu.r.L); break;
		case 0x77: write_byte(cpu.r.HL, cpu.r.A); break;

		case 0x78: cpu.r.A = cpu.r.B; break;
		case 0x79: cpu.r.A = cpu.r.C; break;
		case 0x7A: cpu.r.A = cpu.r.D; break;
		case 0x7B: cpu.r.A = cpu.r.E; break;
		case 0x7C: cpu.r.A = cpu.r.H; break;
		case 0x7D: cpu.r.A = cpu.r.L; break;
		case 0x7E: cpu.r.A = read_byte(cpu.r.HL); break;
		case 0x7F: cpu.r.A = cpu.r.A; break;

		case 0x02: write_byte(cpu.r.BC, cpu.r.A); break;
		case 0x12: write_byte(cpu.r.DE, cpu.r.A); break;
		case 0x22: write_byte(cpu.r.HL++, cpu.r.A); break;
		case 0x32: write_byte(cpu.r.HL--, cpu.r.A); break;

		case 0x06: cpu.r.B = read_byte(cpu.PC++); break;
		case 0x0E: cpu.r.C = read_byte(cpu.PC++); break;
		case 0x16: cpu.r.D = read_byte(cpu.PC++); break;
		case 0x1E: cpu.r.E = read_byte(cpu.PC++); break;
		case 0x26: cpu.r.H = read_byte(cpu.PC++); break;
		case 0x2E: cpu.r.L = read_byte(cpu.PC++); break;
		case 0x36: write_byte(cpu.r.HL, read_byte(cpu.PC++)); break;
		case 0x3E: cpu.r.A = read_byte(cpu.PC++); break;

		case 0x0A: cpu.r.A = read_byte(cpu.r.BC); break;
		case 0x1A: cpu.r.A = read_byte(cpu.r.DE); break;
		case 0x2A: cpu.r.A = read_byte(cpu.r.HL++); break;
		case 0x3A: cpu.r.A = read_byte(cpu.r.HL--); break;

		case 0xE0: write_byte(0xFF00 + read_byte(cpu.PC++), cpu.r.A); break;
		case 0xF0: cpu.r.A = read_byte(0xFF00 + read_byte(cpu.PC++)); break;
		case 0xE2: write_byte(0xFF00 + cpu.r.C, cpu.r.A); break;
		case 0xF2: cpu.r.A = read_byte(0xFF00 + cpu.r.C); break;
		case 0xEA: write_byte(read_word(cpu.PC), cpu.r.A); cpu.PC += 2; break;
		case 0xFA: cpu.r.A = read_byte(read_word(cpu.PC)); cpu.PC += 2; break;

			// 16-bit Load Instructions
		case 0x01: cpu.r.BC = read_byte(cpu.PC++) | (read_byte(cpu.PC++) << 8); break;
		case 0x11: cpu.r.DE = read_byte(cpu.PC++) | (read_byte(cpu.PC++) << 8); break;
		case 0x21: cpu.r.HL = read_byte(cpu.PC++) | (read_byte(cpu.PC++) << 8); break;
		case 0xF8: ld_hl_sp_i8(); break;
		case 0x31: cpu.SP = read_byte(cpu.PC++) | (read_byte(cpu.PC++) << 8); break;
		case 0xF9: cpu.SP = cpu.r.HL; gb.cycles += 4; break;
		case 0x08: ld_a16_sp(); break;
		case 0xC5: push(&cpu.r.BC); break;
		case 0xD5: push(&cpu.r.DE); break;
		case 0xE5: push(&cpu.r.HL); break;
		case 0xF5: push(&cpu.r.AF); break;

		case 0xC1: pop(&cpu.r.BC); break;
		case 0xD1: pop(&cpu.r.DE); break;
		case 0xE1: pop(&cpu.r.HL); break;
		case 0xF1: pop_af(); break;

		case 0x80: add(cpu.r.B); break;
		case 0x81: add(cpu.r.C); break;
		case 0x82: add(cpu.r.D); break;
		case 0x83: add(cpu.r.E); break;
		case 0x84: add(cpu.r.H); break;
		case 0x85: add(cpu.r.L); break;
		case 0x86: add(read_byte(cpu.r.HL)); break;
		case 0x87: add(cpu.r.A); break;
		case 0xC6: add(read_byte(cpu.PC++)); break;
		case 0x09: add_16(cpu.r.BC); break;
		case 0x19: add_16(cpu.r.DE); break;
		case 0x29: add_16(cpu.r.HL); break;
		case 0x39: add_16(cpu.SP); break;
		case 0xE8: add_sp_i8();	break;

		case 0x88: adc(cpu.r.B); break;
		case 0x89: adc(cpu.r.C); break;
		case 0x8A: adc(cpu.r.D); break;
		case 0x8B: adc(cpu.r.E); break;
		case 0x8C: adc(cpu.r.H); break;
		case 0x8D: adc(cpu.r.L); break;
		case 0x8E: adc(read_byte(cpu.r.HL)); break;
		case 0x8F: adc(cpu.r.A); break;
		case 0xCE: adc(read_byte(cpu.PC++)); break;

		case 0x90: sub(cpu.r.B); break;
		case 0x91: sub(cpu.r.C); break;
		case 0x92: sub(cpu.r.D); break;
		case 0x93: sub(cpu.r.E); break;
		case 0x94: sub(cpu.r.H); break;
		case 0x95: sub(cpu.r.L); break;
		case 0x96: sub(read_byte(cpu.r.HL)); break;
		case 0x97: sub(cpu.r.A); break;
		case 0xD6: sub(read_byte(cpu.PC++)); break;

		case 0x98: sbc(cpu.r.B); break;
		case 0x99: sbc(cpu.r.C); break;
		case 0x9A: sbc(cpu.r.D); break;
		case 0x9B: sbc(cpu.r.E); break;
		case 0x9C: sbc(cpu.r.H); break;
		case 0x9D: sbc(cpu.r.L); break;
		case 0x9E: sbc(read_byte(cpu.r.HL)); break;
		case 0x9F: sbc(cpu.r.A); break;
		case 0xDE: sbc(read_byte(cpu.PC++)); break;

		case 0x07: rlca(); break;
		case 0x0F: rrca(); break;
		case 0x17: rla(); break;
		case 0x1F: rra(); break;

		case 0xA0: and (cpu.r.B); break;
		case 0xA1: and (cpu.r.C); break;
		case 0xA2: and (cpu.r.D); break;
		case 0xA3: and (cpu.r.E); break;
		case 0xA4: and (cpu.r.H); break;
		case 0xA5: and (cpu.r.L); break;
		case 0xA6: and (read_byte(cpu.r.HL)); break;
		case 0xA7: and (cpu.r.A); break;
		case 0xE6: and (read_byte(cpu.PC++)); break;

		case 0xA8: xor (cpu.r.B); break;
		case 0xA9: xor (cpu.r.C); break;
		case 0xAA: xor (cpu.r.D); break;
		case 0xAB: xor (cpu.r.E); break;
		case 0xAC: xor (cpu.r.H); break;
		case 0xAD: xor (cpu.r.L); break;
		case 0xAE: xor (read_byte(cpu.r.HL)); break;
		case 0xAF: xor (cpu.r.A); break;
		case 0xEE: xor (read_byte(cpu.PC++)); break;

		case 0xB0: or (cpu.r.B); break;
		case 0xB1: or (cpu.r.C); break;
		case 0xB2: or (cpu.r.D); break;
		case 0xB3: or (cpu.r.E); break;
		case 0xB4: or (cpu.r.H); break;
		case 0xB5: or (cpu.r.L); break;
		case 0xB6: or (read_byte(cpu.r.HL)); break;
		case 0xB7: or (cpu.r.A); break;
		case 0xF6: or (read_byte(cpu.PC++)); break;

		case 0xB8: cp(cpu.r.B); break;
		case 0xB9: cp(cpu.r.C); break;
		case 0xBA: cp(cpu.r.D); break;
		case 0xBB: cp(cpu.r.E); break;
		case 0xBC: cp(cpu.r.H); break;
		case 0xBD: cp(cpu.r.L); break;
		case 0xBE: cp(read_byte(cpu.r.HL)); break;
		case 0xBF: cp(cpu.r.A); break;
		case 0xFE: cp(read_byte(cpu.PC++)); break;
		case 0x04: inc_8(&cpu.r.B); break;
		case 0x14: inc_8(&cpu.r.D); break;
		case 0x24: inc_8(&cpu.r.H); break;
		case 0x34: inc_hl(); break;
		case 0x0C: inc_8(&cpu.r.C); break;
		case 0x1C: inc_8(&cpu.r.E); break;
		case 0x2C: inc_8(&cpu.r.L); break;
		case 0x3C: inc_8(&cpu.r.A); break;
		case 0x03: inc_16(&cpu.r.BC); break;
		case 0x13: inc_16(&cpu.r.DE); break;
		case 0x23: inc_16(&cpu.r.HL); break;
		case 0x33: inc_16(&cpu.SP); break;
		case 0x05: dec(&cpu.r.B); break;
		case 0x0D: dec(&cpu.r.C); break;
		case 0x15: dec(&cpu.r.D); break;
		case 0x1D: dec(&cpu.r.E); break;
		case 0x25: dec(&cpu.r.H); break;
		case 0x2D: dec(&cpu.r.L); break;
		case 0x35: dec_hl(); break;
		case 0x3D: dec(&cpu.r.A); break;
		case 0x0B: dec16(&cpu.r.BC); break;
		case 0x1B: dec16(&cpu.r.DE); break;
		case 0x2B: dec16(&cpu.r.HL); break;
		case 0x3B: dec16(&cpu.SP); break;
		case 0x27: daa(); break;
		case 0x2F: cpl(); break;
		case 0x18: jr(1); break;
		case 0x20: jr(!cpu.r.ZF); break;
		case 0x30: jr(!cpu.r.CF); break;
		case 0x28: jr(cpu.r.ZF); break;
		case 0x38: jr(cpu.r.CF); break;
		case 0xCB: prefix_cb(); break;
		case 0xC3: jp(true); break;
		case 0xC2: jp(!cpu.r.ZF); break;
		case 0xD2: jp(!cpu.r.CF); break;
		case 0xCA: jp(cpu.r.ZF); break;
		case 0xDA: jp(cpu.r.CF); break;
		case 0xE9: cpu.PC = cpu.r.HL; break;
		case 0xC4: call(!cpu.r.ZF); break;
		case 0xD4: call(!cpu.r.CF); break;
		case 0xCC: call(cpu.r.ZF); break;
		case 0xDC: call(cpu.r.CF); break;
		case 0xCD: call(true); break;
		case 0xC0: ret_f(!cpu.r.ZF); break;
		case 0xD0: ret_f(!cpu.r.CF); break;
		case 0xC8: ret_f(cpu.r.ZF); break;
		case 0xD8: ret_f(cpu.r.CF); break;
		case 0xC9: ret(true); break;
		case 0xD9: reti(); break;
		case 0xC7: rst(0x00); break;
		case 0xD7: rst(0x10); break;
		case 0xE7: rst(0x20); break;
		case 0xF7: rst(0x30); break;
		case 0xCF: rst(0x08); break;
		case 0xDF: rst(0x18); break;
		case 0xEF: rst(0x28); break;
		case 0xFF: rst(0x38); break;
		case 0x3F: ccf(); break;
		case 0x37: scf(); break;
		case 0x00: break; // noop
		case 0x10: printf("stop \n");  break; // STOP: TODO
		case 0x76: halt(); break;
		case 0xF3: di(); break;
		case 0xFB: ei(); break;
		default:
			printf("opcode %.2X not implemented\n", opcode);
			exit(0);
	}
}

void prefix_cb() 
{
	u8 opcode = read_byte(cpu.PC++);

	switch (opcode) 
	{
		// RLC
		case 0x00: cpu.r.B = rlc(cpu.r.B); break;
		case 0x01: cpu.r.C = rlc(cpu.r.C); break;
		case 0x02: cpu.r.D = rlc(cpu.r.D); break;
		case 0x03: cpu.r.E = rlc(cpu.r.E); break;
		case 0x04: cpu.r.H = rlc(cpu.r.H); break;
		case 0x05: cpu.r.L = rlc(cpu.r.L); break;
		case 0x06: write_byte(cpu.r.HL, rlc(read_byte(cpu.r.HL))); break;
		case 0x07: cpu.r.A = rlc(cpu.r.A); break;
		// RRC
		case 0x08: cpu.r.B = rrc(cpu.r.B); break;
		case 0x09: cpu.r.C = rrc(cpu.r.C); break;
		case 0x0A: cpu.r.D = rrc(cpu.r.D); break;
		case 0x0B: cpu.r.E = rrc(cpu.r.E); break;
		case 0x0C: cpu.r.H = rrc(cpu.r.H); break;
		case 0x0D: cpu.r.L = rrc(cpu.r.L); break;
		case 0x0E: write_byte(cpu.r.HL, rrc(read_byte(cpu.r.HL))); break;
		case 0x0F: cpu.r.A = rrc(cpu.r.A); break;
		// RL
		case 0x10: cpu.r.B = rl(cpu.r.B); break;
		case 0x11: cpu.r.C = rl(cpu.r.C); break;
		case 0x12: cpu.r.D = rl(cpu.r.D); break;
		case 0x13: cpu.r.E = rl(cpu.r.E); break;
		case 0x14: cpu.r.H = rl(cpu.r.H); break;
		case 0x15: cpu.r.L = rl(cpu.r.L); break;
		case 0x16: write_byte(cpu.r.HL, rl(read_byte(cpu.r.HL))); break;
		case 0x17: cpu.r.A = rl(cpu.r.A); break;
		// RR
		case 0x18: cpu.r.B = rr(cpu.r.B); break;
		case 0x19: cpu.r.C = rr(cpu.r.C); break;
		case 0x1A: cpu.r.D = rr(cpu.r.D); break;
		case 0x1B: cpu.r.E = rr(cpu.r.E); break;
		case 0x1C: cpu.r.H = rr(cpu.r.H); break;
		case 0x1D: cpu.r.L = rr(cpu.r.L); break;
		case 0x1E: write_byte(cpu.r.HL, rr(read_byte(cpu.r.HL))); break;
		case 0x1F: cpu.r.A = rr(cpu.r.A); break;
		// SLA
		case 0x20: cpu.r.B = sla(cpu.r.B); break;
		case 0x21: cpu.r.C = sla(cpu.r.C); break;
		case 0x22: cpu.r.D = sla(cpu.r.D); break;
		case 0x23: cpu.r.E = sla(cpu.r.E); break;
		case 0x24: cpu.r.H = sla(cpu.r.H); break;
		case 0x25: cpu.r.L = sla(cpu.r.L); break;
		case 0x26: write_byte(cpu.r.HL, sla(read_byte(cpu.r.HL))); break;
		case 0x27: cpu.r.A = sla(cpu.r.A); break;
		// SRA
		case 0x28: cpu.r.B = sra(cpu.r.B); break;
		case 0x29: cpu.r.C = sra(cpu.r.C); break;
		case 0x2A: cpu.r.D = sra(cpu.r.D); break;
		case 0x2B: cpu.r.E = sra(cpu.r.E); break;
		case 0x2C: cpu.r.H = sra(cpu.r.H); break;
		case 0x2D: cpu.r.L = sra(cpu.r.L); break;
		case 0x2E: write_byte(cpu.r.HL, sra(read_byte(cpu.r.HL))); break;
		case 0x2F: cpu.r.A = sra(cpu.r.A); break;
		// SWAP
		case 0x30: cpu.r.B = swap(cpu.r.B); break;
		case 0x31: cpu.r.C = swap(cpu.r.C); break;
		case 0x32: cpu.r.D = swap(cpu.r.D); break;
		case 0x33: cpu.r.E = swap(cpu.r.E); break;
		case 0x34: cpu.r.H = swap(cpu.r.H); break;
		case 0x35: cpu.r.L = swap(cpu.r.L); break;
		case 0x36: write_byte(cpu.r.HL, swap(read_byte(cpu.r.HL))); break;
		case 0x37: cpu.r.A = swap(cpu.r.A); break;
		// SRL
		case 0x38: cpu.r.B = srl(cpu.r.B); break;
		case 0x39: cpu.r.C = srl(cpu.r.C); break;
		case 0x3A: cpu.r.D = srl(cpu.r.D); break;
		case 0x3B: cpu.r.E = srl(cpu.r.E); break;
		case 0x3C: cpu.r.H = srl(cpu.r.H); break;
		case 0x3D: cpu.r.L = srl(cpu.r.L); break;
		case 0x3E: write_byte(cpu.r.HL, srl(read_byte(cpu.r.HL))); break;
		case 0x3F: cpu.r.A = srl(cpu.r.A); break;
		// BIT
		case 0x40: bit(0, cpu.r.B); break;
		case 0x41: bit(0, cpu.r.C); break;
		case 0x42: bit(0, cpu.r.D); break;
		case 0x43: bit(0, cpu.r.E); break;
		case 0x44: bit(0, cpu.r.H); break;
		case 0x45: bit(0, cpu.r.L); break;
		case 0x46: bit(0, read_byte(cpu.r.HL)); break;
		case 0x47: bit(0, cpu.r.A); break;

		case 0x48: bit(1, cpu.r.B); break;
		case 0x49: bit(1, cpu.r.C); break;
		case 0x4A: bit(1, cpu.r.D); break;
		case 0x4B: bit(1, cpu.r.E); break;
		case 0x4C: bit(1, cpu.r.H); break;
		case 0x4D: bit(1, cpu.r.L); break;
		case 0x4E: bit(1, read_byte(cpu.r.HL)); break;
		case 0x4F: bit(1, cpu.r.A); break;
			
		case 0x50: bit(2, cpu.r.B); break;
		case 0x51: bit(2, cpu.r.C); break;
		case 0x52: bit(2, cpu.r.D); break;
		case 0x53: bit(2, cpu.r.E); break;
		case 0x54: bit(2, cpu.r.H); break;
		case 0x55: bit(2, cpu.r.L); break;
		case 0x56: bit(2, read_byte(cpu.r.HL)); break;
		case 0x57: bit(2, cpu.r.A); break;

		case 0x58: bit(3, cpu.r.B); break;
		case 0x59: bit(3, cpu.r.C); break;
		case 0x5A: bit(3, cpu.r.D); break;
		case 0x5B: bit(3, cpu.r.E); break;
		case 0x5C: bit(3, cpu.r.H); break;
		case 0x5D: bit(3, cpu.r.L); break;
		case 0x5E: bit(3, read_byte(cpu.r.HL)); break;
		case 0x5F: bit(3, cpu.r.A); break;
			
		case 0x60: bit(4, cpu.r.B); break;
		case 0x61: bit(4, cpu.r.C); break;
		case 0x62: bit(4, cpu.r.D); break;
		case 0x63: bit(4, cpu.r.E); break;
		case 0x64: bit(4, cpu.r.H); break;
		case 0x65: bit(4, cpu.r.L); break;
		case 0x66: bit(4, read_byte(cpu.r.HL)); break;
		case 0x67: bit(4, cpu.r.A); break;

		case 0x68: bit(5, cpu.r.B); break;
		case 0x69: bit(5, cpu.r.C); break;
		case 0x6A: bit(5, cpu.r.D); break;
		case 0x6B: bit(5, cpu.r.E); break;
		case 0x6C: bit(5, cpu.r.H); break;
		case 0x6D: bit(5, cpu.r.L); break;
		case 0x6E: bit(5, read_byte(cpu.r.HL)); break;
		case 0x6F: bit(5, cpu.r.A); break;
			
		case 0x70: bit(6, cpu.r.B); break;
		case 0x71: bit(6, cpu.r.C); break;
		case 0x72: bit(6, cpu.r.D); break;
		case 0x73: bit(6, cpu.r.E); break;
		case 0x74: bit(6, cpu.r.H); break;
		case 0x75: bit(6, cpu.r.L); break;
		case 0x76: bit(6, read_byte(cpu.r.HL)); break;
		case 0x77: bit(6, cpu.r.A); break;

		case 0x78: bit(7, cpu.r.B); break;
		case 0x79: bit(7, cpu.r.C); break;
		case 0x7A: bit(7, cpu.r.D); break;
		case 0x7B: bit(7, cpu.r.E); break;
		case 0x7C: bit(7, cpu.r.H); break;
		case 0x7D: bit(7, cpu.r.L); break;
		case 0x7E: bit(7, read_byte(cpu.r.HL)); break;
		case 0x7F: bit(7, cpu.r.A); break;
		// RES
		case 0x80: cpu.r.B = res(0, cpu.r.B); break;
		case 0x81: cpu.r.C = res(0, cpu.r.C); break;
		case 0x82: cpu.r.D = res(0, cpu.r.D); break;
		case 0x83: cpu.r.E = res(0, cpu.r.E); break;
		case 0x84: cpu.r.H = res(0, cpu.r.H); break;
		case 0x85: cpu.r.L = res(0, cpu.r.L); break;
		case 0x86: write_byte(cpu.r.HL, res(0, read_byte(cpu.r.HL))); break;
		case 0x87: cpu.r.A = res(0, cpu.r.A); break;

		case 0x88: cpu.r.B = res(1, cpu.r.B); break;
		case 0x89: cpu.r.C = res(1, cpu.r.C); break;
		case 0x8A: cpu.r.D = res(1, cpu.r.D); break;
		case 0x8B: cpu.r.E = res(1, cpu.r.E); break;
		case 0x8C: cpu.r.H = res(1, cpu.r.H); break;
		case 0x8D: cpu.r.L = res(1, cpu.r.L); break;
		case 0x8E: write_byte(cpu.r.HL, res(1, read_byte(cpu.r.HL))); break;
		case 0x8F: cpu.r.A = res(1, cpu.r.A); break;

		case 0x90: cpu.r.B = res(2, cpu.r.B); break;
		case 0x91: cpu.r.C = res(2, cpu.r.C); break;
		case 0x92: cpu.r.D = res(2, cpu.r.D); break;
		case 0x93: cpu.r.E = res(2, cpu.r.E); break;
		case 0x94: cpu.r.H = res(2, cpu.r.H); break;
		case 0x95: cpu.r.L = res(2, cpu.r.L); break;
		case 0x96: write_byte(cpu.r.HL, res(2, read_byte(cpu.r.HL))); break;
		case 0x97: cpu.r.A = res(2, cpu.r.A); break;

		case 0x98: cpu.r.B = res(3, cpu.r.B); break;
		case 0x99: cpu.r.C = res(3, cpu.r.C); break;
		case 0x9A: cpu.r.D = res(3, cpu.r.D); break;
		case 0x9B: cpu.r.E = res(3, cpu.r.E); break;
		case 0x9C: cpu.r.H = res(3, cpu.r.H); break;
		case 0x9D: cpu.r.L = res(3, cpu.r.L); break;
		case 0x9E: write_byte(cpu.r.HL, res(3, read_byte(cpu.r.HL))); break;
		case 0x9F: cpu.r.A = res(3, cpu.r.A); break;

		case 0xA0: cpu.r.B = res(4, cpu.r.B); break;
		case 0xA1: cpu.r.C = res(4, cpu.r.C); break;
		case 0xA2: cpu.r.D = res(4, cpu.r.D); break;
		case 0xA3: cpu.r.E = res(4, cpu.r.E); break;
		case 0xA4: cpu.r.H = res(4, cpu.r.H); break;
		case 0xA5: cpu.r.L = res(4, cpu.r.L); break;
		case 0xA6: write_byte(cpu.r.HL, res(4, read_byte(cpu.r.HL))); break;
		case 0xA7: cpu.r.A = res(4, cpu.r.A); break;

		case 0xA8: cpu.r.B = res(5, cpu.r.B); break;
		case 0xA9: cpu.r.C = res(5, cpu.r.C); break;
		case 0xAA: cpu.r.D = res(5, cpu.r.D); break;
		case 0xAB: cpu.r.E = res(5, cpu.r.E); break;
		case 0xAC: cpu.r.H = res(5, cpu.r.H); break;
		case 0xAD: cpu.r.L = res(5, cpu.r.L); break;
		case 0xAE: write_byte(cpu.r.HL, res(5, read_byte(cpu.r.HL))); break;
		case 0xAF: cpu.r.A = res(5, cpu.r.A); break;

		case 0xB0: cpu.r.B = res(6, cpu.r.B); break;
		case 0xB1: cpu.r.C = res(6, cpu.r.C); break;
		case 0xB2: cpu.r.D = res(6, cpu.r.D); break;
		case 0xB3: cpu.r.E = res(6, cpu.r.E); break;
		case 0xB4: cpu.r.H = res(6, cpu.r.H); break;
		case 0xB5: cpu.r.L = res(6, cpu.r.L); break;
		case 0xB6: write_byte(cpu.r.HL, res(6, read_byte(cpu.r.HL))); break;
		case 0xB7: cpu.r.A = res(6, cpu.r.A); break;

		case 0xB8: cpu.r.B = res(7, cpu.r.B); break;
		case 0xB9: cpu.r.C = res(7, cpu.r.C); break;
		case 0xBA: cpu.r.D = res(7, cpu.r.D); break;
		case 0xBB: cpu.r.E = res(7, cpu.r.E); break;
		case 0xBC: cpu.r.H = res(7, cpu.r.H); break;
		case 0xBD: cpu.r.L = res(7, cpu.r.L); break;
		case 0xBE: write_byte(cpu.r.HL, res(7, read_byte(cpu.r.HL))); break;
		case 0xBF: cpu.r.A = res(7, cpu.r.A); break;
		// SET
		case 0xC0: cpu.r.B = set(0, cpu.r.B); break;
		case 0xC1: cpu.r.C = set(0, cpu.r.C); break;
		case 0xC2: cpu.r.D = set(0, cpu.r.D); break;
		case 0xC3: cpu.r.E = set(0, cpu.r.E); break;
		case 0xC4: cpu.r.H = set(0, cpu.r.H); break;
		case 0xC5: cpu.r.L = set(0, cpu.r.L); break;
		case 0xC6: write_byte(cpu.r.HL, set(0, read_byte(cpu.r.HL))); break;
		case 0xC7: cpu.r.A = set(0, cpu.r.A); break;

		case 0xC8: cpu.r.B = set(1, cpu.r.B); break;
		case 0xC9: cpu.r.C = set(1, cpu.r.C); break;
		case 0xCA: cpu.r.D = set(1, cpu.r.D); break;
		case 0xCB: cpu.r.E = set(1, cpu.r.E); break;
		case 0xCC: cpu.r.H = set(1, cpu.r.H); break;
		case 0xCD: cpu.r.L = set(1, cpu.r.L); break;
		case 0xCE: write_byte(cpu.r.HL, set(1, read_byte(cpu.r.HL))); break;
		case 0xCF: cpu.r.A = set(1, cpu.r.A); break;

		case 0xD0: cpu.r.B = set(2, cpu.r.B); break;
		case 0xD1: cpu.r.C = set(2, cpu.r.C); break;
		case 0xD2: cpu.r.D = set(2, cpu.r.D); break;
		case 0xD3: cpu.r.E = set(2, cpu.r.E); break;
		case 0xD4: cpu.r.H = set(2, cpu.r.H); break;
		case 0xD5: cpu.r.L = set(2, cpu.r.L); break;
		case 0xD6: write_byte(cpu.r.HL, set(2, read_byte(cpu.r.HL))); break;
		case 0xD7: cpu.r.A = set(2, cpu.r.A); break;

		case 0xD8: cpu.r.B = set(3, cpu.r.B); break;
		case 0xD9: cpu.r.C = set(3, cpu.r.C); break;
		case 0xDA: cpu.r.D = set(3, cpu.r.D); break;
		case 0xDB: cpu.r.E = set(3, cpu.r.E); break;
		case 0xDC: cpu.r.H = set(3, cpu.r.H); break;
		case 0xDD: cpu.r.L = set(3, cpu.r.L); break;
		case 0xDE: write_byte(cpu.r.HL, set(3, read_byte(cpu.r.HL))); break;
		case 0xDF: cpu.r.A = set(3, cpu.r.A); break;

		case 0xE0: cpu.r.B = set(4, cpu.r.B); break;
		case 0xE1: cpu.r.C = set(4, cpu.r.C); break;
		case 0xE2: cpu.r.D = set(4, cpu.r.D); break;
		case 0xE3: cpu.r.E = set(4, cpu.r.E); break;
		case 0xE4: cpu.r.H = set(4, cpu.r.H); break;
		case 0xE5: cpu.r.L = set(4, cpu.r.L); break;
		case 0xE6: write_byte(cpu.r.HL, set(4, read_byte(cpu.r.HL))); break;
		case 0xE7: cpu.r.A = set(4, cpu.r.A); break;

		case 0xE8: cpu.r.B = set(5, cpu.r.B); break;
		case 0xE9: cpu.r.C = set(5, cpu.r.C); break;
		case 0xEA: cpu.r.D = set(5, cpu.r.D); break;
		case 0xEB: cpu.r.E = set(5, cpu.r.E); break;
		case 0xEC: cpu.r.H = set(5, cpu.r.H); break;
		case 0xED: cpu.r.L = set(5, cpu.r.L); break;
		case 0xEE: write_byte(cpu.r.HL, set(5, read_byte(cpu.r.HL))); break;
		case 0xEF: cpu.r.A = set(5, cpu.r.A); break;

		case 0xF0: cpu.r.B = set(6, cpu.r.B); break;
		case 0xF1: cpu.r.C = set(6, cpu.r.C); break;
		case 0xF2: cpu.r.D = set(6, cpu.r.D); break;
		case 0xF3: cpu.r.E = set(6, cpu.r.E); break;
		case 0xF4: cpu.r.H = set(6, cpu.r.H); break;
		case 0xF5: cpu.r.L = set(6, cpu.r.L); break;
		case 0xF6: write_byte(cpu.r.HL, set(6, read_byte(cpu.r.HL))); break;
		case 0xF7: cpu.r.A = set(6, cpu.r.A); break;

		case 0xF8: cpu.r.B = set(7, cpu.r.B); break;
		case 0xF9: cpu.r.C = set(7, cpu.r.C); break;
		case 0xFA: cpu.r.D = set(7, cpu.r.D); break;
		case 0xFB: cpu.r.E = set(7, cpu.r.E); break;
		case 0xFC: cpu.r.H = set(7, cpu.r.H); break;
		case 0xFD: cpu.r.L = set(7, cpu.r.L); break;
		case 0xFE: write_byte(cpu.r.HL, set(7, read_byte(cpu.r.HL))); break;
		case 0xFF: cpu.r.A = set(7, cpu.r.A); break;

		default:
			printf("CB opcode %.2X not implemented\n", opcode);
			exit(0);
	}
}

u8 cpu_read_wram_byte(u16 address)
{
	if (address >= 0xC000 && address <= 0xCFFF)
	{
		return cpu.wram[address - 0xC000];
	}
	else if (gb.cbg_mode == 0 && address >= 0xD000 && address <= 0xDFFF)
	{
		return cpu.wram[address - 0xC000];
	}
	else if (gb.cbg_mode == 1 && address >= 0xD000 && address <= 0xDFFF)
	{
		return cpu.wram[address - 0xB000 + (cpu.wram_bank * 0x1000)];
	}
	else if (address >= 0xE000 && address <= 0xFDFF)
	{
		return cpu.wram[address - 0xE000];
	}
}

void cpu_write_wram_byte(u16 address, u8 value)
{
	if (address >= 0xC000 && address <= 0xCFFF)
	{
		cpu.wram[address - 0xC000] = value;
	}
	else if (gb.cbg_mode == 0 && address >= 0xD000 && address <= 0xDFFF)
	{
		cpu.wram[address - 0xC000] = value;
	}
	else if (gb.cbg_mode == 1 && address >= 0xD000 && address <= 0xDFFF)
	{	// maybe combine with previous
		cpu.wram[address - 0xB000 + (cpu.wram_bank * 0x1000)] = value;
	}
	else if (address >= 0xE000 && address <= 0xFDFF)
	{
		cpu.wram[address - 0xE000] = value;
	}
}