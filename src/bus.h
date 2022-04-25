#pragma once
#include "gb.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

u8 read_byte(u16 address);
u8 read_io(u16 address);
u16 read_word(u16 address);
void write_byte(u16 address, u8 value);
void write_io(u16 address, u8 value);
void dma_transfer();
u8 read_joyp();

