/* Declarations for handling the emulated keyboard
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

#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef enum {
  AceKey_none,
  AceKey_DeleteLine, AceKey_InverseVideo, AceKey_Graphics,
  AceKey_Left, AceKey_Down, AceKey_Up, AceKey_Right,
  AceKey_Delete = 0x08,
  AceKey_1 = 0x31, AceKey_2 = 0x32, AceKey_3 = 0x33, AceKey_4 = 0x34,
  AceKey_5 = 0x35, AceKey_6 = 0x36, AceKey_7 = 0x37, AceKey_8 = 0x38,
  AceKey_9 = 0x39, AceKey_0 = 0x30,
  AceKey_exclam = 0x21, AceKey_at = 0x40, AceKey_numbersign = 0x23,
  AceKey_dollar = 0x24, AceKey_percent = 0x25,
  AceKey_Break,
  AceKey_ampersand = 0x26, AceKey_apostrophe = 0x27,
  AceKey_parenleft = 0x28, AceKey_parenright = 0x29,
  AceKey_underscore = 0x5F,
  AceKey_A = 0x41, AceKey_a = 0x61, AceKey_B = 0x42, AceKey_b = 0x62,
  AceKey_C = 0x43, AceKey_c = 0x63, AceKey_D = 0x44, AceKey_d = 0x64,
  AceKey_E = 0x45, AceKey_e = 0x65, AceKey_F = 0x46, AceKey_f = 0x66,
  AceKey_G = 0x47, AceKey_g = 0x67, AceKey_H = 0x48, AceKey_h = 0x68,
  AceKey_I = 0x49, AceKey_i = 0x69, AceKey_J = 0x4A, AceKey_j = 0x6A,
  AceKey_K = 0x4B, AceKey_k = 0x6B, AceKey_L = 0x4C, AceKey_l = 0x6C,
  AceKey_M = 0x4D, AceKey_m = 0x6D, AceKey_N = 0x4E, AceKey_n = 0x6E,
  AceKey_O = 0x4F, AceKey_o = 0x6F, AceKey_P = 0x50, AceKey_p = 0x70,
  AceKey_Q = 0x51, AceKey_q = 0x71, AceKey_R = 0x52, AceKey_r = 0x72,
  AceKey_S = 0x53, AceKey_s = 0x73, AceKey_T = 0x54, AceKey_t = 0x74,
  AceKey_U = 0x55, AceKey_u = 0x75, AceKey_V = 0x56, AceKey_v = 0x76,
  AceKey_W = 0x57, AceKey_w = 0x77, AceKey_X = 0x58, AceKey_x = 0x78,
  AceKey_Y = 0x59, AceKey_y = 0x79, AceKey_Z = 0x5A, AceKey_z = 0x80,
  AceKey_less = 0x3C, AceKey_greater = 0x3E,
  AceKey_bracketleft = 0x5B, AceKey_bracketright = 0x5D,
  AceKey_copyright,
  AceKey_semicolon = 0x3B, AceKey_quotedbl = 0x22, AceKey_asciitilde = 0x7E,
  AceKey_bar = 0x7C, AceKey_backslash = 0x5C,
  AceKey_braceleft = 0x7B, AceKey_braceright = 0x7D,
  AceKey_asciicircum = 0x5E,
  AceKey_minus = 0x2D, AceKey_plus = 0x2B, AceKey_equal = 0x3D,
  AceKey_Return = 0x0A,
  AceKey_colon = 0x3A, AceKey_sterling = 0x9C, AceKey_question = 0x3F,
  AceKey_slash = 0x2F, AceKey_asterisk = 0x2A, AceKey_comma = 0x2C,
  AceKey_period = 0x2E,
  AceKey_space = 0x20,
  AceKey_Tab = 0x09
} AceKey;

extern void keyboard_init(void);
extern unsigned char keyboard_get_keyport(int port);
extern void keyboard_clear(void);
extern void keyboard_keypress(AceKey aceKey);
extern void keyboard_keyrelease(AceKey aceKey);

#endif
