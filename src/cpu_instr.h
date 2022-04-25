#pragma once
#include "gb.h"

void ld_hl_sp_i8();
void push(u8);
void pop(u8);
void add(u8);
void add16(u16);
void add_sp_i8();
void adc(u8);
void sub(u8);
void sbc(u8);
void rlca();
void rrca();
void rla();
void rra();
void and(u8);
void xor(u8);
void inc_8(u8);
void inc_16(u8);
void dec(u8*);
void dec_hl();
void daa();
void cpl();
void jp(u16, u8);
void call(u8);
void jr(u8);
void or(u8);
void cp(u8);
void ret(u8);
void scf();
void ccf();
void halt();
void ei();
void di();

u8 rlc(u8);
u8 rrc(u8);
u8 rl(u8);
u8 rr(u8);
u8 sla(u8);
u8 sra(u8);
u8 swap(u8);
u8 srl(u8);
u8 bit(u8, u8);