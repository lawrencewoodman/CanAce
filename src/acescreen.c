/* Emulation of the Ace's screen
 *
 * Copyright (C) 2012 Lawrence Woodman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <SDL.h>
#include "acescreen.h"

const int screen_width = 256;
const int screen_height = 192;
const int borderWidth = 20;

static SDL_Surface *sfScreen = NULL;
static int scale = 1;
static unsigned char *video_ram = NULL;
static unsigned char *charset = NULL;

// Used to see if anything has changed on screen
static unsigned char video_ram_old[24*32];

static void AceScreen_embedInWindow(char *windowID);
static void AceScreen_clear(void);
static void AceScreen_setPixel(int x, int y, int colour);
static void AceScreen_rawSetPixel(int x, int y, Uint32 pixel, int bpp);
static void AceScreen_setCharacter(int x, int y, int inv,
                                   unsigned char *charbmap);

int
AceScreen_init(char *windowID, int _scale, unsigned char *_video_ram,
               unsigned char *_charset)
{
  char buf[50];

  scale = _scale;
  video_ram = _video_ram;
  charset = _charset;

  snprintf(buf, 50, "SDL_WINDOWID=%s", windowID);
  SDL_putenv(buf);

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    return 0;
  }

  sfScreen = SDL_SetVideoMode(((2*borderWidth) + screen_width) * scale,
                              ((2*borderWidth) + screen_height) * scale,
                              0, SDL_NOFRAME|SDL_SWSURFACE|SDL_ANYFORMAT);
  if (sfScreen == NULL) {
    fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    return 0;
  }

  memset(video_ram_old, 0xff, 768);
  AceScreen_clear();

  return 1;
}

void
AceScreen_destroy(void)
{
  SDL_Quit();
}

void
AceScreen_refresh(void)
{
  int x,y,c,inv;
  int video_ram_ofs;
  int video_ram_old_ofs;
  int chrmap_changed = 0;

  if (sfScreen == NULL)
    return;

  video_ram_old_ofs=0;
  video_ram_ofs = 0;
  for (y = 0; y < 24; y++) {
    for (x = 0; x < 32; x++, video_ram_ofs++, video_ram_old_ofs++) {
      c = video_ram[video_ram_ofs];
      if (c != video_ram_old[video_ram_old_ofs]) {
        video_ram_old[video_ram_old_ofs] = c;

        inv = c&128;
        c &= 127;

        AceScreen_setCharacter(x, y, inv, charset+c*8);
      }
    }
  }

  SDL_Flip(sfScreen);
}

SDL_Surface *
AceScreen_getScreenSurface(void)
{
  return sfScreen;
}

static void
AceScreen_clear(void)
{
  SDL_FillRect(sfScreen, NULL, 0);
  SDL_Flip(sfScreen);
}


/* Set a pixel in the image
 * color == 0 is white, else black
 * FIX: Note comment about this not being very efficient:
 *   http://sdl.beuc.net/sdl.wiki/Pixel_Access
 */
void
AceScreen_setPixel(int x, int y, int colour)
{
  int sx, sy;
  int bpp = sfScreen->format->BytesPerPixel;
  Uint32 pixel = colour ? 0xffffffff : 0;

  for(sy = 0; sy < scale; sy++) {
    for(sx = 0; sx < scale; sx++) {
      AceScreen_rawSetPixel((borderWidth+x)*scale+sx, (borderWidth+y)*scale+sy,
                            pixel, bpp);
    }
  }
}

static void
AceScreen_rawSetPixel(int x, int y, Uint32 pixel, int bpp)
{
  // The address of the pixel we want to set
  Uint8 *p = (Uint8 *)sfScreen->pixels + (y) * sfScreen->pitch + (x) * bpp;

  /* FIX: Could probably just fix at 8 bits depth and therefore assume
   * case 1
   */
  switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
  }
}


/* Set a character in the image
 * x              Column in image to draw character
 * y              Row in image to draw character
 * inv            Whether an inverted character
 * charbmap       Ptr to the character bit map
 */
void
AceScreen_setCharacter(int x, int y, int inv, unsigned char *charbmap)
{
  int colour;
  int charbmap_x, charbmap_y;
  unsigned char charbmap_row;
  unsigned char charbmap_row_mask;

  for (charbmap_y = 0; charbmap_y < 8; charbmap_y++) {
    charbmap_row = charbmap[charbmap_y];
    if (inv) charbmap_row ^= 255;

    charbmap_row_mask = 128;
    for (charbmap_x = 0; charbmap_x < 8; charbmap_x++) {
      colour = (charbmap_row & charbmap_row_mask);
      AceScreen_setPixel(x*8+charbmap_x, y*8+charbmap_y, colour);
      charbmap_row_mask >>= 1;
    }
  }
}
