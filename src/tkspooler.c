/* Tcl/Tk spooler commands
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

#include "spooler.h"

static Tcl_Interp *interp;

// Prototypes
static int SpoolCmd(ClientData clientData, Tcl_Interp *_interp,
                    int objc, Tcl_Obj *CONST objv[]);

void
TkSpooler_init(Tcl_Interp *_interp)
{
  interp = _interp;

  Tcl_CreateObjCommand(interp, "Spool", SpoolCmd,
                       (ClientData) NULL,
                       (Tcl_CmdDeleteProc *) NULL);
}

void
TkSpooler_observer(SpoolerMessage message)
{
  switch (message) {
    case SPOOLER_OPENED:
      Tcl_SetVar(interp, "StatusMsg", "Spooling from file...", 0);
      break;

    case SPOOLER_OPEN_ERROR:
      // FIX: Should perhaps have a dialog open for errors
      Tcl_SetVar(interp, "StatusMsg", "Couldn't open spool file.", 0);
      break;

    case SPOOLER_CLOSED:
      Tcl_SetVar(interp, "StatusMsg", "Finished spooling.", 0);
      break;

    case SPOOLER_ALREADY_SPOOLING:
      Tcl_SetVar(interp, "StatusMsg", "The spooler is already spooling.", 0);
      break;
  }
}

static int
SpoolCmd(ClientData clientData, Tcl_Interp *_interp,
         int objc, Tcl_Obj *CONST objv[])
{
  char *filename;

  if (objc != 2) {
    Tcl_WrongNumArgs(_interp, 1, objv, "filename");
    return TCL_ERROR;
  }

  filename = Tcl_GetString(objv[1]);
  // FIX: Check success of command below?
  spooler_open(filename);

  return TCL_OK;
}
