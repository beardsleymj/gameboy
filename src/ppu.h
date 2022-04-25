#pragma once
#include "gb.h"

void ppu_run();
void hblank();
void vblank();
void oam_search();
int sprite_compare(sprite*, sprite*);
void draw_scanline();
void increment_ly();
void draw_bg(u8* scanline);
void draw_obj(u8* scanline);
void fill_buffer(u8* scanline);
u8 get_obj_pixel_color(sprite sprt);
