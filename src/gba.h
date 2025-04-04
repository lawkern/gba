#pragma once

/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define REG_DISPCNT  *((volatile u16 *)0x4000000)
#define REG_VCOUNT   *((volatile u16 *)0x4000006)

enum
{
   MODE0 = 0x0000,
   MODE1 = 0x0001,
   MODE2 = 0x0002,
   MODE3 = 0x0003,
   MODE4 = 0x0004,
   MODE5 = 0x0005,

   BG0 = 0x0100,
   BG1 = 0x0200,
   BG2 = 0x0400,
   BG3 = 0x0800,
   OBJ = 0x1000,
};

#define VRAM ((volatile u16 *)0x06000000)
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 160

#define RGB(r, g, b) (((r) << 0) | ((g) << 5) | ((b) << 10))
#define BLACK   0x0000
#define RED     0x001F
#define GREEN   0x03E0
#define YELLOW  0x03FF
#define BLUE    0x7C00
#define MAGENTA 0x7C1F
#define CYAN    0x7FE0
#define WHITE   0x7FFF
