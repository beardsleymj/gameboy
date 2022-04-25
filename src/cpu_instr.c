#include "cpu_instr.h"

void ld_hl_sp_i8() 
{
	int8_t offset = read_byte(gb.PC++);
	gb.r.HF = (gb.SP & 0xF) + (offset & 0xF) > 0xF;
	gb.r.CF = (gb.SP & 0xFF) + (offset & 0xFF) > 0xFF;
	gb.r.ZF = 0;
	gb.r.NF = 0;
	gb.r.HL = gb.SP + offset;
	gb.cycles += 4;
}

void push(u8 opcode) 
{
	gb.cycles += 4;
	switch (opcode)	
	{
		case 0xC5:
			write_byte(--gb.SP, gb.r.B);
			write_byte(--gb.SP, gb.r.C);
			break;
		case 0xD5:
			write_byte(--gb.SP, gb.r.D);
			write_byte(--gb.SP, gb.r.E);
			break;
		case 0xE5:
			write_byte(--gb.SP, gb.r.H);
			write_byte(--gb.SP, gb.r.L);
			break;
		case 0xF5:
			write_byte(--gb.SP, gb.r.A);
			write_byte(--gb.SP, gb.r.F);
			break;
	}
}

void pop(u8 opcode) 
{
	switch (opcode)	
	{
		case 0xC1:
			gb.r.C = read_byte(gb.SP++);
			gb.r.B = read_byte(gb.SP++);
			break;
		case 0xD1:
			gb.r.E = read_byte(gb.SP++);
			gb.r.D = read_byte(gb.SP++);
			break;
		case 0xE1:
			gb.r.L = read_byte(gb.SP++);
			gb.r.H = read_byte(gb.SP++);
			break;
		case 0xF1:
			gb.r.F = read_byte(gb.SP++);
			gb.r.A = read_byte(gb.SP++);
			gb.r.F = (gb.r.F & 0xF0);
			break;
	}
}

void add(u8 value) 
{
	gb.r.NF = 0;
	gb.r.HF = (gb.r.A & 0x0F) + (value & 0x0F) > 0x0F;
	gb.r.CF = gb.r.A > 0xFF - value;
	gb.r.A += value;
	gb.r.ZF = gb.r.A == 0;
}

void add16(u16 value) 
{
	gb.r.NF = 0;
	gb.r.HF = (gb.r.HL & 0x0FFF) + (value & 0x0FFF) > 0x0FFF;
	gb.r.CF = gb.r.HL > 0xFFFF - value;
	gb.r.HL += value;
	gb.cycles += 4;
}

void add_sp_i8() 
{
	int8_t offset = read_byte(gb.PC++);
	gb.r.HF = (gb.SP & 0xF) + (offset & 0xF) > 0xF;
	gb.r.CF = (gb.SP & 0xFF) + (offset & 0xFF) > 0xFF;
	gb.r.ZF = 0;
	gb.r.NF = 0;
	gb.SP += offset;
	gb.cycles += 8;
}

void adc(u8 value) 
{
	u8 CF_old = gb.r.CF;
	gb.r.NF = 0;
	gb.r.HF = (gb.r.A & 0x0F) + (value & 0x0F) + CF_old > 0x0F;
	gb.r.CF = (u16)gb.r.A + value + CF_old> 0xFF;
	gb.r.A += value + CF_old;
	gb.r.ZF = (gb.r.A == 0);
}

void sub(u8 value) 
{
	gb.r.NF = 1;
	gb.r.HF = (gb.r.A & 0x0F) < (value & 0x0F);
	gb.r.CF = gb.r.A < value;
	gb.r.A -= value;
	gb.r.ZF = gb.r.A == 0;
}

void sbc(u8 value) 
{
	bool carry_old = gb.r.CF;
	gb.r.NF = 1;
	gb.r.HF = (gb.r.A & 0x0F) < (u16)(value & 0x0F) + gb.r.CF;
	gb.r.CF = gb.r.A < (u16)value + gb.r.CF;
	gb.r.A = (gb.r.A - value) - carry_old;
	gb.r.ZF = gb.r.A == 0;
}

void rlca() 
{
	gb.r.CF = (gb.r.A >> 7);
	gb.r.A = (gb.r.A << 1) + gb.r.CF;
	gb.r.ZF = 0;
	gb.r.NF = 0;
	gb.r.HF = 0;
}

void rrca() 
{
	gb.r.CF = (gb.r.A & 0x01);
	gb.r.A = (gb.r.A >> 1) + (gb.r.CF << 7);
	gb.r.ZF = 0;
	gb.r.NF = 0;
	gb.r.HF = 0;
}

void rla() 
{
	u8 bit = (gb.r.A >> 7);
	gb.r.A = (gb.r.A << 1) + (gb.r.CF);
	gb.r.NF = 0;
	gb.r.HF = 0;
	gb.r.CF = bit;
	gb.r.ZF = 0;
}

