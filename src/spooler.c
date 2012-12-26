/* Spooler controller
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

#include <stdio.h>

#include "spooler.h"

static FILE *spooler_file = NULL;
static enum {
  SPOOLER_INACTIVE,
  SPOOLER_READ_CHAR,
  SPOOLER_CLEAR_CHAR
} spooler_state = SPOOLER_INACTIVE;

static SpoolerObserver spooler_observer = NULL;
static ClearKeyboardFunc clear_keyboard = NULL;
static KeypressFunc keypress = NULL;
static NormalSpeedFunc normal_speed = NULL;

void
spooler_init(SpoolerObserver spooler_observer_func,
             ClearKeyboardFunc clear_keyboard_func,
             KeypressFunc keypress_func,
             NormalSpeedFunc normal_speed_func)
{
  spooler_observer = spooler_observer_func;
  clear_keyboard = clear_keyboard_func;
  keypress = keypress_func;
  normal_speed = normal_speed_func;
  spooler_state = SPOOLER_INACTIVE;
}

void
spooler_open(char *filename)
{
  if (spooler_active()) {
      spooler_observer(SPOOLER_ALREADY_SPOOLING);
  } else {
    spooler_file = fopen(filename, "rt");
    if (spooler_file) {
      spooler_state = SPOOLER_READ_CHAR;
      spooler_observer(SPOOLER_OPENED);
      clear_keyboard();
    } else {
      spooler_observer(SPOOLER_OPEN_ERROR);
    }
  }
}

void
spooler_close(void)
{
  if (spooler_active()) {
    fclose(spooler_file);
    normal_speed();
    clear_keyboard();
    spooler_file = NULL;
    spooler_state = SPOOLER_INACTIVE;
    spooler_observer(SPOOLER_CLOSED);
  }
}

static void
spooler_read_char(void)
{
  AceKey aceKey;

  aceKey = fgetc(spooler_file);
  if (aceKey == EOF) {
    spooler_close();
  } else {
    keypress(aceKey);
  }
}

void
spooler_read(void)
{
  switch (spooler_state) {
    case SPOOLER_READ_CHAR:
      spooler_state = SPOOLER_CLEAR_CHAR;
      spooler_read_char();
      break;
    case SPOOLER_CLEAR_CHAR:
      spooler_state = SPOOLER_READ_CHAR;
      clear_keyboard();
      break;
  }
}

int
spooler_active(void)
{
  return spooler_state != SPOOLER_INACTIVE;
}
