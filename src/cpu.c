#include <string.h>
#include "cpu.h"
#include "irqs.h"
#include "instructions.h"
#include "mem.h"

void cpu_run(gameboy* gb)
{
	// Enabling IME is delayed 1 cycle
	if (gb->ime_delay == 1) 
	{
		gb->ime_delay++;
	}
	else if (gb->ime_delay == 2) 
	{
		gb->IME = true;
		gb->ime_delay = 0;
	}
	
	// halt
	if (gb->halt) 
	{
		if (gb->mem[INTERRUPT_ENABLE] && gb->mem[INTERRUPT_FLAG])
			gb->halt = 0;
		else 
		{
			gb->cycles += 4;
			return;
		}
	}

	handle_interrupts(gb);

	uint8_t opcode = read_byte(gb, gb->PC++);
		
	switch (opcode) 
	{
		// 8-bit Load Instructions
		case 0x40: gb->r.B = gb->r.B; break;
		case 0x41: gb->r.B = gb->r.C; break;
		case 0x42: gb->r.B = gb->r.D; break;
		case 0x43: gb->r.B = gb->r.E; break;
		case 0x44: gb->r.B = gb->r.H; break;
		case 0x45: gb->r.B = gb->r.L; break;
		case 0x46: gb->r.B = read_byte(gb, gb->r.HL); break;
		case 0x47: gb->r.B = gb->r.A; break;

		case 0x48: gb->r.C = gb->r.B; break;
		case 0x49: gb->r.C = gb->r.C; break;
		case 0x4A: gb->r.C = gb->r.D; break;
		case 0x4B: gb->r.C = gb->r.E; break;
		case 0x4C: gb->r.C = gb->r.H; break;
		case 0x4D: gb->r.C = gb->r.L; break;
		case 0x4E: gb->r.C = read_byte(gb, gb->r.HL); break;
		case 0x4F: gb->r.C = gb->r.A; break;

		case 0x50: gb->r.D = gb->r.B; break;
		case 0x51: gb->r.D = gb->r.C; break;
		case 0x52: gb->r.D = gb->r.D; break;
		case 0x53: gb->r.D = gb->r.E; break;
		case 0x54: gb->r.D = gb->r.H; break;
		case 0x55: gb->r.D = gb->r.L; break;
		case 0x56: gb->r.D = read_byte(gb, gb->r.HL); break;
		case 0x57: gb->r.D = gb->r.A; break;

		case 0x58: gb->r.E = gb->r.B; break;
		case 0x59: gb->r.E = gb->r.C; break;
		case 0x5A: gb->r.E = gb->r.D; break;
		case 0x5B: gb->r.E = gb->r.E; break;
		case 0x5C: gb->r.E = gb->r.H; break;
		case 0x5D: gb->r.E = gb->r.L; break;
		case 0x5E: gb->r.E = read_byte(gb, gb->r.HL); break;
		case 0x5F: gb->r.E = gb->r.A; break;

		case 0x60: gb->r.H = gb->r.B; break;
		case 0x61: gb->r.H = gb->r.C; break;
		case 0x62: gb->r.H = gb->r.D; break;
		case 0x63: gb->r.H = gb->r.E; break;
		case 0x64: gb->r.H = gb->r.H; break;
		case 0x65: gb->r.H = gb->r.L; break;
		case 0x66: gb->r.H = read_byte(gb, gb->r.HL); break;
		case 0x67: gb->r.H = gb->r.A; break;

		case 0x68: gb->r.L = gb->r.B; break;
		case 0x69: gb->r.L = gb->r.C; break;
		case 0x6A: gb->r.L = gb->r.D; break;
		case 0x6B: gb->r.L = gb->r.E; break;
		case 0x6C: gb->r.L = gb->r.H; break;
		case 0x6D: gb->r.L = gb->r.L; break;
		case 0x6E: gb->r.L = read_byte(gb, gb->r.HL); break;
		case 0x6F: gb->r.L = gb->r.A; break;

		case 0x70: write_byte(gb, gb->r.HL, gb->r.B); break;
		case 0x71: write_byte(gb, gb->r.HL, gb->r.C); break;
		case 0x72: write_byte(gb, gb->r.HL, gb->r.D); break;
		case 0x73: write_byte(gb, gb->r.HL, gb->r.E); break;
		case 0x74: write_byte(gb, gb->r.HL, gb->r.H); break;
		case 0x75: write_byte(gb, gb->r.HL, gb->r.L); break;
		case 0x77: write_byte(gb, gb->r.HL, gb->r.A); break;

		case 0x78: gb->r.A = gb->r.B; break;
		case 0x79: gb->r.A = gb->r.C; break;
		case 0x7A: gb->r.A = gb->r.D; break;
		case 0x7B: gb->r.A = gb->r.E; break;
		case 0x7C: gb->r.A = gb->r.H; break;
		case 0x7D: gb->r.A = gb->r.L; break;
		case 0x7E: gb->r.A = read_byte(gb, gb->r.HL); break;
		case 0x7F: gb->r.A = gb->r.A; break;

		case 0x02: write_byte(gb, gb->r.BC, gb->r.A); break;
		case 0x12: write_byte(gb, gb->r.DE, gb->r.A); break;
		case 0x22: write_byte(gb, gb->r.HL++, gb->r.A); break;
		case 0x32: write_byte(gb, gb->r.HL--, gb->r.A); break;

		case 0x06: gb->r.B = read_byte(gb, gb->PC++); break;
		case 0x0E: gb->r.C = read_byte(gb, gb->PC++); break;
		case 0x16: gb->r.D = read_byte(gb, gb->PC++); break;
		case 0x1E: gb->r.E = read_byte(gb, gb->PC++); break;
		case 0x26: gb->r.H = read_byte(gb, gb->PC++); break;
		case 0x2E: gb->r.L = read_byte(gb, gb->PC++); break;
		case 0x36: write_byte(gb, gb->r.HL, read_byte(gb, gb->PC++)); break;
		case 0x3E: gb->r.A = read_byte(gb, gb->PC++); break;

		case 0x0A: gb->r.A = read_byte(gb, gb->r.BC); break;
		case 0x1A: gb->r.A = read_byte(gb, gb->r.DE); break;
		case 0x2A: gb->r.A = read_byte(gb, gb->r.HL++); break;
		case 0x3A: gb->r.A = read_byte(gb, gb->r.HL--); break;

		case 0xE0: write_byte(gb, 0xFF00 + read_byte(gb, gb->PC++), gb->r.A); break;
		case 0xF0: gb->r.A = read_byte(gb, 0xFF00 + read_byte(gb, gb->PC++)); break;
		case 0xE2: write_byte(gb, 0xFF00 + gb->r.C, gb->r.A); break;
		case 0xF2: gb->r.A = read_byte(gb, 0xFF00 + gb->r.C); break;
		case 0xEA: write_byte(gb, read_word(gb, gb->PC), gb->r.A); gb->PC += 2; break;
		case 0xFA: gb->r.A = read_byte(gb, read_word(gb, gb->PC)); gb->PC+= 2; break;

			// 16-bit Load Instructions
		case 0x01: gb->r.BC = read_byte(gb, gb->PC++) | (read_byte(gb, gb->PC++) << 8); break;
		case 0x11: gb->r.DE = read_byte(gb, gb->PC++) | (read_byte(gb, gb->PC++) << 8); break;
		case 0x21: gb->r.HL = read_byte(gb, gb->PC++) | (read_byte(gb, gb->PC++) << 8); break;
		case 0xF8: ld_hl_sp_i8(gb); break;
		case 0x31: gb->SP = read_byte(gb, gb->PC++) | (read_byte(gb, gb->PC++) << 8); break;
		case 0xF9: gb->SP = gb->r.HL; gb->cycles += 4; break;
		case 0x08: {
			uint16_t address = read_byte(gb, gb->PC++) | (read_byte(gb, gb->PC++) << 8);
			write_byte(gb, address, gb->SP & 0xFF);
			write_byte(gb, address + 1, gb->SP >> 8);
		}
		break;
		case 0xC5: case 0xD5: case 0xE5: case 0xF5:
			push(gb, opcode);
			break;
		case 0xC1: case 0xD1: case 0xE1: case 0xF1:
			pop(gb, opcode);
			break;

		case 0x80: add(gb, gb->r.B); break;
		case 0x81: add(gb, gb->r.C); break;
		case 0x82: add(gb, gb->r.D); break;
		case 0x83: add(gb, gb->r.E); break;
		case 0x84: add(gb, gb->r.H); break;
		case 0x85: add(gb, gb->r.L); break;
		case 0x86: add(gb, read_byte(gb, gb->r.HL)); break;
		case 0x87: add(gb, gb->r.A); break;
		case 0xC6: add(gb, read_byte(gb, gb->PC++)); break;
		case 0x09: add16(gb, gb->r.BC); break;
		case 0x19: add16(gb, gb->r.DE); break;
		case 0x29: add16(gb, gb->r.HL); break;
		case 0x39: add16(gb, gb->SP); break;
		case 0xE8: add_sp_i8(gb);	break;

		case 0x88: adc(gb, gb->r.B); break;
		case 0x89: adc(gb, gb->r.C); break;
		case 0x8A: adc(gb, gb->r.D); break;
		case 0x8B: adc(gb, gb->r.E); break;
		case 0x8C: adc(gb, gb->r.H); break;
		case 0x8D: adc(gb, gb->r.L); break;
		case 0x8E: adc(gb, read_byte(gb, gb->r.HL)); break;
		case 0x8F: adc(gb, gb->r.A); break;
		case 0xCE: adc(gb, read_byte(gb, gb->PC++)); break;

		case 0x90: sub(gb, gb->r.B); break;
		case 0x91: sub(gb, gb->r.C); break;
		case 0x92: sub(gb, gb->r.D); break;
		case 0x93: sub(gb, gb->r.E); break;
		case 0x94: sub(gb, gb->r.H); break;
		case 0x95: sub(gb, gb->r.L); break;
		case 0x96: sub(gb, read_byte(gb, gb->r.HL)); break;
		case 0x97: sub(gb, gb->r.A); break;
		case 0xD6: sub(gb, read_byte(gb, gb->PC++)); break;

		case 0x98: sbc(gb, gb->r.B); break;
		case 0x99: sbc(gb, gb->r.C); break;
		case 0x9A: sbc(gb, gb->r.D); break;
		case 0x9B: sbc(gb, gb->r.E); break;
		case 0x9C: sbc(gb, gb->r.H); break;
		case 0x9D: sbc(gb, gb->r.L); break;
		case 0x9E: sbc(gb, read_byte(gb, gb->r.HL)); break;
		case 0x9F: sbc(gb, gb->r.A); break;
		case 0xDE: sbc(gb, read_byte(gb, gb->PC++)); break;

		case 0x07: rlca(gb); break;
		case 0x0F: rrca(gb); break;
		case 0x17: rla(gb); break;
		case 0x1F: rra(gb); break;

		case 0xA0: and (gb, gb->r.B); break;
		case 0xA1: and (gb, gb->r.C); break;
		case 0xA2: and (gb, gb->r.D); break;
		case 0xA3: and (gb, gb->r.E); break;
		case 0xA4: and (gb, gb->r.H); break;
		case 0xA5: and (gb, gb->r.L); break;
		case 0xA6: and (gb, read_byte(gb, gb->r.HL)); break;
		case 0xA7: and (gb, gb->r.A); break;
		case 0xE6: and (gb, read_byte(gb, gb->PC++)); break;

		case 0xA8: xor (gb, gb->r.B); break;
		case 0xA9: xor (gb, gb->r.C); break;
		case 0xAA: xor (gb, gb->r.D); break;
		case 0xAB: xor (gb, gb->r.E); break;
		case 0xAC: xor (gb, gb->r.H); break;
		case 0xAD: xor (gb, gb->r.L); break;
		case 0xAE: xor (gb, read_byte(gb, gb->r.HL)); break;
		case 0xAF: xor (gb, gb->r.A); break;
		case 0xEE: xor (gb, read_byte(gb, gb->PC++)); break;
				
		case 0xB0: or (gb, gb->r.B); break;
		case 0xB1: or (gb, gb->r.C); break;
		case 0xB2: or (gb, gb->r.D); break;
		case 0xB3: or (gb, gb->r.E); break;
		case 0xB4: or (gb, gb->r.H); break;
		case 0xB5: or (gb, gb->r.L); break;
		case 0xB6: or (gb, read_byte(gb, gb->r.HL)); break;
		case 0xB7: or (gb, gb->r.A); break;
		case 0xF6: or (gb, read_byte(gb, gb->PC++)); break;
				
		case 0xB8: cp(gb, gb->r.B); break;
		case 0xB9: cp(gb, gb->r.C); break;
		case 0xBA: cp(gb, gb->r.D); break;
		case 0xBB: cp(gb, gb->r.E); break;
		case 0xBC: cp(gb, gb->r.H); break;
		case 0xBD: cp(gb, gb->r.L); break;
		case 0xBE: cp(gb, read_byte(gb, gb->r.HL)); break;
		case 0xBF: cp(gb, gb->r.A); break;
		case 0xFE: cp(gb, read_byte(gb, gb->PC++)); break;
				
		case 0x04: case 0x14: case 0x24: case 0x34: case 0x0C: case 0x1C: case 0x2C: case 0x3C:
			inc_8(gb, opcode);
			break;
		case 0x03: case 0x13: case 0x23: case 0x33:
			inc_16(gb, opcode);
			break;
		case 0x05: dec(gb, &gb->r.B); break;
		case 0x0D: dec(gb, &gb->r.C); break;
		case 0x15: dec(gb, &gb->r.D); break;
		case 0x1D: dec(gb, &gb->r.E); break;
		case 0x25: dec(gb, &gb->r.H); break;
		case 0x2D: dec(gb, &gb->r.L); break;
		case 0x35: dec_hl(gb); break;
		case 0x3D: dec(gb, &gb->r.A); break;
		case 0x0B: dec16(gb, &gb->r.BC); break;
		case 0x1B: dec16(gb, &gb->r.DE); break;
		case 0x2B: dec16(gb, &gb->r.HL); break;
		case 0x3B: dec16(gb, &gb->SP); break;
		case 0x27: daa(gb); break;
		case 0x2F: cpl(gb); break;

		case 0x20: case 0x30: case 0x18: case 0x28: case 0x38:
			jr(gb, opcode);
			break;
				
		case 0xCB: prefix_cb(gb); break;
				
		case 0xC2: jp(gb, read_byte(gb, gb->PC++) + (read_byte(gb, gb->PC++) << 8), opcode); break;
		case 0xD2: jp(gb, read_byte(gb, gb->PC++) + (read_byte(gb, gb->PC++) << 8), opcode); break;
		case 0xCA: jp(gb, read_byte(gb, gb->PC++) + (read_byte(gb, gb->PC++) << 8), opcode); break;
		case 0xDA: jp(gb, read_byte(gb, gb->PC++) + (read_byte(gb, gb->PC++) << 8), opcode); break;
		case 0xE9: gb->PC = gb->r.HL; break;
		case 0xC3: gb->PC = (read_byte(gb, gb->PC++) + (read_byte(gb, gb->PC++) << 8)); break;
				
		case 0xC4: case 0xD4: case 0xCC: case 0xDC: case 0xCD:
			call(gb, opcode);
			break;
		case 0xC0: case 0xD0: case 0xC8: case 0xD8: case 0xC9: case 0xD9:
			ret(gb, opcode);
			break;
		case 0xC7: rst(gb, 0x00); break;
		case 0xD7: rst(gb, 0x10); break;
		case 0xE7: rst(gb, 0x20); break;
		case 0xF7: rst(gb, 0x30); break;
		case 0xCF: rst(gb, 0x08); break;
		case 0xDF: rst(gb, 0x18); break;
		case 0xEF: rst(gb, 0x28); break;
		case 0xFF: rst(gb, 0x38); break;
		case 0x3F: ccf(gb); break;
		case 0x37: scf(gb); break;
		case 0x00: break; // noop
		case 0x10: break; // STOP: TODO
		case 0x76: halt(gb); break;
		case 0xF3: di(gb); break;
		case 0xFB: ei(gb); break;
		default:
			printf("opcode %.2X not implemented\n", opcode);
			exit(0);
	}
}