void rra() 
{
	u8 bit = (gb.r.A & 0x01);
	gb.r.A = (gb.r.A >> 1) + (gb.r.CF * 0x80);
	gb.r.NF = 0;
	gb.r.HF = 0;
	gb.r.CF = bit;
	gb.r.ZF = 0;
}

void and (u8 value) 
{
	gb.r.A &= value; 
	gb.r.ZF = gb.r.A == 0;
	gb.r.NF = 0;
	gb.r.HF = 1;
	gb.r.CF = 0;
}

void xor(u8 value) 
{
	gb.r.A ^= value;
	gb.r.ZF = gb.r.A == 0;
	gb.r.NF = 0;
	gb.r.HF = 0;
	gb.r.CF = 0;
}

void inc_8(u8 opcode) {
	u8 value = 0;

	switch (opcode)	{
		case 0x04:
			value = ++gb.r.B;
			break;
		case 0x14:
			value = ++gb.r.D;
			break;
		case 0x24:
			value = ++gb.r.H;
			break;
		case 0x34:
			write_byte(gb.r.HL, read_byte(gb.r.HL) + 1);
			value = read_byte(gb.r.HL);
			break;
		case 0x0C:
			value = ++gb.r.C;
			break;
		case 0x1C:
			value = ++gb.r.E;
			break;
		case 0x2C:
			value = ++gb.r.L;
			break;
		case 0x3C:
			value = ++gb.r.A;
			break;
	}
	gb.r.ZF = value == 0;
	gb.r.NF = 0;
	gb.r.HF = (value & 0x0F) == 0x00; // 0bXXXX0000
}

void inc_16(u8 opcode) 
{
	switch (opcode)	
	{
		case 0x03: gb.r.BC++; break;
		case 0x13: gb.r.DE++; break;
		case 0x23: gb.r.HL++; break;
		case 0x33: gb.SP++; break;
	}
	gb.cycles += 4;
}

void dec(u8* byte) 
{
	gb.r.HF = (*byte & 0x0F) == 0;
	*byte = *byte - 1;
	gb.r.ZF = (*byte == 0);
	gb.r.NF = 1;
}

void dec_hl() 
{
	u8 byte = read_byte(gb.r.HL);
	gb.r.HF = (byte & 0x0F) == 0;
	byte = byte - 1;
	gb.r.ZF = (byte == 0);
	gb.r.NF = 1;
	write_byte(gb.r.HL, byte);
}

void dec16(u16* word) 
{
	(*word)--;
	gb.cycles += 4;
}

void daa() 
{
	int a = gb.r.A;
	
	if (!gb.r.NF) 
	{
		if (gb.r.HF || (a & 0xF) > 9)
			a += 0x06;
		if (gb.r.CF || (a > 0x9F))
			a += 0x60;
	}
	else 
	{
		if (gb.r.HF)
			a = (a - 6) & 0xFF;
		if (gb.r.CF)
			a -= 0x60;
	}

	gb.r.HF = 0;
	gb.r.CF |= (a >= 0x100);
	gb.r.A = (a & 0xFF);
	gb.r.ZF = (gb.r.A == 0);
	a &= 0xFF;
}

void cpl()
{
	gb.r.A = (gb.r.A ^ 0xFF);
	gb.r.NF = 1;
	gb.r.HF = 1;
}

void jp(u16 address, u8 opcode) {
	bool jump = true;
	switch (opcode)	
	{
		case 0xC2: jump = !gb.r.ZF; break;
		case 0xD2: jump = !gb.r.CF; break;
		case 0xCA: jump = gb.r.ZF; break;
		case 0xDA: jump = gb.r.CF; break;
	}
	if (jump) {
		gb.cycles += 4;
		gb.PC = address;
	}
}

void call(u8 opcode) 
{
	bool jump = true;
	switch (opcode) 
	{
		case 0xC4: jump = !gb.r.ZF; break;
		case 0xD4: jump = !gb.r.CF; break;
		case 0xCC: jump = gb.r.ZF; break;
		case 0xDC: jump = gb.r.CF; break;
		case 0xCD: break;
	}
	u16 nn = (read_byte(gb.PC++) | (read_byte(gb.PC++) << 8));
	if (jump) 
	{
		write_byte(--gb.SP, (gb.PC >> 8));
		write_byte(--gb.SP, gb.PC & 0xFF);
		gb.PC = nn;
		gb.cycles += 4;
	}
}

void rst(u8 vec) 
{
	gb.cycles += 4;
	write_byte(--gb.SP, gb.PC >> 8);
	write_byte(--gb.SP, gb.PC & 0xFF);
	gb.PC = vec;
}

