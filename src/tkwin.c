/* Tk window setup functions and Tcl commands
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
#include <string.h>
#include <tcl.h>
#include <tk.h>

#include "acescreen.h"
#include "keyboard.h"
#include "tape.h"
#include "spooler.h"
#include "tkkeys.h"
#include "tkspooler.h"
#include "tktape.h"

static Tcl_Interp *interp;
static unsigned char *mem;

static int TkWin_displayWindow(void);
static void TkWin_createCommands(void);

int
TkWin_init(unsigned char *_mem)
{
  // Include canace.tcl which has been converted to a char array using xxd -i
  #include "canace.tcl.h"

  mem = _mem;
  interp = Tcl_CreateInterp();
  if (Tcl_Init(interp) == TCL_ERROR) { return 0; }
  if (Tk_Init(interp) == TCL_ERROR) { return 0; }

  TkWin_createCommands();
  TkSpooler_init(interp);
  TkTape_init(interp);

  if (Tcl_EvalEx(interp, canace_tcl, canace_tcl_len, 0) == TCL_ERROR ) {
    fprintf(stderr, "Error in embedded canace.tcl\n");
    fprintf(stderr, "%s\n", Tcl_GetStringResult(interp));
    return 0;
  }

  TkWin_displayWindow();

  return 1;
}

char *
TkWin_getWindowID(void)
{
  Tcl_Obj *result;

  if (Tcl_Eval(interp, "winfo id .screen") == TCL_ERROR)
    return NULL;

  result = Tcl_GetObjResult(interp);
  return Tcl_GetStringFromObj(result, NULL);
}

void
TkWin_checkEvents(void)
{
  Tk_DoOneEvent(TK_ALL_EVENTS|TK_DONT_WAIT);
}

static int
TkWin_displayWindow(void)
{
  return Tcl_Eval(interp, "update");
}

static int
DestroyWindowCmd(ClientData clientData, Tcl_Interp *_interp,
                 int objc, Tcl_Obj *CONST objv[])
{
  if (objc != 1) {
    Tcl_WrongNumArgs(_interp, 1, objv, "");
  }

  spooler_close();
  tape_detach();
  AceScreen_destroy();
  Tcl_Eval(_interp, "exit");
  return TCL_OK;
}

static int
ScreenRefreshCmd(ClientData clientData, Tcl_Interp *_interp,
                 int objc, Tcl_Obj *CONST objv[])
{
  if (objc != 1) {
    Tcl_WrongNumArgs(_interp, 1, objv, "");
  }

  AceScreen_refresh();
  return TCL_OK;
}

static int
ResetAceCmd(ClientData clientData, Tcl_Interp *_interp,
            int objc, Tcl_Obj *CONST objv[])
{
  if (objc != 1) {
    Tcl_WrongNumArgs(_interp, 1, objv, "");
  }

  spooler_close();
  reset_ace();
  keyboard_clear();
  AceScreen_refresh();
  return TCL_OK;
}


static int
GetScaleCmd(ClientData clientData, Tcl_Interp *_interp,
            int objc, Tcl_Obj *CONST objv[])
{
  int scale;

  if (objc != 1) {
    Tcl_WrongNumArgs(_interp, 1, objv, "");
  }

  Tcl_SetObjResult(_interp, Tcl_NewIntObj(SCALE));

  return TCL_OK;
}

static int
GetCanAceVersionCmd(ClientData clientData, Tcl_Interp *_interp,
                    int objc, Tcl_Obj *CONST objv[])
{
  if (objc != 1) {
    Tcl_WrongNumArgs(_interp, 1, objv, "");
  }

  Tcl_SetObjResult(_interp, Tcl_NewStringObj(CANACE_VERSION, -1));

  return TCL_OK;
}

static int
LoadROMCmd(ClientData clientData, Tcl_Interp *_interp,
           int objc, Tcl_Obj *CONST objv[])
{
  FILE *fp;
  char *filename;
  int  success = 0;

  if (objc != 2) {
    Tcl_WrongNumArgs(_interp, 2, objv, "ROMfilename");
  }

  filename = Tcl_GetString(objv[1]);

  if ( (fp=fopen(filename, "rb")) != NULL) {
    if (fread(mem, 1, 8192, fp) == 8192)
      success = 1;
    fclose(fp);
  }

  Tcl_SetObjResult(_interp, Tcl_NewBooleanObj(success));
  return TCL_OK;
}


static void
TkWin_createCommands(void)
{
  Tcl_CreateObjCommand(interp, "DestroyWindow", DestroyWindowCmd,
                       (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateObjCommand(interp, "ScreenRefresh", ScreenRefreshCmd,
                       (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateObjCommand(interp, "ResetAce", ResetAceCmd,
                       (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateObjCommand(interp, "GetScale", GetScaleCmd,
                       (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateObjCommand(interp, "GetCanAceVersion", GetCanAceVersionCmd,
                       (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateObjCommand(interp, "LoadROM", LoadROMCmd,
                       (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);

  TkKeys_createCommands(interp);
}
