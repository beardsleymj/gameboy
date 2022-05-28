#include "cpu_instr.h"
#include "cpu.h"
#include "gb_system.h"
#include "bus.h"

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

void ld_a16_sp()
{
	u16 address = read_byte(cpu.PC++) | (read_byte(cpu.PC++) << 8);
	write_byte(address, cpu.SP & 0xFF);
	write_byte(address + 1, cpu.SP >> 8);
}

void push(u16* data)
{
	gb.cycles += 4;
	write_byte(--cpu.SP, (*data) >> 8);
	write_byte(--cpu.SP, (*data) & 0xFF);
}

void pop(u16* data)
{
	*data = (read_byte(cpu.SP++) & 0xFF) | (read_byte(cpu.SP++) << 8);
}

void pop_af()
{
	pop(&cpu.r.AF);
	cpu.r.AF = cpu.r.AF & 0xFFF0;
}

void add(u8 value) 
{
	cpu.r.NF = 0;
	cpu.r.HF = (cpu.r.A & 0x0F) + (value & 0x0F) > 0x0F;
	cpu.r.CF = cpu.r.A > 0xFF - value;
	cpu.r.A += value;
	cpu.r.ZF = cpu.r.A == 0;
}

void add_16(u16 value) 
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

void inc_8(u8* data)
{
	(*data)++;
	cpu.r.ZF = *data == 0;
	cpu.r.NF = 0;
	cpu.r.HF = (*data & 0x0F) == 0x00;
}

void inc_hl()
{
	u8 data = read_byte(cpu.r.HL);
	inc_8(&data);
	write_byte(cpu.r.HL, data);
}

void inc_16(u16* data) 
{
	gb.cycles += 4;
	(*data)++;
}

void dec(u8* data) 
{
	cpu.r.HF = (*data & 0x0F) == 0;
	*data = *data - 1;
	cpu.r.ZF = (*data == 0);
	cpu.r.NF = 1;
}

void dec_hl() 
{
	u8 data = read_byte(cpu.r.HL);
	cpu.r.HF = (data & 0x0F) == 0;
	data = data - 1;
	cpu.r.ZF = (data == 0);
	cpu.r.NF = 1;
	write_byte(cpu.r.HL, data);
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

void jp(bool jump) 
{
	u16 address = read_byte(cpu.PC++) + (read_byte(cpu.PC++) << 8);

	if (jump) 
	{
		gb.cycles += 4;
		cpu.PC = address;
	}
}

void call(bool jump) 
{
	u16 nn = read_byte(cpu.PC++) | (read_byte(cpu.PC++) << 8);

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

void jr(bool jump) 
{
	s8 data = read_byte(cpu.PC++);
	
	if (jump) 
	{
		gb.cycles += 4;
		cpu.PC += data;
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

void ret_f(bool jump)
{
	gb.cycles += 4;
	ret(jump);
}

void ret(bool jump) 
{
	if (jump) 
	{
		gb.cycles += 8;
		cpu.PC = read_word(cpu.SP); 
		cpu.SP += 2;
	}
}

void reti()
{
	ret(true);
	gb.IME = 1;
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
	printf("stop \n");
	if (cpu.prepare_speed_switch)
	{
		printf("double speed switch\n");
		cpu.prepare_speed_switch = 0;
		cpu.double_speed = 1;
	}
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
u8 rlc(u8 data) 
{
	cpu.r.CF = (data >> 7);
	data = (data << 1) + cpu.r.CF;
	cpu.r.ZF = (data == 0);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	return data;
}

u8 rrc(u8 data) 
{
	cpu.r.CF = (data & 0x01);
	data = (data >> 1) + (cpu.r.CF << 7);
	cpu.r.ZF = (data == 0);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	return data;
}

u8 rl(u8 data) 
{
	u8 bit = (data >> 7);
	data = (data << 1) + (cpu.r.CF);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	cpu.r.CF = bit;
	cpu.r.ZF = (data == 0);
	return data;
}

u8 rr(u8 data) 
{
	u8 bit = (data & 0x01);
	data = (data >> 1) + (cpu.r.CF << 7);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	cpu.r.CF = bit;
	cpu.r.ZF = (data == 0);
	return data;
}

u8 sla(u8 data) 
{
	cpu.r.CF = (data >> 7);
	data <<= 1;
	cpu.r.ZF = (data == 0);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	return data;
}

u8 sra(u8 data) 
{
	cpu.r.CF = (data & 0x01);
	data = (data >> 1) | (data & 0x80);
	cpu.r.ZF = (data == 0);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	return data;
}

u8 swap(u8 data) 
{
	u8 temp = data;
	data = data >> 4;
	data = data + (temp << 4);
	cpu.r.ZF = (data == 0);
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	cpu.r.CF = 0;
	return data;
}

u8 srl(u8 data) 
{
	cpu.r.NF = 0;
	cpu.r.HF = 0;
	cpu.r.CF = (data) & 0x01;
	data = (data >> 1);
	cpu.r.ZF = (data == 0);
	return data;
}

u8 bit(u8 bit, u8 data) 
{
	cpu.r.ZF = ((data >> bit) & 0x01) == 0;
	cpu.r.NF = 0;
	cpu.r.HF = 1;
	return data;
}

u8 res(u8 bit, u8 data) 
{
	u8 temp = (1 << bit);
	data &= ~temp;
	return data;
}

u8 set(u8 bit, u8 data) 
{
	u8 temp = (1 << bit);
	data |= temp;
	return data;
}