void jr(u8 opcode) 
{
	bool jump = true;
	switch (opcode) 
	{
		case 0x20: jump = !gb.r.ZF; break;
		case 0x30: jump = !gb.r.CF; break;
		case 0x28: jump = gb.r.ZF; break;
		case 0x38: jump = gb.r.CF;	break;
	}
	u8 value = read_byte(gb.PC++);
	if (jump) 
	{
		gb.cycles += 4;
		gb.PC += (int8_t)value;
	}
}

void or (u8 value) 
{
	gb.r.A |= value;
	gb.r.ZF = gb.r.A == 0;
	gb.r.NF = 0;
	gb.r.HF = 0;
	gb.r.CF = 0;
}

void cp(u8 value) 
{
	gb.r.ZF = (gb.r.A - value) == 0;
	gb.r.NF = 1;
	gb.r.HF = (gb.r.A & 0x0F) < (value & 0x0F);
	gb.r.CF = (gb.r.A < value);
}

void ret(u8 opcode) 
{
	bool jump = true;
	switch (opcode)	
	{
		case 0xC0:
			jump = !gb.r.ZF;
			gb.cycles += 4;
			break;
		case 0xD0:
			jump = !gb.r.CF;
			gb.cycles += 4;
			break;
		case 0xC8:
			jump = gb.r.ZF;
			gb.cycles += 4;
			break;
		case 0xD8: 
			jump = gb.r.CF;
			gb.cycles += 4;
			break;
		case 0xC9:
			break;
		case 0xD9:
			gb.IME = 1;
			break;
	}
	if (jump) 
	{
		gb.cycles += 4;
		gb.PC = read_word(gb.SP); 
		gb.SP += 2;
	}
}

void scf() 
{
	gb.r.CF = 1;
	gb.r.NF = 0;
	gb.r.HF = 0;
}

void ccf() 
{
	gb.r.NF = 0;
	gb.r.HF = 0;
	gb.r.CF = (gb.r.CF ^ 1);
}

void halt() 
{
	gb.halt = 1;
}

void stop()
{
	gb.stop = 1;
}

// Actual ime enable is delayed one cycle
void ei() 
{
	gb.ime_delay = 1; 
}

void di() 
{
	gb.IME = false;
}

// CB
u8 rlc(u8 byte) 
{
	gb.r.CF = (byte >> 7);
	byte = (byte << 1) + gb.r.CF;
	gb.r.ZF = (byte == 0);
	gb.r.NF = 0;
	gb.r.HF = 0;
	return byte;
}

u8 rrc(u8 byte) 
{
	gb.r.CF = (byte & 0x01);
	byte = (byte >> 1) + (gb.r.CF << 7);
	gb.r.ZF = (byte == 0);
	gb.r.NF = 0;
	gb.r.HF = 0;
	return byte;
}

u8 rl(u8 byte) 
{
	u8 bit = (byte >> 7);
	byte = (byte << 1) + (gb.r.CF);
	gb.r.NF = 0;
	gb.r.HF = 0;
	gb.r.CF = bit;
	gb.r.ZF = (byte == 0);
	return byte;
}

u8 rr(u8 byte) 
{
	u8 bit = (byte & 0x01);
	byte = (byte >> 1) + (gb.r.CF << 7);
	gb.r.NF = 0;
	gb.r.HF = 0;
	gb.r.CF = bit;
	gb.r.ZF = (byte == 0);
	return byte;
}

u8 sla(u8 byte) 
{
	gb.r.CF = (byte >> 7);
	byte <<= 1;
	gb.r.ZF = (byte == 0);
	gb.r.NF = 0;
	gb.r.HF = 0;
	return byte;
}

u8 sra(u8 byte) 
{
	gb.r.CF = (byte & 0x01);
	byte = (byte >> 1) | (byte & 0x80);
	gb.r.ZF = (byte == 0);
	gb.r.NF = 0;
	gb.r.HF = 0;
	return byte;
}

u8 swap(u8 byte) 
{
	u8 temp = byte;
	byte = byte >> 4;
	byte = byte + (temp << 4);
	gb.r.ZF = (byte == 0);
	gb.r.NF = 0;
	gb.r.HF = 0;
	gb.r.CF = 0;
	return byte;
}

u8 srl(u8 byte) 
{
	gb.r.NF = 0;
	gb.r.HF = 0;
	gb.r.CF = (byte) & 0x01;
	byte = (byte >> 1);
	gb.r.ZF = (byte == 0);
	return byte;
}

u8 bit(u8 bit, u8 byte) 
{
	gb.r.ZF = ((byte >> bit) & 0x01) == 0;
	gb.r.NF = 0;
	gb.r.HF = 1;
	return byte;
}

u8 res(u8 bit, u8 byte) 
{
	u8 temp = (1 << bit);
	byte &= ~temp;
	return byte;
}

u8 set(u8 bit, u8 byte) 
{
	u8 temp = (1 << bit);
	byte |= temp;
	return byte;
}