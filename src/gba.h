#pragma once

/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define REG_DISPCNT  *((volatile u16 *)0x4000000)
#define REG_DISPSTAT *((volatile u16 *)0x4000004)
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

#define REG_KEYINPUT *((volatile u16 *)0x4000130)
#define REG_KEYCNT   *((volatile u16 *)0x4000132)

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

inline void gba_process_input(gba_input *input)
{
   u16 button_mask = 0x03FF;
   input->previous = input->current;
   input->current = ~REG_KEYINPUT & button_mask;
}

// TODO: These could be made "faster" by not passing in the input struct and
// replacing the boolean logic with bit manipulations. Check back later to see
// if this ever matters in practice.

inline u32 is_held(gba_input input, gba_button button)
{
   // The button is currently down.
   return(input.current & button);
}
inline u32 was_pressed(gba_input input, gba_button button)
{
   // The button was pressed this frame.
   return(!(input.previous & button) && (input.current & button));
}
inline u32 was_released(gba_input input, gba_button button)
{
   // The button was released this frame.
   return((input.previous & button) && !(input.current & button));
}

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