void prefix_cb(gameboy* gb) 
{
	uint8_t opcode = read_byte(gb, gb->PC++);

	switch (opcode) 
	{
		// RLC
		case 0x00: gb->r.B = rlc(gb, gb->r.B); break;
		case 0x01: gb->r.C = rlc(gb, gb->r.C); break;
		case 0x02: gb->r.D = rlc(gb, gb->r.D); break;
		case 0x03: gb->r.E = rlc(gb, gb->r.E); break;
		case 0x04: gb->r.H = rlc(gb, gb->r.H); break;
		case 0x05: gb->r.L = rlc(gb, gb->r.L); break;
		case 0x06: write_byte(gb, gb->r.HL, rlc(gb, read_byte(gb, gb->r.HL))); break;
		case 0x07: gb->r.A = rlc(gb, gb->r.A); break;
		// RRC
		case 0x08: gb->r.B = rrc(gb, gb->r.B); break;
		case 0x09: gb->r.C = rrc(gb, gb->r.C); break;
		case 0x0A: gb->r.D = rrc(gb, gb->r.D); break;
		case 0x0B: gb->r.E = rrc(gb, gb->r.E); break;
		case 0x0C: gb->r.H = rrc(gb, gb->r.H); break;
		case 0x0D: gb->r.L = rrc(gb, gb->r.L); break;
		case 0x0E: write_byte(gb, gb->r.HL, rrc(gb, read_byte(gb, gb->r.HL))); break;
		case 0x0F: gb->r.A = rrc(gb, gb->r.A); break;
		// RL
		case 0x10: gb->r.B = rl(gb, gb->r.B); break;
		case 0x11: gb->r.C = rl(gb, gb->r.C); break;
		case 0x12: gb->r.D = rl(gb, gb->r.D); break;
		case 0x13: gb->r.E = rl(gb, gb->r.E); break;
		case 0x14: gb->r.H = rl(gb, gb->r.H); break;
		case 0x15: gb->r.L = rl(gb, gb->r.L); break;
		case 0x16: write_byte(gb, gb->r.HL, rl(gb, read_byte(gb, gb->r.HL))); break;
		case 0x17: gb->r.A = rl(gb, gb->r.A); break;
		// RR
		case 0x18: gb->r.B = rr(gb, gb->r.B); break;
		case 0x19: gb->r.C = rr(gb, gb->r.C); break;
		case 0x1A: gb->r.D = rr(gb, gb->r.D); break;
		case 0x1B: gb->r.E = rr(gb, gb->r.E); break;
		case 0x1C: gb->r.H = rr(gb, gb->r.H); break;
		case 0x1D: gb->r.L = rr(gb, gb->r.L); break;
		case 0x1E: write_byte(gb, gb->r.HL, rr(gb, read_byte(gb, gb->r.HL))); break;
		case 0x1F: gb->r.A = rr(gb, gb->r.A); break;
		// SLA
		case 0x20: gb->r.B = sla(gb, gb->r.B); break;
		case 0x21: gb->r.C = sla(gb, gb->r.C); break;
		case 0x22: gb->r.D = sla(gb, gb->r.D); break;
		case 0x23: gb->r.E = sla(gb, gb->r.E); break;
		case 0x24: gb->r.H = sla(gb, gb->r.H); break;
		case 0x25: gb->r.L = sla(gb, gb->r.L); break;
		case 0x26: write_byte(gb, gb->r.HL, sla(gb, read_byte(gb, gb->r.HL))); break;
		case 0x27: gb->r.A = sla(gb, gb->r.A); break;
		// SRA
		case 0x28: gb->r.B = sra(gb, gb->r.B); break;
		case 0x29: gb->r.C = sra(gb, gb->r.C); break;
		case 0x2A: gb->r.D = sra(gb, gb->r.D); break;
		case 0x2B: gb->r.E = sra(gb, gb->r.E); break;
		case 0x2C: gb->r.H = sra(gb, gb->r.H); break;
		case 0x2D: gb->r.L = sra(gb, gb->r.L); break;
		case 0x2E: write_byte(gb, gb->r.HL, sra(gb, read_byte(gb, gb->r.HL))); break;
		case 0x2F: gb->r.A = sra(gb, gb->r.A); break;
		// SWAP
		case 0x30: gb->r.B = swap(gb, gb->r.B); break;
		case 0x31: gb->r.C = swap(gb, gb->r.C); break;
		case 0x32: gb->r.D = swap(gb, gb->r.D); break;
		case 0x33: gb->r.E = swap(gb, gb->r.E); break;
		case 0x34: gb->r.H = swap(gb, gb->r.H); break;
		case 0x35: gb->r.L = swap(gb, gb->r.L); break;
		case 0x36: write_byte(gb, gb->r.HL, swap(gb, read_byte(gb, gb->r.HL))); break;
		case 0x37: gb->r.A = swap(gb, gb->r.A); break;
		// SRL
		case 0x38: gb->r.B = srl(gb, gb->r.B); break;
		case 0x39: gb->r.C = srl(gb, gb->r.C); break;
		case 0x3A: gb->r.D = srl(gb, gb->r.D); break;
		case 0x3B: gb->r.E = srl(gb, gb->r.E); break;
		case 0x3C: gb->r.H = srl(gb, gb->r.H); break;
		case 0x3D: gb->r.L = srl(gb, gb->r.L); break;
		case 0x3E: write_byte(gb, gb->r.HL, srl(gb, read_byte(gb, gb->r.HL))); break;
		case 0x3F: gb->r.A = srl(gb, gb->r.A); break;
		// BIT
		case 0x40: gb->r.B = bit(gb, 0, gb->r.B); break;
		case 0x41: gb->r.C = bit(gb, 0, gb->r.C); break;
		case 0x42: gb->r.D = bit(gb, 0, gb->r.D); break;
		case 0x43: gb->r.E = bit(gb, 0, gb->r.E); break;
		case 0x44: gb->r.H = bit(gb, 0, gb->r.H); break;
		case 0x45: gb->r.L = bit(gb, 0, gb->r.L); break;
		case 0x46: write_byte(gb, gb->r.HL, bit(gb, 0, read_byte(gb, gb->r.HL))); break;
		case 0x47: gb->r.A = bit(gb, 0, gb->r.A); break;

		case 0x48: gb->r.B = bit(gb, 1, gb->r.B); break;
		case 0x49: gb->r.C = bit(gb, 1, gb->r.C); break;
		case 0x4A: gb->r.D = bit(gb, 1, gb->r.D); break;
		case 0x4B: gb->r.E = bit(gb, 1, gb->r.E); break;
		case 0x4C: gb->r.H = bit(gb, 1, gb->r.H); break;
		case 0x4D: gb->r.L = bit(gb, 1, gb->r.L); break;
		case 0x4E: write_byte(gb, gb->r.HL, bit(gb, 1, read_byte(gb, gb->r.HL))); break;
		case 0x4F: gb->r.A = bit(gb, 1, gb->r.A); break;
			
		case 0x50: gb->r.B = bit(gb, 2, gb->r.B); break;
		case 0x51: gb->r.C = bit(gb, 2, gb->r.C); break;
		case 0x52: gb->r.D = bit(gb, 2, gb->r.D); break;
		case 0x53: gb->r.E = bit(gb, 2, gb->r.E); break;
		case 0x54: gb->r.H = bit(gb, 2, gb->r.H); break;
		case 0x55: gb->r.L = bit(gb, 2, gb->r.L); break;
		case 0x56: write_byte(gb, gb->r.HL, bit(gb, 2, read_byte(gb, gb->r.HL))); break;
		case 0x57: gb->r.A = bit(gb, 2, gb->r.A); break;

		case 0x58: gb->r.B = bit(gb, 3, gb->r.B); break;
		case 0x59: gb->r.C = bit(gb, 3, gb->r.C); break;
		case 0x5A: gb->r.D = bit(gb, 3, gb->r.D); break;
		case 0x5B: gb->r.E = bit(gb, 3, gb->r.E); break;
		case 0x5C: gb->r.H = bit(gb, 3, gb->r.H); break;
		case 0x5D: gb->r.L = bit(gb, 3, gb->r.L); break;
		case 0x5E: write_byte(gb, gb->r.HL, bit(gb, 3, read_byte(gb, gb->r.HL))); break;
		case 0x5F: gb->r.A = bit(gb, 3, gb->r.A); break;
			
		case 0x60: gb->r.B = bit(gb, 4, gb->r.B); break;
		case 0x61: gb->r.C = bit(gb, 4, gb->r.C); break;
		case 0x62: gb->r.D = bit(gb, 4, gb->r.D); break;
		case 0x63: gb->r.E = bit(gb, 4, gb->r.E); break;
		case 0x64: gb->r.H = bit(gb, 4, gb->r.H); break;
		case 0x65: gb->r.L = bit(gb, 4, gb->r.L); break;
		case 0x66: write_byte(gb, gb->r.HL, bit(gb, 4, read_byte(gb, gb->r.HL))); break;
		case 0x67: gb->r.A = bit(gb, 4, gb->r.A); break;

		case 0x68: gb->r.B = bit(gb, 5, gb->r.B); break;
		case 0x69: gb->r.C = bit(gb, 5, gb->r.C); break;
		case 0x6A: gb->r.D = bit(gb, 5, gb->r.D); break;
		case 0x6B: gb->r.E = bit(gb, 5, gb->r.E); break;
		case 0x6C: gb->r.H = bit(gb, 5, gb->r.H); break;
		case 0x6D: gb->r.L = bit(gb, 5, gb->r.L); break;
		case 0x6E: write_byte(gb, gb->r.HL, bit(gb, 5, read_byte(gb, gb->r.HL))); break;
		case 0x6F: gb->r.A = bit(gb, 5, gb->r.A); break;
			
		case 0x70: gb->r.B = bit(gb, 6, gb->r.B); break;
		case 0x71: gb->r.C = bit(gb, 6, gb->r.C); break;
		case 0x72: gb->r.D = bit(gb, 6, gb->r.D); break;
		case 0x73: gb->r.E = bit(gb, 6, gb->r.E); break;
		case 0x74: gb->r.H = bit(gb, 6, gb->r.H); break;
		case 0x75: gb->r.L = bit(gb, 6, gb->r.L); break;
		case 0x76: write_byte(gb, gb->r.HL, bit(gb, 6, read_byte(gb, gb->r.HL))); break;
		case 0x77: gb->r.A = bit(gb, 6, gb->r.A); break;

		case 0x78: gb->r.B = bit(gb, 7, gb->r.B); break;
		case 0x79: gb->r.C = bit(gb, 7, gb->r.C); break;
		case 0x7A: gb->r.D = bit(gb, 7, gb->r.D); break;
		case 0x7B: gb->r.E = bit(gb, 7, gb->r.E); break;
		case 0x7C: gb->r.H = bit(gb, 7, gb->r.H); break;
		case 0x7D: gb->r.L = bit(gb, 7, gb->r.L); break;
		case 0x7E: write_byte(gb, gb->r.HL, bit(gb, 7, read_byte(gb, gb->r.HL))); break;
		case 0x7F: gb->r.A = bit(gb, 7, gb->r.A); break;
		// RES
		case 0x80: gb->r.B = res(gb, 0, gb->r.B); break;
		case 0x81: gb->r.C = res(gb, 0, gb->r.C); break;
		case 0x82: gb->r.D = res(gb, 0, gb->r.D); break;
		case 0x83: gb->r.E = res(gb, 0, gb->r.E); break;
		case 0x84: gb->r.H = res(gb, 0, gb->r.H); break;
		case 0x85: gb->r.L = res(gb, 0, gb->r.L); break;
		case 0x86: write_byte(gb, gb->r.HL, res(gb, 0, read_byte(gb, gb->r.HL))); break;
		case 0x87: gb->r.A = res(gb, 0, gb->r.A); break;

		case 0x88: gb->r.B = res(gb, 1, gb->r.B); break;
		case 0x89: gb->r.C = res(gb, 1, gb->r.C); break;
		case 0x8A: gb->r.D = res(gb, 1, gb->r.D); break;
		case 0x8B: gb->r.E = res(gb, 1, gb->r.E); break;
		case 0x8C: gb->r.H = res(gb, 1, gb->r.H); break;
		case 0x8D: gb->r.L = res(gb, 1, gb->r.L); break;
		case 0x8E: write_byte(gb, gb->r.HL, res(gb, 1, read_byte(gb, gb->r.HL))); break;
		case 0x8F: gb->r.A = res(gb, 1, gb->r.A); break;

		case 0x90: gb->r.B = res(gb, 2, gb->r.B); break;
		case 0x91: gb->r.C = res(gb, 2, gb->r.C); break;
		case 0x92: gb->r.D = res(gb, 2, gb->r.D); break;
		case 0x93: gb->r.E = res(gb, 2, gb->r.E); break;
		case 0x94: gb->r.H = res(gb, 2, gb->r.H); break;
		case 0x95: gb->r.L = res(gb, 2, gb->r.L); break;
		case 0x96: write_byte(gb, gb->r.HL, res(gb, 2, read_byte(gb, gb->r.HL))); break;
		case 0x97: gb->r.A = res(gb, 2, gb->r.A); break;

		case 0x98: gb->r.B = res(gb, 3, gb->r.B); break;
		case 0x99: gb->r.C = res(gb, 3, gb->r.C); break;
		case 0x9A: gb->r.D = res(gb, 3, gb->r.D); break;
		case 0x9B: gb->r.E = res(gb, 3, gb->r.E); break;
		case 0x9C: gb->r.H = res(gb, 3, gb->r.H); break;
		case 0x9D: gb->r.L = res(gb, 3, gb->r.L); break;
		case 0x9E: write_byte(gb, gb->r.HL, res(gb, 3, read_byte(gb, gb->r.HL))); break;
		case 0x9F: gb->r.A = res(gb, 3, gb->r.A); break;

		case 0xA0: gb->r.B = res(gb, 4, gb->r.B); break;
		case 0xA1: gb->r.C = res(gb, 4, gb->r.C); break;
		case 0xA2: gb->r.D = res(gb, 4, gb->r.D); break;
		case 0xA3: gb->r.E = res(gb, 4, gb->r.E); break;
		case 0xA4: gb->r.H = res(gb, 4, gb->r.H); break;
		case 0xA5: gb->r.L = res(gb, 4, gb->r.L); break;
		case 0xA6: write_byte(gb, gb->r.HL, res(gb, 4, read_byte(gb, gb->r.HL))); break;
		case 0xA7: gb->r.A = res(gb, 4, gb->r.A); break;

		case 0xA8: gb->r.B = res(gb, 5, gb->r.B); break;
		case 0xA9: gb->r.C = res(gb, 5, gb->r.C); break;
		case 0xAA: gb->r.D = res(gb, 5, gb->r.D); break;
		case 0xAB: gb->r.E = res(gb, 5, gb->r.E); break;
		case 0xAC: gb->r.H = res(gb, 5, gb->r.H); break;
		case 0xAD: gb->r.L = res(gb, 5, gb->r.L); break;
		case 0xAE: write_byte(gb, gb->r.HL, res(gb, 5, read_byte(gb, gb->r.HL))); break;
		case 0xAF: gb->r.A = res(gb, 5, gb->r.A); break;

		case 0xB0: gb->r.B = res(gb, 6, gb->r.B); break;
		case 0xB1: gb->r.C = res(gb, 6, gb->r.C); break;
		case 0xB2: gb->r.D = res(gb, 6, gb->r.D); break;
		case 0xB3: gb->r.E = res(gb, 6, gb->r.E); break;
		case 0xB4: gb->r.H = res(gb, 6, gb->r.H); break;
		case 0xB5: gb->r.L = res(gb, 6, gb->r.L); break;
		case 0xB6: write_byte(gb, gb->r.HL, res(gb, 6, read_byte(gb, gb->r.HL))); break;
		case 0xB7: gb->r.A = res(gb, 6, gb->r.A); break;

		case 0xB8: gb->r.B = res(gb, 7, gb->r.B); break;
		case 0xB9: gb->r.C = res(gb, 7, gb->r.C); break;
		case 0xBA: gb->r.D = res(gb, 7, gb->r.D); break;
		case 0xBB: gb->r.E = res(gb, 7, gb->r.E); break;
		case 0xBC: gb->r.H = res(gb, 7, gb->r.H); break;
		case 0xBD: gb->r.L = res(gb, 7, gb->r.L); break;
		case 0xBE: write_byte(gb, gb->r.HL, res(gb, 7, read_byte(gb, gb->r.HL))); break;
		case 0xBF: gb->r.A = res(gb, 7, gb->r.A); break;
		// SET
		case 0xC0: gb->r.B = set(gb, 0, gb->r.B); break;
		case 0xC1: gb->r.C = set(gb, 0, gb->r.C); break;
		case 0xC2: gb->r.D = set(gb, 0, gb->r.D); break;
		case 0xC3: gb->r.E = set(gb, 0, gb->r.E); break;
		case 0xC4: gb->r.H = set(gb, 0, gb->r.H); break;
		case 0xC5: gb->r.L = set(gb, 0, gb->r.L); break;
		case 0xC6: write_byte(gb, gb->r.HL, set(gb, 0, read_byte(gb, gb->r.HL))); break;
		case 0xC7: gb->r.A = set(gb, 0, gb->r.A); break;

		case 0xC8: gb->r.B = set(gb, 1, gb->r.B); break;
		case 0xC9: gb->r.C = set(gb, 1, gb->r.C); break;
		case 0xCA: gb->r.D = set(gb, 1, gb->r.D); break;
		case 0xCB: gb->r.E = set(gb, 1, gb->r.E); break;
		case 0xCC: gb->r.H = set(gb, 1, gb->r.H); break;
		case 0xCD: gb->r.L = set(gb, 1, gb->r.L); break;
		case 0xCE: write_byte(gb, gb->r.HL, set(gb, 1, read_byte(gb, gb->r.HL))); break;
		case 0xCF: gb->r.A = set(gb, 1, gb->r.A); break;

		case 0xD0: gb->r.B = set(gb, 2, gb->r.B); break;
		case 0xD1: gb->r.C = set(gb, 2, gb->r.C); break;
		case 0xD2: gb->r.D = set(gb, 2, gb->r.D); break;
		case 0xD3: gb->r.E = set(gb, 2, gb->r.E); break;
		case 0xD4: gb->r.H = set(gb, 2, gb->r.H); break;
		case 0xD5: gb->r.L = set(gb, 2, gb->r.L); break;
		case 0xD6: write_byte(gb, gb->r.HL, set(gb, 2, read_byte(gb, gb->r.HL))); break;
		case 0xD7: gb->r.A = set(gb, 2, gb->r.A); break;

		case 0xD8: gb->r.B = set(gb, 3, gb->r.B); break;
		case 0xD9: gb->r.C = set(gb, 3, gb->r.C); break;
		case 0xDA: gb->r.D = set(gb, 3, gb->r.D); break;
		case 0xDB: gb->r.E = set(gb, 3, gb->r.E); break;
		case 0xDC: gb->r.H = set(gb, 3, gb->r.H); break;
		case 0xDD: gb->r.L = set(gb, 3, gb->r.L); break;
		case 0xDE: write_byte(gb, gb->r.HL, set(gb, 3, read_byte(gb, gb->r.HL))); break;
		case 0xDF: gb->r.A = set(gb, 3, gb->r.A); break;

		case 0xE0: gb->r.B = set(gb, 4, gb->r.B); break;
		case 0xE1: gb->r.C = set(gb, 4, gb->r.C); break;
		case 0xE2: gb->r.D = set(gb, 4, gb->r.D); break;
		case 0xE3: gb->r.E = set(gb, 4, gb->r.E); break;
		case 0xE4: gb->r.H = set(gb, 4, gb->r.H); break;
		case 0xE5: gb->r.L = set(gb, 4, gb->r.L); break;
		case 0xE6: write_byte(gb, gb->r.HL, set(gb, 4, read_byte(gb, gb->r.HL))); break;
		case 0xE7: gb->r.A = set(gb, 4, gb->r.A); break;

		case 0xE8: gb->r.B = set(gb, 5, gb->r.B); break;
		case 0xE9: gb->r.C = set(gb, 5, gb->r.C); break;
		case 0xEA: gb->r.D = set(gb, 5, gb->r.D); break;
		case 0xEB: gb->r.E = set(gb, 5, gb->r.E); break;
		case 0xEC: gb->r.H = set(gb, 5, gb->r.H); break;
		case 0xED: gb->r.L = set(gb, 5, gb->r.L); break;
		case 0xEE: write_byte(gb, gb->r.HL, set(gb, 5, read_byte(gb, gb->r.HL))); break;
		case 0xEF: gb->r.A = set(gb, 5, gb->r.A); break;

		case 0xF0: gb->r.B = set(gb, 6, gb->r.B); break;
		case 0xF1: gb->r.C = set(gb, 6, gb->r.C); break;
		case 0xF2: gb->r.D = set(gb, 6, gb->r.D); break;
		case 0xF3: gb->r.E = set(gb, 6, gb->r.E); break;
		case 0xF4: gb->r.H = set(gb, 6, gb->r.H); break;
		case 0xF5: gb->r.L = set(gb, 6, gb->r.L); break;
		case 0xF6: write_byte(gb, gb->r.HL, set(gb, 6, read_byte(gb, gb->r.HL))); break;
		case 0xF7: gb->r.A = set(gb, 6, gb->r.A); break;

		case 0xF8: gb->r.B = set(gb, 7, gb->r.B); break;
		case 0xF9: gb->r.C = set(gb, 7, gb->r.C); break;
		case 0xFA: gb->r.D = set(gb, 7, gb->r.D); break;
		case 0xFB: gb->r.E = set(gb, 7, gb->r.E); break;
		case 0xFC: gb->r.H = set(gb, 7, gb->r.H); break;
		case 0xFD: gb->r.L = set(gb, 7, gb->r.L); break;
		case 0xFE: write_byte(gb, gb->r.HL, set(gb, 7, read_byte(gb, gb->r.HL))); break;
		case 0xFF: gb->r.A = set(gb, 7, gb->r.A); break;

		default:
			printf("CB opcode %.2X not implemented\n", opcode);
			exit(0);
	}
}