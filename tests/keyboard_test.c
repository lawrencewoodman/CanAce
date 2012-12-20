/* Tests for keyboard emulation
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
#include <assert.h>
#include <stdlib.h>

#include "keyboard.h"

static void
check_keyports(unsigned char *expected_keyports)
{
  int i;
  for (i = 0; i < 8; i++) {
    assert(keyboard_get_keyport(i) == expected_keyports[i]);
  }
}

static void
test_keyboard_clear()
{
  unsigned char expected_keyports[8] = {
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff
  };

  keyboard_init();
  keyboard_keypress(AceKey_3);
  keyboard_keypress(AceKey_7);
  keyboard_keypress(AceKey_u);
  keyboard_keypress(AceKey_e);
  keyboard_keypress(AceKey_f);
  keyboard_keypress(AceKey_l);
  keyboard_keypress(AceKey_n);
  keyboard_keypress(AceKey_z);
  keyboard_clear();

  check_keyports(expected_keyports);
}

static void
test_keyboard_keypress_single_key()
{
  unsigned char expected_keyports[8] = {
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xfe
  };

  keyboard_init();
  keyboard_keypress(AceKey_Tab);
  check_keyports(expected_keyports);
}

static void
test_keyboard_keypress_multiple_keys()
{
  unsigned char expected_keyports[8] = {
    0xff, 0xf7, 0xfb, 0xff,
    0xf7, 0xf7, 0xff, 0xfb
  };

  keyboard_init();
  keyboard_keypress(AceKey_7);
  keyboard_keypress(AceKey_u);
  keyboard_keypress(AceKey_e);
  keyboard_keypress(AceKey_f);
  keyboard_keypress(AceKey_n);
  check_keyports(expected_keyports);
}

static void
test_keyboard_keypress_symbol_on_physical_keyboard()
{
  unsigned char expected_keyports[8] = {
    0xfd, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xf7
  };

  keyboard_init();
  keyboard_keypress(AceKey_asterisk);
  check_keyports(expected_keyports);
}

static void
test_keyboard_keypress_key_not_found()
{
  unsigned char expected_keyports[8] = {
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff
  };

  keyboard_init();
  keyboard_keypress(0xff);
  check_keyports(expected_keyports);
}
/*
 FIX: Detect control key
static void
test_keyboard_keypress_ignore_keyports_for_keys_pressed_with_control_key()
{
  unsigned char expected_keyports[8] = {
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff
  };

  non_ace_key_handler_init();
  keyboard_init(non_ace_key_handler);
  keyboard_keypress(XK_A, ControlMask);
  assert(non_ace_key_handler_status.handler_called);
  assert(non_ace_key_handler_status.keySym == XK_A);
  assert(non_ace_key_handler_status.key_state == ControlMask);

  check_keyports(expected_keyports);
}
*/

static void
test_keyboard_keyrelease_from_single_key()
{
  unsigned char expected_keyports[8] = {
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff
  };

  keyboard_init();
  keyboard_keypress(AceKey_A);
  keyboard_keyrelease(AceKey_A);
  check_keyports(expected_keyports);
}

static void
test_keyboard_keyrelease_from_single_key_with_multiple_pressed()
{
  unsigned char expected_keyports[8] = {
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xfe
  };

  keyboard_init();
  keyboard_keypress(AceKey_A);
  keyboard_keypress(AceKey_Tab);
  keyboard_keyrelease(AceKey_A);
  check_keyports(expected_keyports);
}

/*
 * FIX: Detect control key pressed

static void
test_keyboard_keyrelease_ignore_keyports_for_keys_pressed_with_control_key()
{
  unsigned char expected_keyports[8] = {
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff
  };

  keyboard_init();
  keyboard_keyrelease(XK_A, ControlMask);

  check_keyports(expected_keyports);
}
*/

int main()
{
  test_keyboard_clear();
  test_keyboard_keypress_single_key();
  test_keyboard_keypress_multiple_keys();
  test_keyboard_keypress_symbol_on_physical_keyboard();
  test_keyboard_keypress_key_not_found();
  //test_keyboard_keypress_ignore_keyports_for_keys_pressed_with_control_key();
  test_keyboard_keyrelease_from_single_key();
  test_keyboard_keyrelease_from_single_key_with_multiple_pressed();
  //test_keyboard_keyrelease_ignore_keyports_for_keys_pressed_with_control_key();
  exit(0);
}
