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
static gba_object ascii[128];

int main(void)
{
   memcpy(TILE4_BLOCKS[4], debug_tiles, sizeof(debug_tiles));
   memcpy(TILE4_BLOCKS[5], text_tiles, sizeof(text_tiles));

   u16 *palette = (u16 *)PALETTE_OBJ;
   memcpy(palette, debug_palette, sizeof(debug_palette));

   oam_initialize(objects, countof(objects));
   oam_initialize(ascii, countof(ascii));

   REGISTER_DISPCNT = DISPCNT_OBJ|DISPCNT_OBJ1D;

   int object_count = 0;
   gba_object *debug_object = objects + object_count++;
   debug_object->attribute0 = ATTR0_4BPP;
   debug_object->attribute1 = ATTR1_SIZE_16;
   debug_object->attribute2 = ATTR2_INDEX(0) | ATTR2_PALETTEBANK(0);

   int ascii_count = 0;
   for(int index = 0; index < countof(text_tiles)/8; index++)
   {
      gba_object *object = ascii + ascii_count++;
      object->attribute0 = ATTR0_4BPP;
      object->attribute1 = ATTR1_SIZE_8;
      object->attribute2 = ATTR2_INDEX(index + 512) | ATTR2_PALETTEBANK(0);
   }

   int posx = SCREEN_WIDTH/2 - 8;
   int posy = SCREEN_HEIGHT/2 - 8;

   gba_input input = {0};
   while(gba_begin_frame(&input))
   {
      if(is_held(input, BUTTON_LEFT))  posx -= 1;
      if(is_held(input, BUTTON_RIGHT)) posx += 1;
      if(is_held(input, BUTTON_UP))    posy -= 1;
      if(is_held(input, BUTTON_DOWN))  posy += 1;

      is_held(input, BUTTON_START)
         ? object_hide(debug_object)
         : object_unhide(debug_object, 0);

      object_set_position(debug_object, posx, posy);
      for(int index = 0; index < countof(text_tiles)/8; index++)
      {
         int x = 8 * index;
         int y = 0;

         while(x >= SCREEN_WIDTH)
         {
            x -= SCREEN_WIDTH;
            y += 8;
         }

         object_set_position(ascii + index, x, y);
      }

      gba_object *dst = (gba_object *)OAM_ADDRESS + 0;
      oam_copy(dst, objects, object_count);

      dst = (gba_object *)OAM_ADDRESS + object_count;
      oam_copy(dst, ascii, ascii_count);
   }

   return(0);
}
