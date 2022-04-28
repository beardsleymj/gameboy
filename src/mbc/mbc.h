#pragma once
#include "../types.h"

u8 mbc1_read(u16 address);
void mbc1_write(u16 address, u8 value);

u8 mbc2_read(u16 address);
void mbc2_write(u16 address, u8 value);

u8 mbc3_read(u16 address);
void mbc3_write(u16 address, u8 value);

u8 mbc5_read(u16 address);
void mbc5_write(u16 address, u8 value);
