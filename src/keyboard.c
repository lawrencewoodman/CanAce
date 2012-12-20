/* Emulation of the keyboard
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
 * --------------------------------------------------------------------------
 *
 * For information on how the Ace's keyboard works look at:
 *   http://www.jupiter-ace.co.uk/prog_keyboardread.html
 *
 * Certain compromises had to be made in the emulation of the keyboard.
 * It is not possible to detect the shift key separately for example,
 * because that would mean that it wouldn't be possible to use the symbols
 * on the physical keyboard such as $ or ^, as on the Ace these are accessed
 * via the symbol symbol shift key not the letter shift.  However, I don't
 * think this should be an issue very often and in turn it creates a much
 * nicer experience for the user of the emulator.
 */
#include <stdio.h>

#include "keyboard.h"

static unsigned char keyboard_ports[8] = {
  0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff
};

/* key, keyport_index, and_value, keyport_index, and_value
 * if keyport_index == -1 then no action for that port */
static const int keypress_response[] = {
  AceKey_DeleteLine, 3, 0xfe, 0, 0xfe,
  AceKey_InverseVideo, 3, 0xf7, 0, 0xfe,
  AceKey_Graphics, 4, 0xfd, 0, 0xfe,
  AceKey_Left, 3, 0xef, 0, 0xfe,
  AceKey_Down, 4, 0xf7, 0, 0xfe,
  AceKey_Up, 4, 0xef, 0, 0xfe,
  AceKey_Right, 4, 0xfb, 0, 0xfe,
  AceKey_Delete, 0, 0xfe, 4, 0xfe,
  AceKey_1, 3, 0xfe, -1, 0,
  AceKey_2, 3, 0xfd, -1, 0,
  AceKey_3, 3, 0xfb, -1, 0,
  AceKey_4, 3, 0xf7, -1, 0,
  AceKey_5, 3, 0xef, -1, 0,
  AceKey_6, 4, 0xef, -1, 0,
  AceKey_7, 4, 0xf7, -1, 0,
  AceKey_8, 4, 0xfb, -1, 0,
  AceKey_9, 4, 0xfd, -1, 0,
  AceKey_0, 4, 0xfe, -1, 0,
  AceKey_exclam, 3, 0xfe, 0, 0xfd,
  AceKey_at, 3, 0xfd, 0, 0xfd,
  AceKey_numbersign, 3, 0xfb, 0, 0xfd,
  AceKey_dollar, 3, 0xf7, 0, 0xfd,
  AceKey_percent, 3, 0xef, 0, 0xfd,
  AceKey_Break, 7, 0xfe, 0, 0xfe,     /* Break */
  AceKey_ampersand, 4, 0xef, 0, 0xfd,
  AceKey_apostrophe, 4, 0xf7, 0, 0xfd,
  AceKey_parenleft, 4, 0xfb, 0, 0xfd,
  AceKey_parenright, 4, 0xfd, 0, 0xfd,
  AceKey_underscore, 4, 0xfe, 0, 0xfd,
  AceKey_A, 0, 0xfe, 1, 0xfe,
  AceKey_a, 1, 0xfe, -1, 0,
  AceKey_B, 0, 0xfe, 7, 0xf7,
  AceKey_b, 7, 0xf7, -1, 0,
  AceKey_C, 0, 0xee, -1, 0,
  AceKey_c, 0, 0xef, -1, 0,
  AceKey_D, 0, 0xfe, 1, 0xfb,
  AceKey_d, 1, 0xfb, -1, 0,
  AceKey_E, 0, 0xfe, 2, 0xfb,
  AceKey_e, 2, 0xfb, -1, 0,
  AceKey_F, 0, 0xfe, 1, 0xf7,
  AceKey_f, 1, 0xf7, -1, 0,
  AceKey_G, 0, 0xfe, 1, 0xef,
  AceKey_g, 1, 0xef, -1, 0,
  AceKey_H, 0, 0xfe, 6, 0xef,
  AceKey_h, 6, 0xef, -1, 0,
  AceKey_I, 0, 0xfe, 5, 0xfb,
  AceKey_i, 5, 0xfb, -1, 0,
  AceKey_J, 0, 0xfe, 6, 0xf7,
  AceKey_j, 6, 0xf7, -1, 0,
  AceKey_K, 0, 0xfe, 6, 0xfb,
  AceKey_k, 6, 0xfb, -1, 0,
  AceKey_L, 0, 0xfe, 6, 0xfd,
  AceKey_l, 6, 0xfd, -1, 0,
  AceKey_M, 0, 0xfe, 7, 0xfd,
  AceKey_m, 7, 0xfd, -1, 0,
  AceKey_N, 0, 0xfe, 7, 0xfb,
  AceKey_n, 7, 0xfb, -1, 0,
  AceKey_O, 0, 0xfe, 5, 0xfd,
  AceKey_o, 5, 0xfd, -1, 0,
  AceKey_P, 0, 0xfe, 5, 0xfe,
  AceKey_p, 5, 0xfe, -1, 0,
  AceKey_Q, 0, 0xfe, 2, 0xfe,
  AceKey_q, 2, 0xfe, -1, 0,
  AceKey_R, 0, 0xfe, 2, 0xf7,
  AceKey_r, 2, 0xf7, -1, 0,
  AceKey_S, 0, 0xfe, 1, 0xfd,
  AceKey_s, 1, 0xfd, -1, 0,
  AceKey_T, 0, 0xfe, 2, 0xef,
  AceKey_t, 2, 0xef, -1, 0,
  AceKey_U, 0, 0xfe, 5, 0xf7,
  AceKey_u, 5, 0xf7, -1, 0,
  AceKey_V, 0, 0xfe, 7, 0xef,
  AceKey_v, 7, 0xef, -1, 0,
  AceKey_W, 0, 0xfe, 2, 0xfd,
  AceKey_w, 2, 0xfd, -1, 0,
  AceKey_X, 0, 0xf6, -1, 0,
  AceKey_x, 0, 0xf7, -1, 0,
  AceKey_Y, 0, 0xfe, 5, 0xef,
  AceKey_y, 5, 0xef, -1, 0,
  AceKey_Z, 0, 0xfa, -1, 0,
  AceKey_z, 0, 0xfb, -1, 0,
  AceKey_less, 2, 0xf7, 0, 0xfd,
  AceKey_greater, 2, 0xef, 0, 0xfd,
  AceKey_bracketleft, 5, 0xef, 0, 0xfd,
  AceKey_bracketright, 5, 0xf7, 0, 0xfd,
  AceKey_copyright, 5, 0xfb, 0, 0xfd,
  AceKey_semicolon, 5, 0xfd, 0, 0xfd,
  AceKey_quotedbl, 5, 0xfe, 0, 0xfd,
  AceKey_asciitilde, 1, 0xfe, 0, 0xfd,
  AceKey_bar, 1, 0xfd, 0, 0xfd,
  AceKey_backslash, 1, 0xfb, 0, 0xfd,
  AceKey_braceleft, 1, 0xf7, 0, 0xfd,
  AceKey_braceright, 1, 0xef, 0, 0xfd,
  AceKey_asciicircum, 6, 0xef, 0, 0xfd,
  AceKey_minus, 6, 0xf7, 0, 0xfd,
  AceKey_plus, 6, 0xfb, 0, 0xfd,
  AceKey_equal, 6, 0xfd, 0, 0xfd,
  AceKey_Return, 6, 0xfe, -1, 0,
  AceKey_colon, 0, 0xf9, -1, 0,
  AceKey_sterling, 0, 0xf5, -1, 0,
  AceKey_question, 0, 0xed, -1, 0,
  AceKey_slash, 7, 0xef, 0, 0xfd,
  AceKey_asterisk, 7, 0xf7, 0, 0xfd,
  AceKey_comma, 7, 0xfb, 0, 0xfd,
  AceKey_period, 7, 0xfd, 0, 0xfd,
  AceKey_space, 7, 0xfe, -1, 0,
  AceKey_Tab, 7, 0xfe, -1, 0,
};

