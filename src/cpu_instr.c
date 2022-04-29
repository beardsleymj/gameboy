#include "cpu_instr.h"
#include "cpu.h"
#include "gb_system.h"

void ld_hl_sp_i8() 
{
	int8_t offset = read_byte(cpu.PC++);
	cpu.r.HF = (cpu.SP & 0xF) + (offset & 0xF) > 0xF;
	cpu.r.CF = (cpu.SP & 0xFF) + (offset & 0xFF) > 0xFF;
	cpu.r.ZF = 0;
	cpu.r.NF = 0;
	cpu.r.HL = cpu.SP + offset;
	gb.cycles += 4;
}

void push(u8 opcode) 
{
	gb.cycles += 4;
	switch (opcode)	
	{
		case 0xC5:
			write_byte(--cpu.SP, cpu.r.B);
			write_byte(--cpu.SP, cpu.r.C);
			break;
		case 0xD5:
			write_byte(--cpu.SP, cpu.r.D);
			write_byte(--cpu.SP, cpu.r.E);
			break;
		case 0xE5:
			write_byte(--cpu.SP, cpu.r.H);
			write_byte(--cpu.SP, cpu.r.L);
			break;
		case 0xF5:
			write_byte(--cpu.SP, cpu.r.A);
			write_byte(--cpu.SP, cpu.r.F);
			break;
	}
}

void pop(u8 opcode) 
{
	switch (opcode)	
	{
		case 0xC1:
			cpu.r.C = read_byte(cpu.SP++);
			cpu.r.B = read_byte(cpu.SP++);
			break;
		case 0xD1:
			cpu.r.E = read_byte(cpu.SP++);
			cpu.r.D = read_byte(cpu.SP++);
			break;
		case 0xE1:
			cpu.r.L = read_byte(cpu.SP++);
			cpu.r.H = read_byte(cpu.SP++);
			break;
		case 0xF1:
			cpu.r.F = read_byte(cpu.SP++);
			cpu.r.A = read_byte(cpu.SP++);
			cpu.r.F = (cpu.r.F & 0xF0);
			break;
	}
}

void add(u8 value) 
{
	cpu.r.NF = 0;
	cpu.r.HF = (cpu.r.A & 0x0F) + (value & 0x0F) > 0x0F;
	cpu.r.CF = cpu.r.A > 0xFF - value;
	cpu.r.A += value;
	cpu.r.ZF = cpu.r.A == 0;
}

void add16(u16 value) 
{
	cpu.r.NF = 0;
	cpu.r.HF = (cpu.r.HL & 0x0FFF) + (value & 0x0FFF) > 0x0FFF;
	cpu.r.CF = cpu.r.HL > 0xFFFF - value;
	cpu.r.HL += value;
	gb.cycles += 4;
}

void add_sp_i8() 
{
	int8_t offset = read_byte(cpu.PC++);
	cpu.r.HF = (cpu.SP & 0xF) + (offset & 0xF) > 0xF;
	cpu.r.CF = (cpu.SP & 0xFF) + (offset & 0xFF) > 0xFF;
	cpu.r.ZF = 0;
	cpu.r.NF = 0;
	cpu.SP += offset;
	gb.cycles += 8;
}

void adc(u8 value) 
{
	u8 CF_old = cpu.r.CF;
	cpu.r.NF = 0;
	cpu.r.HF = (cpu.r.A & 0x0F) + (value & 0x0F) + CF_old > 0x0F;
	cpu.r.CF = (u16)cpu.r.A + value + CF_old> 0xFF;
	cpu.r.A += value + CF_old;
	cpu.r.ZF = (cpu.r.A == 0);
}

void sub(u8 value) 
{
	cpu.r.NF = 1;
	cpu.r.HF = (cpu.r.A & 0x0F) < (value & 0x0F);
	cpu.r.CF = cpu.r.A < value;
	cpu.r.A -= value;
	cpu.r.ZF = cpu.r.A == 0;
}

void sbc(u8 value) 
{
	bool carry_old = cpu.r.CF;
	cpu.r.NF = 1;
	cpu.r.HF = (cpu.r.A & 0x0F) < (u16)(value & 0x0F) + cpu.r.CF;
	cpu.r.CF = cpu.r.A < (u16)value + cpu.r.CF;
	cpu.r.A = (cpu.r.A - value) - carry_old;
	cpu.r.ZF = cpu.r.A == 0;
}

