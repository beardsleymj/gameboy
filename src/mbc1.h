#include "gb.h"

uint8_t mbc1_read_byte(gameboy* gb, uint16_t address);
void mbc1_write_byte(gameboy* gb, uint16_t address, uint8_t value);