void
keyboard_init(void)
{
  keyboard_clear();
}

unsigned char
keyboard_get_keyport(int port)
{
  return keyboard_ports[port];
}

void
keyboard_clear(void)
{
  int i;
  for (i = 0; i < 8; i++)
    keyboard_ports[i] = 0xff;
}

static int
keyboard_get_key_response(AceKey aceKey, int *keyport1, int *keyport2,
                          int *keyport1_response, int *keyport2_response)
{
  int i;
  int num_keys = sizeof(keypress_response)/sizeof(keypress_response[0]);

  for (i = 0; i < num_keys; i+= 5) {
    if (keypress_response[i] == aceKey) {
      *keyport1 = keypress_response[i+1];
      *keyport2 = keypress_response[i+3];
      *keyport1_response = keypress_response[i+2];
      *keyport2_response = keypress_response[i+4];
      return 1;
    }
  }
  return 0;
}

static void
keyboard_process_keypress_keyports(AceKey aceKey)
{
  int key_found;
  int keyport1, keyport2;
  int keyport1_and_value, keyport2_and_value;

  key_found = keyboard_get_key_response(aceKey, &keyport1, &keyport2,
                &keyport1_and_value, &keyport2_and_value);
  if (key_found) {
    keyboard_ports[keyport1] &= keyport1_and_value;
    if (keyport2 != -1)
      keyboard_ports[keyport2] &= keyport2_and_value;
  }
}

static void
keyboard_process_keyrelease_keyports(AceKey aceKey)
{
  int key_found;
  int keyport1, keyport2;
  int keyport1_or_value, keyport2_or_value;

  key_found = keyboard_get_key_response(aceKey, &keyport1, &keyport2,
                &keyport1_or_value, &keyport2_or_value);
  if (key_found) {
    keyboard_ports[keyport1] |= ~keyport1_or_value;
    if (keyport2 != -1)
      keyboard_ports[keyport2] |= ~keyport2_or_value;
  }
}

void
keyboard_keypress(AceKey aceKey)
{
  keyboard_process_keypress_keyports(aceKey);
}

void
keyboard_keyrelease(AceKey aceKey)
{
  keyboard_process_keyrelease_keyports(aceKey);
}
