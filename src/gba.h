#pragma once

/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

#include <stdint.h>
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define countof(array) (sizeof(array) / sizeof((array)[0]))
#define align(n) __attribute__((aligned(n)))

// Memory map base addresses:
#define BIOS_ADDRESS    0x00000000
#define EWRAM_ADDRESS   0x02000000
#define IWRAM_ADDRESS   0x03000000
#define IO_ADDRESS      0x04000000
#define PALETTE_ADDRESS 0x05000000
#define VRAM_ADDRESS    0x06000000
#define OAM_ADDRESS     0x07000000

// Video registers:
#define REGISTER_DISPCNT  *(volatile u32 *)(IO_ADDRESS + 0x00)
#define REGISTER_DISPSTAT *(volatile u16 *)(IO_ADDRESS + 0x04)
#define REGISTER_VCOUNT   *(volatile u16 *)(IO_ADDRESS + 0x06)

// Input registers:
#define REGISTER_KEYINPUT *(volatile u16 *)(IO_ADDRESS + 0x0130)
#define REGISTER_KEYCNT   *(volatile u16 *)(IO_ADDRESS + 0x0132)

// Palette addresses:
#define PALETTE_BG  (PALETTE_ADDRESS + 0x0000)
#define PALETTE_OBJ (PALETTE_ADDRESS + 0x0200)

// VRAM addresses:
#define VRAM_BG  (VRAM_ADDRESS + 0x0A000)
#define VRAM_OBJ (VRAM_ADDRESS + 0x10000)

// Bit flag manipulations:
#define FLAG_PREP(name, value) (((value) << name##_SHIFT) & name##_MASK)
#define FLAG_GET(name, value) (((value) & name##_MASK) >> name##_SHIFT)
#define FLAG_SET(name, dest, value) (dest = ((dest) & ~name##_MASK) | FLAG_PREP(name, value))

#define FLAG_PREP_NOSHIFT(name, value) ((value) & name##_MASK)
#define FLAG_GET_NOSHIFT(name, value) ((value) & name##_MASK)
#define FLAG_SET_NOSHIFT(name, dest, value) (dest = ((dest) & ~name##_MASK) | FLAG_PREP_NOSHIFT(name, value))

// DISPCNT register flags:
#define DISPCNT_MODE0     0x0000
#define DISPCNT_MODE1     0x0001
#define DISPCNT_MODE2     0x0002
#define DISPCNT_MODE3     0x0003
#define DISPCNT_MODE4     0x0004
#define DISPCNT_MODE5     0x0005
#define DISPCNT_GAMEBOY   0x0008
#define DISPCNT_PAGE      0x0010
#define DISPCNT_OAMHBLANK 0x0020
#define DISPCNT_OBJ1D     0x0040
#define DISPCNT_OBJ2D     0x0000
#define DISPCNT_BLANK     0x0080
#define DISPCNT_BG0       0x0100
#define DISPCNT_BG1       0x0200
#define DISPCNT_BG2       0x0400
#define DISPCNT_BG3       0x0800
#define DISPCNT_OBJ       0x1000
#define DISPCNT_WINDOW0   0x2000
#define DISPCNT_WINDOW1   0x4000
#define DISPCNT_WINDOWOBJ 0x8000

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 160

// 15-bit colors:
#define RGB(r, g, b) (((r) << 0) | ((g) << 5) | ((b) << 10))
#define BLACK   0x0000
#define RED     0x001F
#define GREEN   0x03E0
#define YELLOW  0x03FF
#define BLUE    0x7C00
#define MAGENTA 0x7C1F
#define CYAN    0x7FE0
#define WHITE   0x7FFF

// Input:
typedef enum {
   BUTTON_A      = 0x0001,
   BUTTON_B      = 0x0002,
   BUTTON_SELECT = 0x0004,
   BUTTON_START  = 0x0008,
   BUTTON_RIGHT  = 0x0010,
   BUTTON_LEFT   = 0x0020,
   BUTTON_UP     = 0x0040,
   BUTTON_DOWN   = 0x0080,
   BUTTON_R      = 0x0100,
   BUTTON_L      = 0x0200,
} gba_button;

typedef struct {
   u16 current;
   u16 previous;
} gba_input;

// TODO: These could be made "faster" by not passing in the input struct and
// replacing the boolean logic with bit manipulations. Check back later to see
// if this ever matters in practice.

static inline _Bool is_held(gba_input input, gba_button button)
{
   // The button is currently down.
   return(input.current & button);
}
static inline _Bool was_pressed(gba_input input, gba_button button)
{
   // The button was pressed this frame.
   return(!(input.previous & button) && (input.current & button));
}
static inline _Bool was_released(gba_input input, gba_button button)
{
   // The button was released this frame.
   return((input.previous & button) && !(input.current & button));
}

// Tiles types:
typedef struct {
   u32 data[8];
} tile4;

typedef struct {
   u32 data[16];
} tile8;

typedef tile4 charblock4[512];
typedef tile8 charblock8[256];

#define TILE4_BLOCKS ((charblock4 *)VRAM_ADDRESS)
#define TILE8_BLOCKS ((charblock8 *)VRAM_ADDRESS)

