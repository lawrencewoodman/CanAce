/* Tcl/Tk key handling commands
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
#include <stdbool.h>
#include <tcl.h>

#include "keyboard.h"
#include "spooler.h"

static AceKey tk2aceKey(char *keyText);
static int KeypressCmd(ClientData clientData, Tcl_Interp *interp,
                       int objc, Tcl_Obj *CONST objv[]);
static int KeyreleaseCmd(ClientData clientData, Tcl_Interp *interp,
                         int objc, Tcl_Obj *CONST objv[]);
static int KeyClearCmd(ClientData clientData, Tcl_Interp *_interp,
                       int objc, Tcl_Obj *CONST objv[]);
static bool keyModifiersPressed(void);
static void processKeyModifiers(char *key);

static struct {
  bool control_l;
  bool control_r;
  bool alt_l;
} keyModifiers = {false, false, false};

static struct {
  char *tkKey;
  AceKey aceKey;
} tk2aceKeyTable[] = {
  "F1", AceKey_DeleteLine,
  "F4", AceKey_InverseVideo,
  "F9", AceKey_Graphics,
  "Left", AceKey_Left,
  "Down", AceKey_Down,
  "Up", AceKey_Up,
  "Right", AceKey_Right,
  "Delete", AceKey_Delete,
  "BackSpace", AceKey_Delete,
  "1", AceKey_1,
  "2", AceKey_2,
  "3", AceKey_3,
  "4", AceKey_4,
  "5", AceKey_5,
  "6", AceKey_6,
  "7", AceKey_7,
  "8", AceKey_8,
  "9", AceKey_9,
  "0", AceKey_0,
  "exclam", AceKey_exclam,
  "at", AceKey_at,
  "numbersign", AceKey_numbersign,
  "dollar", AceKey_dollar,
  "percent", AceKey_percent,
  "Escape", AceKey_Break,
  "ampersand", AceKey_ampersand,
  "apostrophe", AceKey_apostrophe,
  "parenleft", AceKey_parenleft,
  "parenright", AceKey_parenright,
  "underscore", AceKey_underscore,
  "A", AceKey_A,
  "a", AceKey_a,
  "B", AceKey_B,
  "b", AceKey_b,
  "C", AceKey_C,
  "c", AceKey_c,
  "D", AceKey_D,
  "d", AceKey_d,
  "E", AceKey_E,
  "e", AceKey_e,
  "F", AceKey_F,
  "f", AceKey_f,
  "G", AceKey_G,
  "g", AceKey_g,
  "H", AceKey_H,
  "h", AceKey_h,
  "I", AceKey_I,
  "i", AceKey_i,
  "J", AceKey_J,
  "j", AceKey_j,
  "K", AceKey_K,
  "k", AceKey_k,
  "L", AceKey_L,
  "l", AceKey_l,
  "M", AceKey_M,
  "m", AceKey_m,
  "N", AceKey_N,
  "n", AceKey_n,
  "O", AceKey_O,
  "o", AceKey_o,
  "P", AceKey_P,
  "p", AceKey_p,
  "Q", AceKey_Q,
  "q", AceKey_q,
  "R", AceKey_R,
  "r", AceKey_r,
  "S", AceKey_S,
  "s", AceKey_s,
  "T", AceKey_T,
  "t", AceKey_t,
  "U", AceKey_U,
  "u", AceKey_u,
  "V", AceKey_V,
  "v", AceKey_v,
  "W", AceKey_W,
  "w", AceKey_w,
  "X", AceKey_X,
  "x", AceKey_x,
  "Y", AceKey_Y,
  "y", AceKey_y,
  "Z", AceKey_Z,
  "z", AceKey_z,
  "less", AceKey_less,
  "greater", AceKey_greater,
  "bracketleft", AceKey_bracketleft,
  "bracketright", AceKey_bracketright,
  "copyright", AceKey_copyright,
  "semicolon", AceKey_semicolon,
  "quotedbl", AceKey_quotedbl,
  "asciitilde", AceKey_asciitilde,
  "bar", AceKey_bar,
  "backslash", AceKey_backslash,
  "braceleft", AceKey_braceleft,
  "braceright", AceKey_braceright,
  "asciicircum", AceKey_asciicircum,
  "minus", AceKey_minus,
  "plus", AceKey_plus,
  "equal", AceKey_equal,
  "Return", AceKey_Return,
  "colon", AceKey_colon,
  "sterling", AceKey_sterling,
  "question", AceKey_question,
  "slash", AceKey_slash,
  "asterisk", AceKey_asterisk,
  "comma", AceKey_comma,
  "period", AceKey_period,
  "space", AceKey_space,
  "Tab", AceKey_Tab,
};

void
TkKeys_createCommands(Tcl_Interp *interp)
{
  Tcl_CreateObjCommand(interp, "KeyPress", KeypressCmd,
                       (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateObjCommand(interp, "KeyRelease", KeyreleaseCmd,
                       (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateObjCommand(interp, "KeyClear", KeyClearCmd,
                       (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);
}

static int
KeypressCmd(ClientData clientData, Tcl_Interp *interp,
            int objc, Tcl_Obj *CONST objv[])
{
  char *key;
  AceKey ace_key;

  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "key");
    return TCL_ERROR;
  }

  key = Tcl_GetString(objv[1]);
  processKeyModifiers(key);

  if (!keyModifiersPressed()) {
    ace_key = tk2aceKey(key);
    if (ace_key != AceKey_none && !spooler_active())
      keyboard_keypress(ace_key);
  }

  return TCL_OK;
}

static int
KeyreleaseCmd(ClientData clientData, Tcl_Interp *interp,
              int objc, Tcl_Obj *CONST objv[])
{
  char *key;
  AceKey ace_key;

  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "key");
    return TCL_ERROR;
  }

  key = Tcl_GetString(objv[1]);
  processKeyModifiers(key);

  ace_key = tk2aceKey(key);
  if (ace_key != AceKey_none && !spooler_active())
    keyboard_keyrelease(ace_key);

  return TCL_OK;
}

static int
KeyClearCmd(ClientData clientData, Tcl_Interp *_interp,
            int objc, Tcl_Obj *CONST objv[])
{
  if (objc != 1) {
    Tcl_WrongNumArgs(_interp, 1, objv, "");
  }

  keyboard_clear();
  keyModifiers.control_l = false;
  keyModifiers.control_r = false;
  keyModifiers.alt_l = false;
  return TCL_OK;
}


static AceKey
tk2aceKey(char *keyText)
{
  int i;
  int numKeys = sizeof(tk2aceKeyTable)/sizeof(tk2aceKeyTable[0]);

  for (i = 0; i < numKeys; i++) {
    if (strcmp(tk2aceKeyTable[i].tkKey, keyText) == 0)
      return tk2aceKeyTable[i].aceKey;
  }
  return AceKey_none;
}

static bool
keyModifiersPressed(void)
{
  return keyModifiers.control_l || keyModifiers.control_r ||
         keyModifiers.alt_l;
}

static void
processKeyModifiers(char *key)
{
  if (strcmp("Control_L", key) == 0)
    keyModifiers.control_l = !keyModifiers.control_l;
  else if (strcmp("Control_R", key) == 0)
    keyModifiers.control_r = !keyModifiers.control_r;
  else if (strcmp("Alt_L", key) == 0)
    keyModifiers.alt_l = !keyModifiers.alt_l;
}
