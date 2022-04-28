#pragma once

#include "types.h"

#define NR10 0xFF10
#define NR11 0xFF11
#define NR12 0XFF12
#define NR13 0XFF13
#define NR14 0xFF14
#define NR21 0xFF16
#define NR22 0xFF17
#define NR23 0xFF18
#define NR24 0xFF19
#define NR30 0xFF1A
#define NR31 0xFF1B
#define NR32 0xFF1C
#define NR33 0xFF1D
#define NR34 0xFF1E

#define NR41 0xFF20
#define NR42 0xFF21
#define NR43 0xFF22
#define NR44 0xFF23

#define NR50 0xFF24
#define NR51 0xFF25
#define NR52 0xFF26

#define LCDC 0xFF40
#define STAT 0xFF41
#define SCY 0xFF42
#define SCX 0xFF43
#define LY 0xFF44
#define LYC 0xFF45
#define WY 0xFF4A
#define WX 0xFF4B
#define BGP 0xFF47
#define OBP0 0xFF48
#define OBP1 0xFF49

#define INTERRUPT_ENABLE 0xFFFF
#define INTERRUPT_FLAG 0xFF0F

#define SB 0xFF01
#define SC 0xFF02
#define DIV 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TAC 0xFF07
#define JOYP 0xFF00
#define DMA 0xFF46
#define OAM_START 0xFE00
#define OAM_END 0xFE9F

u8 read_byte(u16 address);
u8 read_io(u16 address);
u16 read_word(u16 address);
void write_byte(u16 address, u8 value);
void write_io(u16 address, u8 value);
void dma_transfer();
u8 read_joyp();

