# GUI creation and handling
#
# Copyright (C) 2012 Lawrence Woodman
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#/
proc handle_emu_key {key} {
  switch $key {
    F12 {reset_ace}
  }
}

proc create_window {} {
  wm title . {CanAce}
  wm protocol . WM_DELETE_WINDOW {destroy_window}
}

proc create_screen {} {
  set width [expr {[get_scale] * 256}]
  set height [expr {[get_scale] * 192}]
  frame .screen -width $width -height $height -background ""
  pack .screen
}

proc get_tap_file {} {
  tk_getOpenFile -filetypes {
    {{Tape file} {.tap .Tap .TAP}}
    {{All files} *}
  }
}

proc get_spool_file {} {
  tk_getOpenFile -filetypes {
    {{Spool file} {.spool .Spool .SPOOL}}
    {{All files} *}
  }
}

proc create_buttons {} {
  frame .buttons

  ttk::button .buttons.tape_attach -text {Attach Tape} -command {
    set filename [get_tap_file]

    if {$filename != ""} {
      tape_attach $filename
    }
    focus .screen
  }

  ttk::button .buttons.spool -text {Spool from File} -command {
    set filename [get_spool_file]

    if {$filename != ""} {
      spool $filename
    }
    focus .screen
  }

  ttk::button .buttons.reset -text {Reset} -command {reset_ace; focus .screen}
  ttk::button .buttons.quit -text {Quit} -command {destroy_window}

  pack .buttons.tape_attach -side left -expand 1 -fill both
  pack .buttons.spool -side left -expand 1 -fill both
  pack .buttons.reset -side left -expand 1 -fill both
  pack .buttons.quit -side left -expand 1 -fill both
  pack .buttons -side left -expand 1 -fill both
}

proc bind_events {} {
  #  FIX: make sure spooler isn't active before passing key to keyrelease
  bind . <KeyPress> {keypress %K; handle_emu_key %K}
  bind . <KeyRelease> {keyrelease %K}
  bind . <Expose> {screen_refresh}
}

create_window
create_screen
create_buttons
bind_events
