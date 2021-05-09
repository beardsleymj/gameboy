#pragma once
#include "gb.h"
#include "mem.h"

void ld_hl_sp_i8(gameboy*);
void push(gameboy*, uint8_t);
void pop(gameboy*, uint8_t);
void add(gameboy*, uint8_t);
void add16(gameboy*, uint16_t);
void add_sp_i8(gameboy*);
void adc(gameboy*, uint8_t);
void sub(gameboy*, uint8_t);
void sbc(gameboy*, uint8_t);
void rlca(gameboy*);
void rrca(gameboy*);
void rla(gameboy*);
void rra(gameboy*);
void and(gameboy*, uint8_t);
void xor(gameboy*, uint8_t);
void inc_8(gameboy*, uint8_t);
void inc_16(gameboy*, uint8_t);
void dec(gameboy*, uint8_t*);
void dec_hl(gameboy*);
void daa(gameboy*);
void cpl(gameboy*);
void jp(gameboy*, uint16_t, uint8_t);
void call(gameboy*, uint8_t);
void jp_nn(gameboy*);
void jr(gameboy*, uint8_t);
void or(gameboy*, uint8_t);
void cp(gameboy*, uint8_t);
void ret(gameboy*, uint8_t);
void scf(gameboy*);
void ccf(gameboy*);
void halt(gameboy*);
void ei(gameboy*);
void di(gameboy*);

uint8_t rlc(gameboy*, uint8_t);
uint8_t rrc(gameboy*, uint8_t);
uint8_t rl(gameboy*, uint8_t);
uint8_t rr(gameboy*, uint8_t);
uint8_t sla(gameboy*, uint8_t);
uint8_t sra(gameboy*, uint8_t);
uint8_t swap(gameboy*, uint8_t);
uint8_t srl(gameboy*, uint8_t);
uint8_t bit(gameboy*, uint8_t, uint8_t);