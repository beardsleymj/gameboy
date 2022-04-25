#include "gb.h"
#include "cpu.h"
#include "cpu_instr.h"
#include "bus.h"
#include <string.h>

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
		if (gb.interrupt_enable.raw && gb.interrupt_flag.raw)
			gb.halt = 0;
		else 
		{
			gb.cycles += 4;
			return;
		}
	}

	// Handle Interrupts
	if (gb.IME && (gb.interrupt_enable.raw & gb.interrupt_flag.raw))
	{
		write_byte(--gb.SP, (gb.PC >> 8));
		write_byte(--gb.SP, gb.PC & 0xFF);
		gb.cycles += 12;
		gb.IME = false;

		if (gb.interrupt_enable.vblank && gb.interrupt_flag.vblank)
		{
			gb.PC = 0x40;
			gb.interrupt_flag.vblank = 0;
		}
		else if (gb.interrupt_enable.stat && gb.interrupt_flag.stat)
		{
			gb.PC = 0x48;
			gb.interrupt_flag.stat = 0;
		}
		else if (gb.interrupt_enable.timer && gb.interrupt_flag.timer)
		{
			gb.PC = 0x50;
			gb.interrupt_flag.timer = 0;
		}
		else if (gb.interrupt_enable.serial && gb.interrupt_flag.serial)
		{
			gb.PC = 0x58;
			gb.interrupt_flag.serial = 0;
		}
		else if (gb.interrupt_enable.joypad && gb.interrupt_flag.joypad)
		{
			gb.PC = 0x60;
			gb.interrupt_flag.joypad = 0;
		}
	}

	u8 opcode = read_byte(gb.PC++);
		
	switch (opcode) 
	{
		// 8-bit Load Instructions
		case 0x40: gb.r.B = gb.r.B; break;
		case 0x41: gb.r.B = gb.r.C; break;
		case 0x42: gb.r.B = gb.r.D; break;
		case 0x43: gb.r.B = gb.r.E; break;
		case 0x44: gb.r.B = gb.r.H; break;
		case 0x45: gb.r.B = gb.r.L; break;
		case 0x46: gb.r.B = read_byte(gb.r.HL); break;
		case 0x47: gb.r.B = gb.r.A; break;

		case 0x48: gb.r.C = gb.r.B; break;
		case 0x49: gb.r.C = gb.r.C; break;
		case 0x4A: gb.r.C = gb.r.D; break;
		case 0x4B: gb.r.C = gb.r.E; break;
		case 0x4C: gb.r.C = gb.r.H; break;
		case 0x4D: gb.r.C = gb.r.L; break;
		case 0x4E: gb.r.C = read_byte(gb.r.HL); break;
		case 0x4F: gb.r.C = gb.r.A; break;

		case 0x50: gb.r.D = gb.r.B; break;
		case 0x51: gb.r.D = gb.r.C; break;
		case 0x52: gb.r.D = gb.r.D; break;
		case 0x53: gb.r.D = gb.r.E; break;
		case 0x54: gb.r.D = gb.r.H; break;
		case 0x55: gb.r.D = gb.r.L; break;
		case 0x56: gb.r.D = read_byte(gb.r.HL); break;
		case 0x57: gb.r.D = gb.r.A; break;

		case 0x58: gb.r.E = gb.r.B; break;
		case 0x59: gb.r.E = gb.r.C; break;
		case 0x5A: gb.r.E = gb.r.D; break;
		case 0x5B: gb.r.E = gb.r.E; break;
		case 0x5C: gb.r.E = gb.r.H; break;
		case 0x5D: gb.r.E = gb.r.L; break;
		case 0x5E: gb.r.E = read_byte(gb.r.HL); break;
		case 0x5F: gb.r.E = gb.r.A; break;

		case 0x60: gb.r.H = gb.r.B; break;
		case 0x61: gb.r.H = gb.r.C; break;
		case 0x62: gb.r.H = gb.r.D; break;
		case 0x63: gb.r.H = gb.r.E; break;
		case 0x64: gb.r.H = gb.r.H; break;
		case 0x65: gb.r.H = gb.r.L; break;
		case 0x66: gb.r.H = read_byte(gb.r.HL); break;
		case 0x67: gb.r.H = gb.r.A; break;

		case 0x68: gb.r.L = gb.r.B; break;
		case 0x69: gb.r.L = gb.r.C; break;
		case 0x6A: gb.r.L = gb.r.D; break;
		case 0x6B: gb.r.L = gb.r.E; break;
		case 0x6C: gb.r.L = gb.r.H; break;
		case 0x6D: gb.r.L = gb.r.L; break;
		case 0x6E: gb.r.L = read_byte(gb.r.HL); break;
		case 0x6F: gb.r.L = gb.r.A; break;

		case 0x70: write_byte(gb.r.HL, gb.r.B); break;
		case 0x71: write_byte(gb.r.HL, gb.r.C); break;
		case 0x72: write_byte(gb.r.HL, gb.r.D); break;
		case 0x73: write_byte(gb.r.HL, gb.r.E); break;
		case 0x74: write_byte(gb.r.HL, gb.r.H); break;
		case 0x75: write_byte(gb.r.HL, gb.r.L); break;
		case 0x77: write_byte(gb.r.HL, gb.r.A); break;

		case 0x78: gb.r.A = gb.r.B; break;
		case 0x79: gb.r.A = gb.r.C; break;
		case 0x7A: gb.r.A = gb.r.D; break;
		case 0x7B: gb.r.A = gb.r.E; break;
		case 0x7C: gb.r.A = gb.r.H; break;
		case 0x7D: gb.r.A = gb.r.L; break;
		case 0x7E: gb.r.A = read_byte(gb.r.HL); break;
		case 0x7F: gb.r.A = gb.r.A; break;

		case 0x02: write_byte(gb.r.BC, gb.r.A); break;
		case 0x12: write_byte(gb.r.DE, gb.r.A); break;
		case 0x22: write_byte(gb.r.HL++, gb.r.A); break;
		case 0x32: write_byte(gb.r.HL--, gb.r.A); break;

		case 0x06: gb.r.B = read_byte(gb.PC++); break;
		case 0x0E: gb.r.C = read_byte(gb.PC++); break;
		case 0x16: gb.r.D = read_byte(gb.PC++); break;
		case 0x1E: gb.r.E = read_byte(gb.PC++); break;
		case 0x26: gb.r.H = read_byte(gb.PC++); break;
		case 0x2E: gb.r.L = read_byte(gb.PC++); break;
		case 0x36: write_byte(gb.r.HL, read_byte(gb.PC++)); break;
		case 0x3E: gb.r.A = read_byte(gb.PC++); break;

		case 0x0A: gb.r.A = read_byte(gb.r.BC); break;
		case 0x1A: gb.r.A = read_byte(gb.r.DE); break;
		case 0x2A: gb.r.A = read_byte(gb.r.HL++); break;
		case 0x3A: gb.r.A = read_byte(gb.r.HL--); break;

		case 0xE0: write_byte(0xFF00 + read_byte(gb.PC++), gb.r.A); break;
		case 0xF0: gb.r.A = read_byte(0xFF00 + read_byte(gb.PC++)); break;
		case 0xE2: write_byte(0xFF00 + gb.r.C, gb.r.A); break;
		case 0xF2: gb.r.A = read_byte(0xFF00 + gb.r.C); break;
		case 0xEA: write_byte(read_word(gb.PC), gb.r.A); gb.PC += 2; break;
		case 0xFA: gb.r.A = read_byte(read_word(gb.PC)); gb.PC+= 2; break;

			// 16-bit Load Instructions
		case 0x01: gb.r.BC = read_byte(gb.PC++) | (read_byte(gb.PC++) << 8); break;
		case 0x11: gb.r.DE = read_byte(gb.PC++) | (read_byte(gb.PC++) << 8); break;
		case 0x21: gb.r.HL = read_byte(gb.PC++) | (read_byte(gb.PC++) << 8); break;
		case 0xF8: ld_hl_sp_i8(); break;
		case 0x31: gb.SP = read_byte(gb.PC++) | (read_byte(gb.PC++) << 8); break;
		case 0xF9: gb.SP = gb.r.HL; gb.cycles += 4; break;
		case 0x08: {
			u16 address = read_byte(gb.PC++) | (read_byte(gb.PC++) << 8);
			write_byte(address, gb.SP & 0xFF);
			write_byte(address + 1, gb.SP >> 8);
		}
		break;
		case 0xC5: case 0xD5: case 0xE5: case 0xF5:
			push(opcode);
			break;
		case 0xC1: case 0xD1: case 0xE1: case 0xF1:
			pop(opcode);
			break;

		case 0x80: add(gb.r.B); break;
		case 0x81: add(gb.r.C); break;
		case 0x82: add(gb.r.D); break;
		case 0x83: add(gb.r.E); break;
		case 0x84: add(gb.r.H); break;
		case 0x85: add(gb.r.L); break;
		case 0x86: add(read_byte(gb.r.HL)); break;
		case 0x87: add(gb.r.A); break;
		case 0xC6: add(read_byte(gb.PC++)); break;
		case 0x09: add16(gb.r.BC); break;
		case 0x19: add16(gb.r.DE); break;
		case 0x29: add16(gb.r.HL); break;
		case 0x39: add16(gb.SP); break;
		case 0xE8: add_sp_i8();	break;

		case 0x88: adc(gb.r.B); break;
		case 0x89: adc(gb.r.C); break;
		case 0x8A: adc(gb.r.D); break;
		case 0x8B: adc(gb.r.E); break;
		case 0x8C: adc(gb.r.H); break;
		case 0x8D: adc(gb.r.L); break;
		case 0x8E: adc(read_byte(gb.r.HL)); break;
		case 0x8F: adc(gb.r.A); break;
		case 0xCE: adc(read_byte(gb.PC++)); break;

		case 0x90: sub(gb.r.B); break;
		case 0x91: sub(gb.r.C); break;
		case 0x92: sub(gb.r.D); break;
		case 0x93: sub(gb.r.E); break;
		case 0x94: sub(gb.r.H); break;
		case 0x95: sub(gb.r.L); break;
		case 0x96: sub(read_byte(gb.r.HL)); break;
		case 0x97: sub(gb.r.A); break;
		case 0xD6: sub(read_byte(gb.PC++)); break;

		case 0x98: sbc(gb.r.B); break;
		case 0x99: sbc(gb.r.C); break;
		case 0x9A: sbc(gb.r.D); break;
		case 0x9B: sbc(gb.r.E); break;
		case 0x9C: sbc(gb.r.H); break;
		case 0x9D: sbc(gb.r.L); break;
		case 0x9E: sbc(read_byte(gb.r.HL)); break;
		case 0x9F: sbc(gb.r.A); break;
		case 0xDE: sbc(read_byte(gb.PC++)); break;

		case 0x07: rlca(); break;
		case 0x0F: rrca(); break;
		case 0x17: rla(); break;
		case 0x1F: rra(); break;

		case 0xA0: and (gb.r.B); break;
		case 0xA1: and (gb.r.C); break;
		case 0xA2: and (gb.r.D); break;
		case 0xA3: and (gb.r.E); break;
		case 0xA4: and (gb.r.H); break;
		case 0xA5: and (gb.r.L); break;
		case 0xA6: and (read_byte(gb.r.HL)); break;
		case 0xA7: and (gb.r.A); break;
		case 0xE6: and (read_byte(gb.PC++)); break;

		case 0xA8: xor (gb.r.B); break;
		case 0xA9: xor (gb.r.C); break;
		case 0xAA: xor (gb.r.D); break;
		case 0xAB: xor (gb.r.E); break;
		case 0xAC: xor (gb.r.H); break;
		case 0xAD: xor (gb.r.L); break;
		case 0xAE: xor (read_byte(gb.r.HL)); break;
		case 0xAF: xor (gb.r.A); break;
		case 0xEE: xor (read_byte(gb.PC++)); break;
				
		case 0xB0: or (gb.r.B); break;
		case 0xB1: or (gb.r.C); break;
		case 0xB2: or (gb.r.D); break;
		case 0xB3: or (gb.r.E); break;
		case 0xB4: or (gb.r.H); break;
		case 0xB5: or (gb.r.L); break;
		case 0xB6: or (read_byte(gb.r.HL)); break;
		case 0xB7: or (gb.r.A); break;
		case 0xF6: or (read_byte(gb.PC++)); break;
				
		case 0xB8: cp(gb.r.B); break;
		case 0xB9: cp(gb.r.C); break;
		case 0xBA: cp(gb.r.D); break;
		case 0xBB: cp(gb.r.E); break;
		case 0xBC: cp(gb.r.H); break;
		case 0xBD: cp(gb.r.L); break;
		case 0xBE: cp(read_byte(gb.r.HL)); break;
		case 0xBF: cp(gb.r.A); break;
		case 0xFE: cp(read_byte(gb.PC++)); break;
				
		case 0x04: case 0x14: case 0x24: case 0x34: case 0x0C: case 0x1C: case 0x2C: case 0x3C:
			inc_8(opcode);
			break;
		case 0x03: case 0x13: case 0x23: case 0x33:
			inc_16(opcode);
			break;
		case 0x05: dec(&gb.r.B); break;
		case 0x0D: dec(&gb.r.C); break;
		case 0x15: dec(&gb.r.D); break;
		case 0x1D: dec(&gb.r.E); break;
		case 0x25: dec(&gb.r.H); break;
		case 0x2D: dec(&gb.r.L); break;
		case 0x35: dec_hl(); break;
		case 0x3D: dec(&gb.r.A); break;
		case 0x0B: dec16(&gb.r.BC); break;
		case 0x1B: dec16(&gb.r.DE); break;
		case 0x2B: dec16(&gb.r.HL); break;
		case 0x3B: dec16(&gb.SP); break;
		case 0x27: daa(); break;
		case 0x2F: cpl(); break;

		case 0x20: case 0x30: case 0x18: case 0x28: case 0x38:
			jr(opcode);
			break;
				
		case 0xCB: prefix_cb(); break;
				
		case 0xC2: jp(read_byte(gb.PC++) + (read_byte(gb.PC++) << 8), opcode); break;
		case 0xD2: jp(read_byte(gb.PC++) + (read_byte(gb.PC++) << 8), opcode); break;
		case 0xCA: jp(read_byte(gb.PC++) + (read_byte(gb.PC++) << 8), opcode); break;
		case 0xDA: jp(read_byte(gb.PC++) + (read_byte(gb.PC++) << 8), opcode); break;
		case 0xE9: gb.PC = gb.r.HL; break;
		case 0xC3: gb.PC = (read_byte(gb.PC++) + (read_byte(gb.PC++) << 8)); break;
				
		case 0xC4: case 0xD4: case 0xCC: case 0xDC: case 0xCD:
			call(opcode);
			break;
		case 0xC0: case 0xD0: case 0xC8: case 0xD8: case 0xC9: case 0xD9:
			ret(opcode);
			break;
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
		case 0x10: break; // STOP: TODO
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
	u8 opcode = read_byte(gb.PC++);

	switch (opcode) 
	{
		// RLC
		case 0x00: gb.r.B = rlc(gb.r.B); break;
		case 0x01: gb.r.C = rlc(gb.r.C); break;
		case 0x02: gb.r.D = rlc(gb.r.D); break;
		case 0x03: gb.r.E = rlc(gb.r.E); break;
		case 0x04: gb.r.H = rlc(gb.r.H); break;
		case 0x05: gb.r.L = rlc(gb.r.L); break;
		case 0x06: write_byte(gb.r.HL, rlc(read_byte(gb.r.HL))); break;
		case 0x07: gb.r.A = rlc(gb.r.A); break;
		// RRC
		case 0x08: gb.r.B = rrc(gb.r.B); break;
		case 0x09: gb.r.C = rrc(gb.r.C); break;
		case 0x0A: gb.r.D = rrc(gb.r.D); break;
		case 0x0B: gb.r.E = rrc(gb.r.E); break;
		case 0x0C: gb.r.H = rrc(gb.r.H); break;
		case 0x0D: gb.r.L = rrc(gb.r.L); break;
		case 0x0E: write_byte(gb.r.HL, rrc(read_byte(gb.r.HL))); break;
		case 0x0F: gb.r.A = rrc(gb.r.A); break;
		// RL
		case 0x10: gb.r.B = rl(gb.r.B); break;
		case 0x11: gb.r.C = rl(gb.r.C); break;
		case 0x12: gb.r.D = rl(gb.r.D); break;
		case 0x13: gb.r.E = rl(gb.r.E); break;
		case 0x14: gb.r.H = rl(gb.r.H); break;
		case 0x15: gb.r.L = rl(gb.r.L); break;
		case 0x16: write_byte(gb.r.HL, rl(read_byte(gb.r.HL))); break;
		case 0x17: gb.r.A = rl(gb.r.A); break;
		// RR
		case 0x18: gb.r.B = rr(gb.r.B); break;
		case 0x19: gb.r.C = rr(gb.r.C); break;
		case 0x1A: gb.r.D = rr(gb.r.D); break;
		case 0x1B: gb.r.E = rr(gb.r.E); break;
		case 0x1C: gb.r.H = rr(gb.r.H); break;
		case 0x1D: gb.r.L = rr(gb.r.L); break;
		case 0x1E: write_byte(gb.r.HL, rr(read_byte(gb.r.HL))); break;
		case 0x1F: gb.r.A = rr(gb.r.A); break;
		// SLA
		case 0x20: gb.r.B = sla(gb.r.B); break;
		case 0x21: gb.r.C = sla(gb.r.C); break;
		case 0x22: gb.r.D = sla(gb.r.D); break;
		case 0x23: gb.r.E = sla(gb.r.E); break;
		case 0x24: gb.r.H = sla(gb.r.H); break;
		case 0x25: gb.r.L = sla(gb.r.L); break;
		case 0x26: write_byte(gb.r.HL, sla(read_byte(gb.r.HL))); break;
		case 0x27: gb.r.A = sla(gb.r.A); break;
		// SRA
		case 0x28: gb.r.B = sra(gb.r.B); break;
		case 0x29: gb.r.C = sra(gb.r.C); break;
		case 0x2A: gb.r.D = sra(gb.r.D); break;
		case 0x2B: gb.r.E = sra(gb.r.E); break;
		case 0x2C: gb.r.H = sra(gb.r.H); break;
		case 0x2D: gb.r.L = sra(gb.r.L); break;
		case 0x2E: write_byte(gb.r.HL, sra(read_byte(gb.r.HL))); break;
		case 0x2F: gb.r.A = sra(gb.r.A); break;
		// SWAP
		case 0x30: gb.r.B = swap(gb.r.B); break;
		case 0x31: gb.r.C = swap(gb.r.C); break;
		case 0x32: gb.r.D = swap(gb.r.D); break;
		case 0x33: gb.r.E = swap(gb.r.E); break;
		case 0x34: gb.r.H = swap(gb.r.H); break;
		case 0x35: gb.r.L = swap(gb.r.L); break;
		case 0x36: write_byte(gb.r.HL, swap(read_byte(gb.r.HL))); break;
		case 0x37: gb.r.A = swap(gb.r.A); break;
		// SRL
		case 0x38: gb.r.B = srl(gb.r.B); break;
		case 0x39: gb.r.C = srl(gb.r.C); break;
		case 0x3A: gb.r.D = srl(gb.r.D); break;
		case 0x3B: gb.r.E = srl(gb.r.E); break;
		case 0x3C: gb.r.H = srl(gb.r.H); break;
		case 0x3D: gb.r.L = srl(gb.r.L); break;
		case 0x3E: write_byte(gb.r.HL, srl(read_byte(gb.r.HL))); break;
		case 0x3F: gb.r.A = srl(gb.r.A); break;
		// BIT
		case 0x40: bit(0, gb.r.B); break;
		case 0x41: bit(0, gb.r.C); break;
		case 0x42: bit(0, gb.r.D); break;
		case 0x43: bit(0, gb.r.E); break;
		case 0x44: bit(0, gb.r.H); break;
		case 0x45: bit(0, gb.r.L); break;
		case 0x46: bit(0, read_byte(gb.r.HL)); break;
		case 0x47: bit(0, gb.r.A); break;

		case 0x48: bit(1, gb.r.B); break;
		case 0x49: bit(1, gb.r.C); break;
		case 0x4A: bit(1, gb.r.D); break;
		case 0x4B: bit(1, gb.r.E); break;
		case 0x4C: bit(1, gb.r.H); break;
		case 0x4D: bit(1, gb.r.L); break;
		case 0x4E: bit(1, read_byte(gb.r.HL)); break;
		case 0x4F: bit(1, gb.r.A); break;
			
		case 0x50: bit(2, gb.r.B); break;
		case 0x51: bit(2, gb.r.C); break;
		case 0x52: bit(2, gb.r.D); break;
		case 0x53: bit(2, gb.r.E); break;
		case 0x54: bit(2, gb.r.H); break;
		case 0x55: bit(2, gb.r.L); break;
		case 0x56: bit(2, read_byte(gb.r.HL)); break;
		case 0x57: bit(2, gb.r.A); break;

		case 0x58: bit(3, gb.r.B); break;
		case 0x59: bit(3, gb.r.C); break;
		case 0x5A: bit(3, gb.r.D); break;
		case 0x5B: bit(3, gb.r.E); break;
		case 0x5C: bit(3, gb.r.H); break;
		case 0x5D: bit(3, gb.r.L); break;
		case 0x5E: bit(3, read_byte(gb.r.HL)); break;
		case 0x5F: bit(3, gb.r.A); break;
			
		case 0x60: bit(4, gb.r.B); break;
		case 0x61: bit(4, gb.r.C); break;
		case 0x62: bit(4, gb.r.D); break;
		case 0x63: bit(4, gb.r.E); break;
		case 0x64: bit(4, gb.r.H); break;
		case 0x65: bit(4, gb.r.L); break;
		case 0x66: bit(4, read_byte(gb.r.HL)); break;
		case 0x67: bit(4, gb.r.A); break;

		case 0x68: bit(5, gb.r.B); break;
		case 0x69: bit(5, gb.r.C); break;
		case 0x6A: bit(5, gb.r.D); break;
		case 0x6B: bit(5, gb.r.E); break;
		case 0x6C: bit(5, gb.r.H); break;
		case 0x6D: bit(5, gb.r.L); break;
		case 0x6E: bit(5, read_byte(gb.r.HL)); break;
		case 0x6F: bit(5, gb.r.A); break;
			
		case 0x70: bit(6, gb.r.B); break;
		case 0x71: bit(6, gb.r.C); break;
		case 0x72: bit(6, gb.r.D); break;
		case 0x73: bit(6, gb.r.E); break;
		case 0x74: bit(6, gb.r.H); break;
		case 0x75: bit(6, gb.r.L); break;
		case 0x76: bit(6, read_byte(gb.r.HL)); break;
		case 0x77: bit(6, gb.r.A); break;

		case 0x78: bit(7, gb.r.B); break;
		case 0x79: bit(7, gb.r.C); break;
		case 0x7A: bit(7, gb.r.D); break;
		case 0x7B: bit(7, gb.r.E); break;
		case 0x7C: bit(7, gb.r.H); break;
		case 0x7D: bit(7, gb.r.L); break;
		case 0x7E: bit(7, read_byte(gb.r.HL)); break;
		case 0x7F: bit(7, gb.r.A); break;
		// RES
		case 0x80: gb.r.B = res(0, gb.r.B); break;
		case 0x81: gb.r.C = res(0, gb.r.C); break;
		case 0x82: gb.r.D = res(0, gb.r.D); break;
		case 0x83: gb.r.E = res(0, gb.r.E); break;
		case 0x84: gb.r.H = res(0, gb.r.H); break;
		case 0x85: gb.r.L = res(0, gb.r.L); break;
		case 0x86: write_byte(gb.r.HL, res(0, read_byte(gb.r.HL))); break;
		case 0x87: gb.r.A = res(0, gb.r.A); break;

		case 0x88: gb.r.B = res(1, gb.r.B); break;
		case 0x89: gb.r.C = res(1, gb.r.C); break;
		case 0x8A: gb.r.D = res(1, gb.r.D); break;
		case 0x8B: gb.r.E = res(1, gb.r.E); break;
		case 0x8C: gb.r.H = res(1, gb.r.H); break;
		case 0x8D: gb.r.L = res(1, gb.r.L); break;
		case 0x8E: write_byte(gb.r.HL, res(1, read_byte(gb.r.HL))); break;
		case 0x8F: gb.r.A = res(1, gb.r.A); break;

		case 0x90: gb.r.B = res(2, gb.r.B); break;
		case 0x91: gb.r.C = res(2, gb.r.C); break;
		case 0x92: gb.r.D = res(2, gb.r.D); break;
		case 0x93: gb.r.E = res(2, gb.r.E); break;
		case 0x94: gb.r.H = res(2, gb.r.H); break;
		case 0x95: gb.r.L = res(2, gb.r.L); break;
		case 0x96: write_byte(gb.r.HL, res(2, read_byte(gb.r.HL))); break;
		case 0x97: gb.r.A = res(2, gb.r.A); break;

		case 0x98: gb.r.B = res(3, gb.r.B); break;
		case 0x99: gb.r.C = res(3, gb.r.C); break;
		case 0x9A: gb.r.D = res(3, gb.r.D); break;
		case 0x9B: gb.r.E = res(3, gb.r.E); break;
		case 0x9C: gb.r.H = res(3, gb.r.H); break;
		case 0x9D: gb.r.L = res(3, gb.r.L); break;
		case 0x9E: write_byte(gb.r.HL, res(3, read_byte(gb.r.HL))); break;
		case 0x9F: gb.r.A = res(3, gb.r.A); break;

		case 0xA0: gb.r.B = res(4, gb.r.B); break;
		case 0xA1: gb.r.C = res(4, gb.r.C); break;
		case 0xA2: gb.r.D = res(4, gb.r.D); break;
		case 0xA3: gb.r.E = res(4, gb.r.E); break;
		case 0xA4: gb.r.H = res(4, gb.r.H); break;
		case 0xA5: gb.r.L = res(4, gb.r.L); break;
		case 0xA6: write_byte(gb.r.HL, res(4, read_byte(gb.r.HL))); break;
		case 0xA7: gb.r.A = res(4, gb.r.A); break;

		case 0xA8: gb.r.B = res(5, gb.r.B); break;
		case 0xA9: gb.r.C = res(5, gb.r.C); break;
		case 0xAA: gb.r.D = res(5, gb.r.D); break;
		case 0xAB: gb.r.E = res(5, gb.r.E); break;
		case 0xAC: gb.r.H = res(5, gb.r.H); break;
		case 0xAD: gb.r.L = res(5, gb.r.L); break;
		case 0xAE: write_byte(gb.r.HL, res(5, read_byte(gb.r.HL))); break;
		case 0xAF: gb.r.A = res(5, gb.r.A); break;

		case 0xB0: gb.r.B = res(6, gb.r.B); break;
		case 0xB1: gb.r.C = res(6, gb.r.C); break;
		case 0xB2: gb.r.D = res(6, gb.r.D); break;
		case 0xB3: gb.r.E = res(6, gb.r.E); break;
		case 0xB4: gb.r.H = res(6, gb.r.H); break;
		case 0xB5: gb.r.L = res(6, gb.r.L); break;
		case 0xB6: write_byte(gb.r.HL, res(6, read_byte(gb.r.HL))); break;
		case 0xB7: gb.r.A = res(6, gb.r.A); break;

		case 0xB8: gb.r.B = res(7, gb.r.B); break;
		case 0xB9: gb.r.C = res(7, gb.r.C); break;
		case 0xBA: gb.r.D = res(7, gb.r.D); break;
		case 0xBB: gb.r.E = res(7, gb.r.E); break;
		case 0xBC: gb.r.H = res(7, gb.r.H); break;
		case 0xBD: gb.r.L = res(7, gb.r.L); break;
		case 0xBE: write_byte(gb.r.HL, res(7, read_byte(gb.r.HL))); break;
		case 0xBF: gb.r.A = res(7, gb.r.A); break;
		// SET
		case 0xC0: gb.r.B = set(0, gb.r.B); break;
		case 0xC1: gb.r.C = set(0, gb.r.C); break;
		case 0xC2: gb.r.D = set(0, gb.r.D); break;
		case 0xC3: gb.r.E = set(0, gb.r.E); break;
		case 0xC4: gb.r.H = set(0, gb.r.H); break;
		case 0xC5: gb.r.L = set(0, gb.r.L); break;
		case 0xC6: write_byte(gb.r.HL, set(0, read_byte(gb.r.HL))); break;
		case 0xC7: gb.r.A = set(0, gb.r.A); break;

		case 0xC8: gb.r.B = set(1, gb.r.B); break;
		case 0xC9: gb.r.C = set(1, gb.r.C); break;
		case 0xCA: gb.r.D = set(1, gb.r.D); break;
		case 0xCB: gb.r.E = set(1, gb.r.E); break;
		case 0xCC: gb.r.H = set(1, gb.r.H); break;
		case 0xCD: gb.r.L = set(1, gb.r.L); break;
		case 0xCE: write_byte(gb.r.HL, set(1, read_byte(gb.r.HL))); break;
		case 0xCF: gb.r.A = set(1, gb.r.A); break;

		case 0xD0: gb.r.B = set(2, gb.r.B); break;
		case 0xD1: gb.r.C = set(2, gb.r.C); break;
		case 0xD2: gb.r.D = set(2, gb.r.D); break;
		case 0xD3: gb.r.E = set(2, gb.r.E); break;
		case 0xD4: gb.r.H = set(2, gb.r.H); break;
		case 0xD5: gb.r.L = set(2, gb.r.L); break;
		case 0xD6: write_byte(gb.r.HL, set(2, read_byte(gb.r.HL))); break;
		case 0xD7: gb.r.A = set(2, gb.r.A); break;

		case 0xD8: gb.r.B = set(3, gb.r.B); break;
		case 0xD9: gb.r.C = set(3, gb.r.C); break;
		case 0xDA: gb.r.D = set(3, gb.r.D); break;
		case 0xDB: gb.r.E = set(3, gb.r.E); break;
		case 0xDC: gb.r.H = set(3, gb.r.H); break;
		case 0xDD: gb.r.L = set(3, gb.r.L); break;
		case 0xDE: write_byte(gb.r.HL, set(3, read_byte(gb.r.HL))); break;
		case 0xDF: gb.r.A = set(3, gb.r.A); break;

		case 0xE0: gb.r.B = set(4, gb.r.B); break;
		case 0xE1: gb.r.C = set(4, gb.r.C); break;
		case 0xE2: gb.r.D = set(4, gb.r.D); break;
		case 0xE3: gb.r.E = set(4, gb.r.E); break;
		case 0xE4: gb.r.H = set(4, gb.r.H); break;
		case 0xE5: gb.r.L = set(4, gb.r.L); break;
		case 0xE6: write_byte(gb.r.HL, set(4, read_byte(gb.r.HL))); break;
		case 0xE7: gb.r.A = set(4, gb.r.A); break;

		case 0xE8: gb.r.B = set(5, gb.r.B); break;
		case 0xE9: gb.r.C = set(5, gb.r.C); break;
		case 0xEA: gb.r.D = set(5, gb.r.D); break;
		case 0xEB: gb.r.E = set(5, gb.r.E); break;
		case 0xEC: gb.r.H = set(5, gb.r.H); break;
		case 0xED: gb.r.L = set(5, gb.r.L); break;
		case 0xEE: write_byte(gb.r.HL, set(5, read_byte(gb.r.HL))); break;
		case 0xEF: gb.r.A = set(5, gb.r.A); break;

		case 0xF0: gb.r.B = set(6, gb.r.B); break;
		case 0xF1: gb.r.C = set(6, gb.r.C); break;
		case 0xF2: gb.r.D = set(6, gb.r.D); break;
		case 0xF3: gb.r.E = set(6, gb.r.E); break;
		case 0xF4: gb.r.H = set(6, gb.r.H); break;
		case 0xF5: gb.r.L = set(6, gb.r.L); break;
		case 0xF6: write_byte(gb.r.HL, set(6, read_byte(gb.r.HL))); break;
		case 0xF7: gb.r.A = set(6, gb.r.A); break;

		case 0xF8: gb.r.B = set(7, gb.r.B); break;
		case 0xF9: gb.r.C = set(7, gb.r.C); break;
		case 0xFA: gb.r.D = set(7, gb.r.D); break;
		case 0xFB: gb.r.E = set(7, gb.r.E); break;
		case 0xFC: gb.r.H = set(7, gb.r.H); break;
		case 0xFD: gb.r.L = set(7, gb.r.L); break;
		case 0xFE: write_byte(gb.r.HL, set(7, read_byte(gb.r.HL))); break;
		case 0xFF: gb.r.A = set(7, gb.r.A); break;

		default:
			printf("CB opcode %.2X not implemented\n", opcode);
			exit(0);
	}
}