// Object attributes:
typedef struct {
   u16 attribute0;
   u16 attribute1;
   u16 attribute2;
   s16 padding;
} align(4) gba_object;

typedef struct {
   u16 padding0[3];
   s16 pa;
   u16 padding1[3];
   s16 pb;
   u16 padding2[3];
   s16 pc;
   u16 padding3[3];
   s16 pd;
} align(4) gba_affine;

// Object attribute 0:
#define ATTR0_REGULAR      0x0000
#define ATTR0_AFFINE       0x0100
#define ATTR0_HIDE         0x0200
#define ATTR0_AFFINEDOUBLE 0x0300
#define ATTR0_4BPP         0x0000
#define ATTR0_8BPP         0x2000
#define ATTR0_SQUARE       0x0000
#define ATTR0_WIDE         0x4000
#define ATTR0_TALL         0x8000

#define ATTR0_Y_MASK 0x00FF
#define ATTR0_Y_SHIFT 0
#define ATTR0_Y(value) ((value) << ATTR0_Y_SHIFT)

#define ATTR0_OBJECTMODE_MASK 0x0300
#define ATTR0_OBJECTMODE_SHIFT 8
#define ATTR0_OBJECTMODE(value) ((value) << ATTR0_OBJECTMODE_SHIFT)

#define ATTR0_GFXMODE_MASK 0x0C00
#define ATTR0_GFXMODE_SHIFT 10
#define ATTR0_GFXMODE(value) ((value) << ATTR0_GFXMODE_SHIFT)

#define ATTR0_MOSAIC_MASK 0x1000
#define ATTR0_MOSAIC_SHIFT 12
#define ATTR0_MOSAIC(value) ((value) << ATTR0_MOSAIC_SHIFT)

#define ATTR0_COLORMODE_MASK 0x2000
#define ATTR0_COLORMODE_SHIFT 13
#define ATTR0_COLORMODE(value) ((value) << ATTR0_COLORMODE_SHIFT)

#define ATTR0_SHAPE_MASK 0xC000
#define ATTR0_SHAPE_SHIFT 14
#define ATTR0_SHAPE(value) ((value) << ATTR0_SHAPE_SHIFT)

// Object attribute 1:
#define ATTR1_HFLIP         0x1000
#define ATTR1_VFLIP         0x2000
#define ATTR1_SIZE_8             0 // Base
#define ATTR1_SIZE_16       0x4000 // Base
#define ATTR1_SIZE_32       0x8000 // Base
#define ATTR1_SIZE_64       0xC000 // Base
#define ATTR1_SIZE_8x8           0 // Square
#define ATTR1_SIZE_16x16    0x4000 // Square
#define ATTR1_SIZE_32x32    0x8000 // Square
#define ATTR1_SIZE_64x64    0xC000 // Square
#define ATTR1_SIZE_8x16          0 // Tall
#define ATTR1_SIZE_8x32     0x4000 // Tall
#define ATTR1_SIZE_16x32    0x8000 // Tall
#define ATTR1_SIZE_32x64    0xC000 // Tall
#define ATTR1_SIZE_16x8          0 // Wide
#define ATTR1_SIZE_32x8     0x4000 // Wide
#define ATTR1_SIZE_32x16    0x8000 // Wide
#define ATTR1_SIZE_64x32    0xC000 // Wide

#define ATTR1_X_MASK 0x01FF
#define ATTR1_X_SHIFT 0
#define ATTR1_X(value) ((value) << ATTR1_X_SHIFT)

#define ATTR1_AFFINEINDEX_MASK 0x3E00
#define ATTR1_AFFINEINDEX_SHIFT 9
#define ATTR1_AFFINEINDEX(value) ((value) << ATTR1_AFFINEINDEX_SHIFT)

#define ATTR1_HORZFLIP_MASK 0x1000
#define ATTR1_HORZFLIP_SHIFT 12
#define ATTR1_HORZFLIP(value) ((value) << ATTR1_HORZFLIP_SHIFT)

#define ATTR1_VERTFLIP_MASK 0x2000
#define ATTR1_VERTFLIP_SHIFT 13
#define ATTR1_VERTFLIP(value) ((value) << ATTR1_VERTFLIP_SHIFT)

#define ATTR1_SIZE_MASK 0xC000
#define ATTR1_SIZE_SHIFT 14
#define ATTR1_SIZE(value) ((value) << ATTR1_SIZE_SHIFT)

// Object attribute 2:
#define ATTR2_INDEX_MASK 0x03FF
#define ATTR2_INDEX_SHIFT 0
#define ATTR2_INDEX(value) ((value) << ATTR2_INDEX_SHIFT)

#define ATTR2_PRIORITY_MASK 0x0C00
#define ATTR2_PRIORITY_SHIFT 10
#define ATTR2_PRIORITY(value) ((value) << ATTR2_PRIORITY_SHIFT)

#define ATTR2_PALETTEBANK_MASK 0x0C00
#define ATTR2_PALETTEBANK_SHIFT 10
#define ATTR2_PALETTEBANK(value) ((value) << ATTR2_PALETTEBANK_SHIFT)
