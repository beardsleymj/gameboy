#pragma once
#include "gb.h"

uint64_t get_time_nano();
uint64_t get_time_nano();
void sync_timing(gameboy*);
void reset_div(gameboy*);
void sync_div(gameboy*);
void write_tac(gameboy*, uint8_t);
void sync_tima(gameboy*);