void rlca() 
{
	cpu.r.CF = (cpu.r.A >> 7);
	cpu.r.A = (cpu.r.A << 1) + cpu.r.CF;
	cpu.r.ZF = 0;
	cpu.r.NF = 0;
	cpu.r.HF = 0;
}

void rrca() 
{
	cpu.r.CF = (cpu.r.A & 0x01);
	cpu.r.A = (cpu.r.A >> 1) + (cpu.r.CF << 7);
	cpu.r.ZF = 0;
	cpu.r.NF = 0;
	cpu.r.HF = 0;
}

void rla() 
{
	u8 bit = (cpu.r.A >> 7);
	cpu.r.A = (cpu.r.A << 1) + (cpu.r.CF);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	cpu.r.CF = bit;
	cpu.r.ZF = 0;
}

void rra() 
{
	u8 bit = (cpu.r.A & 0x01);
	cpu.r.A = (cpu.r.A >> 1) + (cpu.r.CF * 0x80);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	cpu.r.CF = bit;
	cpu.r.ZF = 0;
}

void and (u8 value) 
{
	cpu.r.A &= value; 
	cpu.r.ZF = cpu.r.A == 0;
	cpu.r.NF = 0;
	cpu.r.HF = 1;
	cpu.r.CF = 0;
}

void xor(u8 value) 
{
	cpu.r.A ^= value;
	cpu.r.ZF = cpu.r.A == 0;
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	cpu.r.CF = 0;
}

void inc_8(u8 opcode) 
{
	u8 value = 0;

	switch (opcode)	{
		case 0x04:
			value = ++cpu.r.B;
			break;
		case 0x14:
			value = ++cpu.r.D;
			break;
		case 0x24:
			value = ++cpu.r.H;
			break;
		case 0x34:
			write_byte(cpu.r.HL, read_byte(cpu.r.HL) + 1);
			value = read_byte(cpu.r.HL);
			break;
		case 0x0C:
			value = ++cpu.r.C;
			break;
		case 0x1C:
			value = ++cpu.r.E;
			break;
		case 0x2C:
			value = ++cpu.r.L;
			break;
		case 0x3C:
			value = ++cpu.r.A;
			break;
	}
	cpu.r.ZF = value == 0;
	cpu.r.NF = 0;
	cpu.r.HF = (value & 0x0F) == 0x00; // 0bXXXX0000
}

void inc_16(u8 opcode) 
{
	switch (opcode)	
	{
		case 0x03: cpu.r.BC++; break;
		case 0x13: cpu.r.DE++; break;
		case 0x23: cpu.r.HL++; break;
		case 0x33: cpu.SP++; break;
	}
	gb.cycles += 4;
}

void dec(u8* byte) 
{
	cpu.r.HF = (*byte & 0x0F) == 0;
	*byte = *byte - 1;
	cpu.r.ZF = (*byte == 0);
	cpu.r.NF = 1;
}

void dec_hl() 
{
	u8 byte = read_byte(cpu.r.HL);
	cpu.r.HF = (byte & 0x0F) == 0;
	byte = byte - 1;
	cpu.r.ZF = (byte == 0);
	cpu.r.NF = 1;
	write_byte(cpu.r.HL, byte);
}

void dec16(u16* word) 
{
	(*word)--;
	gb.cycles += 4;
}

void daa() 
{
	int a = cpu.r.A;
	
	if (!cpu.r.NF) 
	{
		if (cpu.r.HF || (a & 0xF) > 9)
			a += 0x06;
		if (cpu.r.CF || (a > 0x9F))
			a += 0x60;
	}
	else 
	{
		if (cpu.r.HF)
			a = (a - 6) & 0xFF;
		if (cpu.r.CF)
			a -= 0x60;
	}

	cpu.r.HF = 0;
	cpu.r.CF |= (a >= 0x100);
	cpu.r.A = (a & 0xFF);
	cpu.r.ZF = (cpu.r.A == 0);
	a &= 0xFF;
}

void cpl()
{
	cpu.r.A = (cpu.r.A ^ 0xFF);
	cpu.r.NF = 1;
	cpu.r.HF = 1;
}

void jp(u16 address, u8 opcode) {
	bool jump = true;
	switch (opcode)	
	{
		case 0xC2: jump = !cpu.r.ZF; break;
		case 0xD2: jump = !cpu.r.CF; break;
		case 0xCA: jump = cpu.r.ZF; break;
		case 0xDA: jump = cpu.r.CF; break;
	}
	if (jump) {
		gb.cycles += 4;
		cpu.PC = address;
	}
}

