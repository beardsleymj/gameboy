#pragma once

#include "types.h"

void sync_timing();
void write_div();
void sync_div();
void write_tac(u8);
void sync_tima();
u8 read_div();
void sync_timing();
u8	read_tima();
void write_tima(u8 value);

