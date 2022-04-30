#pragma once
#include "types.h"

void ld_hl_sp_i8();
void ld_a16_sp();
void push(u16*);
void pop(u16* data);
void pop_af();
void add(u8);
void add_16(u16);
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
void inc_8(u8*);
void inc_hl();
void inc_16(u16* data);
void dec(u8*);
void dec_hl();
void dec16(u16*);
void daa();
void cpl();
void call(bool);
void jp(bool);
void jr(bool);
void or(u8);
void cp(u8);
void ret_f(bool);
void ret(bool);
void reti();
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