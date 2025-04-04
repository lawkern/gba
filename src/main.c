/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

#include "gba.h"

static void draw_pixel(int x, int y, u16 color)
{
   VRAM[SCREEN_WIDTH*y + x] = color;
}

static void draw_gradient(void)
{
   for(int y = 0; y < SCREEN_HEIGHT; y++)
   {
      for(int x = 0; x < SCREEN_WIDTH; x++)
      {
         draw_pixel(x, y, RGB(x*31/SCREEN_WIDTH, 15, y*31/SCREEN_HEIGHT));
      }
   }
}

#define START_POSX 120
#define START_POSY 80

int main(void)
{
   REG_DISPCNT = MODE3|BG2;

   draw_gradient();

   int dim = 10;
   int posx = START_POSX;
   int posy = START_POSY;

   gba_input input = {0};

   while(1)
   {
      // Wait for vertical blank.
      while(REG_VCOUNT >= SCREEN_HEIGHT) {}
      while(REG_VCOUNT < SCREEN_HEIGHT) {}

      for(int y = posy; y < posy+dim; y++)
      {
         for(int x = posx; x < posx+dim; x++)
         {
            draw_pixel(x, y, RGB(x*31/SCREEN_WIDTH, y*31/SCREEN_HEIGHT, 15));
         }
      }

      gba_process_input(&input);
      if(is_held(input, BUTTON_LEFT))  posx -= 1;
      if(is_held(input, BUTTON_RIGHT)) posx += 1;
      if(is_held(input, BUTTON_UP))    posy -= 1;
      if(is_held(input, BUTTON_DOWN))  posy += 1;

      if(was_pressed(input, BUTTON_START))
      {
         posx = START_POSX;
         posy = START_POSY;
         draw_gradient();
      }

      for(int y = posy; y < posy+dim; y++)
      {
         for(int x = posx; x < posx+dim; x++)
         {
            draw_pixel(x, y, WHITE);
         }
      }
   }

   return(0);
}
