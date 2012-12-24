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
  switch -- $key {
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

proc displayAbout {} {
  tk_messageBox -message "CanAce v[getCanAceVersion]" -type ok
}

proc create_menu {} {
  option add *Menu.tearOff 0
  menu .mbar
  . configure -menu .mbar

  .mbar add cascade -label File -menu .mbar.file -underline 0
  .mbar add cascade -label Help -menu .mbar.help -underline 0

  # keyclear is used below to reset the key modifiers because otherwise the
  # keypress command will register an Alt key being pressed to access the menu,
  # but will never see it released.  This would mean all following keys would
  # be ignored by keypress.
  menu .mbar.file -postcommand {keyclear}
  menu .mbar.help -postcommand {keyclear}

  .mbar.file add command -label "Attach a Tape" -underline 0 -command {
    set filename [get_tap_file]

    if {$filename != ""} {
      tape_attach $filename
    }
    focus .screen
  }

  .mbar.file add command -label "Spool from File" -underline 0 -command {
    set filename [get_spool_file]

    if {$filename != ""} {
      spool $filename
    }
    focus .screen
  }

  .mbar.file add command -label "Reset" -underline 0 -command {reset_ace}

  .mbar.file add command -label "Quit" -underline 0 -command {destroy_window}

  .mbar.help add command -label "About" -underline 0 -command {displayAbout}

}

proc bind_events {} {
  bind . <KeyPress> {
    keypress %K
    handle_emu_key %K
  }

  bind . <KeyRelease> {keyrelease %K}
  bind . <Expose> {screen_refresh}
}

create_window
create_menu
create_screen
bind_events