void call(u8 opcode) 
{
	bool jump = true;
	switch (opcode) 
	{
		case 0xC4: jump = !cpu.r.ZF; break;
		case 0xD4: jump = !cpu.r.CF; break;
		case 0xCC: jump = cpu.r.ZF; break;
		case 0xDC: jump = cpu.r.CF; break;
		case 0xCD: break;
	}
	u16 nn = (read_byte(cpu.PC++) | (read_byte(cpu.PC++) << 8));
	if (jump) 
	{
		write_byte(--cpu.SP, (cpu.PC >> 8));
		write_byte(--cpu.SP, cpu.PC & 0xFF);
		cpu.PC = nn;
		gb.cycles += 4;
	}
}

void rst(u8 vec) 
{
	gb.cycles += 4;
	write_byte(--cpu.SP, cpu.PC >> 8);
	write_byte(--cpu.SP, cpu.PC & 0xFF);
	cpu.PC = vec;
}

void jr(u8 opcode) 
{
	bool jump = true;
	switch (opcode) 
	{
		case 0x20: jump = !cpu.r.ZF; break;
		case 0x30: jump = !cpu.r.CF; break;
		case 0x28: jump = cpu.r.ZF; break;
		case 0x38: jump = cpu.r.CF;	break;
	}
	u8 value = read_byte(cpu.PC++);
	if (jump) 
	{
		gb.cycles += 4;
		cpu.PC += (int8_t)value;
	}
}

void or (u8 value) 
{
	cpu.r.A |= value;
	cpu.r.ZF = cpu.r.A == 0;
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	cpu.r.CF = 0;
}

void cp(u8 value) 
{
	cpu.r.ZF = (cpu.r.A - value) == 0;
	cpu.r.NF = 1;
	cpu.r.HF = (cpu.r.A & 0x0F) < (value & 0x0F);
	cpu.r.CF = (cpu.r.A < value);
}

void ret(u8 opcode) 
{
	bool jump = true;
	switch (opcode)	
	{
		case 0xC0:
			jump = !cpu.r.ZF;
			gb.cycles += 4;
			break;
		case 0xD0:
			jump = !cpu.r.CF;
			gb.cycles += 4;
			break;
		case 0xC8:
			jump = cpu.r.ZF;
			gb.cycles += 4;
			break;
		case 0xD8: 
			jump = cpu.r.CF;
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
		cpu.PC = read_word(cpu.SP); 
		cpu.SP += 2;
	}
}

void scf() 
{
	cpu.r.CF = 1;
	cpu.r.NF = 0;
	cpu.r.HF = 0;
}

void ccf() 
{
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	cpu.r.CF = (cpu.r.CF ^ 1);
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
	cpu.r.CF = (byte >> 7);
	byte = (byte << 1) + cpu.r.CF;
	cpu.r.ZF = (byte == 0);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	return byte;
}

u8 rrc(u8 byte) 
{
	cpu.r.CF = (byte & 0x01);
	byte = (byte >> 1) + (cpu.r.CF << 7);
	cpu.r.ZF = (byte == 0);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	return byte;
}

u8 rl(u8 byte) 
{
	u8 bit = (byte >> 7);
	byte = (byte << 1) + (cpu.r.CF);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	cpu.r.CF = bit;
	cpu.r.ZF = (byte == 0);
	return byte;
}

u8 rr(u8 byte) 
{
	u8 bit = (byte & 0x01);
	byte = (byte >> 1) + (cpu.r.CF << 7);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	cpu.r.CF = bit;
	cpu.r.ZF = (byte == 0);
	return byte;
}

u8 sla(u8 byte) 
{
	cpu.r.CF = (byte >> 7);
	byte <<= 1;
	cpu.r.ZF = (byte == 0);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	return byte;
}

u8 sra(u8 byte) 
{
	cpu.r.CF = (byte & 0x01);
	byte = (byte >> 1) | (byte & 0x80);
	cpu.r.ZF = (byte == 0);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	return byte;
}

u8 swap(u8 byte) 
{
	u8 temp = byte;
	byte = byte >> 4;
	byte = byte + (temp << 4);
	cpu.r.ZF = (byte == 0);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	cpu.r.CF = 0;
	return byte;
}

u8 srl(u8 byte) 
{
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	cpu.r.CF = (byte) & 0x01;
	byte = (byte >> 1);
	cpu.r.ZF = (byte == 0);
	return byte;
}

u8 bit(u8 bit, u8 byte) 
{
	cpu.r.ZF = ((byte >> bit) & 0x01) == 0;
	cpu.r.NF = 0;
	cpu.r.HF = 1;
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