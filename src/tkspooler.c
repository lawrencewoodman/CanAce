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

void
TkSpooler_init(Tcl_Interp *_interp)
{
  interp = _interp;
}

void
TkSpooler_observer(SpoolerMessage message)
{
  switch (message) {
    case SPOOLER_OPENED:
      Tcl_SetVar(interp, "statusMsg", "Spooling from file...", 0);
      break;

    case SPOOLER_OPEN_ERROR:
      // FIX: Should perhaps have a dialog open for errors
      Tcl_SetVar(interp, "statusMsg", "Couldn't open spool file.", 0);
      break;

    case SPOOLER_CLOSED:
      Tcl_SetVar(interp, "statusMsg", "Finished spooling.", 0);
      break;

    case SPOOLER_ALREADY_SPOOLING:
      Tcl_SetVar(interp, "statusMsg", "The spooler is already spooling.", 0);
      break;
  }
}
