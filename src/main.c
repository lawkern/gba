/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

#include <string.h>

#include "gba.h"
#include "tiles.c"

static _Bool gba_begin_frame(gba_input *input)
{
   // Wait for vertical blank.
   while(REGISTER_VCOUNT >= SCREEN_HEIGHT) {}
   while(REGISTER_VCOUNT < SCREEN_HEIGHT) {}

   // Capture the current input state to use for this frame.
   u16 button_mask = 0x03FF;
   input->previous = input->current;
   input->current = ~REGISTER_KEYINPUT & button_mask;

   _Bool poweroff_requested =
      is_held(*input, BUTTON_A) &&
      is_held(*input, BUTTON_B) &&
      is_held(*input, BUTTON_L) &&
      is_held(*input, BUTTON_R);

   return(!poweroff_requested);
}

static void oam_copy(gba_object *dst, gba_object *src, int count)
{
   // TODO: I'm using a 32-bit word copy instead of a struct copy below due to
   // what I assume is a bug in GCC. The struct copy fails inside
   // oam_initialize, but only when oam_initialize is marked as a static
   // function. It's possibly doing unsafe 8-bit writes to OAM? Check the
   // disassembly later to see what's happening here.

   // while(count--) *dst++ = *src++;

   u32 *dst32 = (u32 *)dst;
   u32 *src32 = (u32 *)src;
   while(count--)
   {
      *dst32++ = *src32++;
      *dst32++ = *src32++;
   }
}

static void oam_initialize(gba_object *objects, int count)
{
   u32 *dst = (u32 *)objects;
   int remaining = count;
   while(remaining--)
   {
      *dst++ = ATTR0_HIDE;
      *dst++ = 0;
   }

   oam_copy((gba_object *)OAM_ADDRESS, objects, count);
}

static inline void object_set_position(gba_object *object, int x, int y)
{
   FLAG_SET(ATTR0_Y, object->attribute0, y);
   FLAG_SET(ATTR1_X, object->attribute1, x);
}

static inline void object_hide(gba_object *object)
{
   FLAG_SET_NOSHIFT(ATTR0_OBJECTMODE, object->attribute0, ATTR0_HIDE);
}

static inline void object_unhide(gba_object *object, u16 mode)
{
   FLAG_SET_NOSHIFT(ATTR0_OBJECTMODE, object->attribute0, mode);
}

static gba_object objects[128];

int main(void)
{
   charblock4 *block = &TILE4_BLOCKS[4];
   u16 *palette = (u16 *)PALETTE_OBJ;

   memcpy(block, debug_tiles, sizeof(debug_tiles));
   memcpy(palette, debug_palette, sizeof(debug_palette));

   oam_initialize(objects, countof(objects));

   REGISTER_DISPCNT = DISPCNT_OBJ|DISPCNT_OBJ1D;

   gba_object *object = objects + 0;
   object->attribute0 = ATTR0_4BPP;
   object->attribute1 = ATTR1_SIZE_32;
   object->attribute2 = ATTR2_INDEX(0) | ATTR2_PALETTEBANK(0);

   int posx = 96;
   int posy = 32;

   gba_input input = {0};
   while(gba_begin_frame(&input))
   {
      if(is_held(input, BUTTON_LEFT))  posx -= 1;
      if(is_held(input, BUTTON_RIGHT)) posx += 1;
      if(is_held(input, BUTTON_UP))    posy -= 1;
      if(is_held(input, BUTTON_DOWN))  posy += 1;

      is_held(input, BUTTON_START)
         ? object_hide(object)
         : object_unhide(object, 0);

      object_set_position(object, posx, posy);

      gba_object *dst = (gba_object *)OAM_ADDRESS + 0;
      oam_copy(dst, objects, 1);
   }

   return(0);
}
