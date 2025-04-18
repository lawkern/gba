/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

static void draw_pixel(int x, int y, u16 color)
{
   ((u16 *)VRAM_ADDRESS)[SCREEN_WIDTH*y + x] = color;
}

static void draw_gradient(void)
{
   for(int y = 0; y < SCREEN_HEIGHT; y++)
   {
      u16 b = 31*y / SCREEN_HEIGHT;
      for(int x = 0; x < SCREEN_WIDTH; x++)
      {
         u16 r = 31*x / SCREEN_WIDTH;
         draw_pixel(x, y, RGB(r, 15, b));
      }
   }
}

static void bitmap_test(void)
{
   REG_DISPCNT = DISPCNT_MODE3|DISPCNT_BG2;

   draw_gradient();

   int dim = 10;
   int posx = SCREEN_WIDTH/2;
   int posy = SCREEN_HEIGHT/2;

   gba_input input = {0};
   while(gba_begin_frame(&input))
   {
      for(int y = posy; y < posy+dim; y++)
      {
         u16 g = 31*y / SCREEN_HEIGHT;
         for(int x = posx; x < posx+dim; x++)
         {
            u16 r = 31*x / SCREEN_WIDTH;
            draw_pixel(x, y, RGB(r, g, 15));
         }
      }

      if(is_held(input, BUTTON_LEFT))  posx -= 1;
      if(is_held(input, BUTTON_RIGHT)) posx += 1;
      if(is_held(input, BUTTON_UP))    posy -= 1;
      if(is_held(input, BUTTON_DOWN))  posy += 1;

      if(was_pressed(input, BUTTON_START))
      {
         posx = SCREEN_WIDTH/2;
         posy = SCREEN_HEIGHT/2;
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
}
