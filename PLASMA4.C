#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <conio.h>
#include <dos.h>
#include <math.h>
#include <mem.h>

#include "types.h"
#include "vga.h"
#include "pal.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

byte far *framebuf;
byte SIN256[256];
byte pal[768];

#define SETPIX(x,y,c) *(framebuf + (dword)SCREEN_WIDTH * (y) + (x)) = c
#define GETPIX(x,y) *(framebuf + (dword)SCREEN_WIDTH * (y) + (x))

#define MODE_Y

void init_sin()
{
  word i;
  float v;
  for( i = 0; i < 256; ++i ) {
    v = (sin( 2.0 * M_PI * i / 255.0 ) + 1.0 ) / 2.0;
    SIN256[ i ] = (byte)(255 * v);
  }
}

void draw_plasma(word x, word y, word w, word h, dword t)
{
  byte c;
  word c1,c2,c3,v1,v2;
  word i,j;

  for(j = y; j < y + h; j++){
    c3 = SIN256[(j*3 + t*3) % 256];
    v1 = SIN256[(t*3)%256];
    v2 = (j*SIN256[(64+t*5)%256])/h;
    for(i = x; i < x + w; i++) {
      c1 = SIN256[(i*3 + t*2) % 256];
      c2 = SIN256[(
	     (i*v1)/w +
	     v2 +
	     t
	   ) % 256];
      c = (byte)((c1 + c2 + c3) / 3);
      SETPIX(i,j,c);
    }
  }
}

int main()
{
  byte do_quit = 0;
  int i;
  dword t;

  init_sin();
  randomize();
  t = random(4096);

  framebuf = malloc(SCREEN_WIDTH*SCREEN_HEIGHT);
  if(framebuf == NULL) {
    printf("not enough memory\n");
    return 1;
  }
#ifdef MODE_Y
  set_mode_y();
#else
  set_graphics_mode();
#endif
  set_palette(fire_pal);
  memset(framebuf, 0x00, SCREEN_WIDTH*SCREEN_HEIGHT);
  while( !do_quit ) {
    if(kbhit()){
      getch();
      do_quit = 1;
    }
#ifdef MODE_Y
    draw_plasma(0,0,80,50,t);
    wait_for_retrace();
    blit4(framebuf,0,0,80,50);
#else
    draw_plasma(80,50,160,100,t);
    wait_for_retrace();
    memcpy(VGA,framebuf,SCREEN_WIDTH*SCREEN_HEIGHT);
#endif
    t++;
  }
  set_text_mode();
  return 0;
}
