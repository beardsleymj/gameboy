#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "gb.h"

void init_memory(gameboy*, FILE*);
uint8_t read_byte(gameboy* gb, uint16_t address);
uint16_t read_word(gameboy* gb, uint16_t address);
void write_byte(gameboy* gb, uint16_t address, uint8_t value);
void dma_transfer(gameboy* gb);


