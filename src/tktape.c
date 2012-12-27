/* Tcl/Tk tape commands
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
#include <tcl.h>

#include "tape.h"

static Tcl_Interp *interp;
static int TapeAttachCmd(ClientData clientData, Tcl_Interp *_interp,
                         int objc, Tcl_Obj *CONST objv[]);

void
TkTape_init(Tcl_Interp *_interp)
{
  interp = _interp;

  Tcl_CreateObjCommand(interp, "TapeAttach", TapeAttachCmd,
                       (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);
}

static int
TapeAttachCmd(ClientData clientData, Tcl_Interp *_interp,
              int objc, Tcl_Obj *CONST objv[])
{
  char *filename;

  if (objc != 2) {
    Tcl_WrongNumArgs(_interp, 1, objv, "filename");
    return TCL_ERROR;
  }

  filename = Tcl_GetString(objv[1]);
  // FIX: Check success of command below?
  tape_attach(filename);

  return TCL_OK;
}


void
TkTape_observer(int tape_attached, int tape_pos,
    TapeMessageType message_type, char *message)
{
  const int statusMsgSize = 256;
  char statusMsg[statusMsgSize+1];

  switch (message_type) {
    case TAPE_MESSAGE:
      if (tape_attached) {
        snprintf(statusMsg, statusMsgSize,
                 "Tape position: %04d - %s", tape_pos, message);
      } else {
        snprintf(statusMsg, statusMsgSize,
                 "Tape position: %04d - %s (Empty tape)", tape_pos, message);
      }
      break;

    case TAPE_ERROR:
      if (tape_attached) {
        snprintf(statusMsg, statusMsgSize,
                 "Tape position: %04d - Error: %s", tape_pos, message);
      } else {
        snprintf(statusMsg, statusMsgSize,
                 "Tape position: %04d - Error: %s (Empty tape)",
                 tape_pos, message);
      }
      break;
  }

  Tcl_SetVar(interp, "StatusMsg", statusMsg, 0);
}
