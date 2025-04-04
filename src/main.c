/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

#include "gba.h"

void draw_pixel(int x, int y, u16 color)
{
   VRAM[SCREEN_WIDTH*y + x] = color;
}

int main(void)
{
   REG_DISPCNT = MODE3|BG2;

   for(int y = 0; y < SCREEN_HEIGHT; y++)
   {
      for(int x = 0; x < SCREEN_WIDTH; x++)
      {
         draw_pixel(x, y, RGB(x*31/SCREEN_WIDTH, 15, y*31/SCREEN_HEIGHT));
      }
   }

   int dim = 10;
   int posx = 120;
   int posy = 80;
   int dx = 2;
   int dy = 1;

   while(1)
   {
      // Wait for vertical blank.
      while(REG_VCOUNT >= 160) {}
      while(REG_VCOUNT < 160) {}

      for(int y = posy; y < posy+dim; y++)
      {
         for(int x = posx; x < posx+dim; x++)
         {
            draw_pixel(x, y, RGB(x*31/SCREEN_WIDTH, y*31/SCREEN_HEIGHT, 15));
         }
      }

      posx += dx;
      posy += dy;

      if(posx <= 0 || posx >= SCREEN_WIDTH-dim)  dx = -dx;
      if(posy <= 0 || posy >= SCREEN_HEIGHT-dim) dy = -dy;

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
