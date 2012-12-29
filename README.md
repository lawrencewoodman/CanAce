CanAce
======
A Jupiter ACE emulator.

The Jupiter ACE
---------------

The Jupiter ACE was designed by Richard Altwasser and Steven Vickers who had previously been on the design team for the Sinclair ZX Spectrum.  It was released in 1982 and stood out from the crowd because of the bold decision to use Forth instead of Basic as its programming language.  Forth was much better suited to the small memory and low processing power of early microcomputers and allowed users of the Ace to do a lot more with the hardware.

Origins of CanAce
-----------------
CanAce is derived from [xAce](http://lawrencewoodman.github.com/xAce/) which was written by Edward Patel and heavily modified by [Lawrence Woodman](http://techtinkering.com).  This was in turn based on a ZX81 emulator written by Russell Marks, called: xz81, which was based on Ian Collier's ZX Spectrum emulator: xz80.

Requirements
------------
*  A C/C++ compiler (Currently tested with gcc and g++)
*  Tcl/Tk 8.5+ and development files
*  SDL Library and development files
*  CMake 2.8+ (To compile)

Compiling and Running
---------------------
CanAce is designed to be run under Unix and has been most thoroughly tested on Linux.

From the root directory of the repository, the directory this file is in, create the Makefiles:

    $ cmake .

Then compile the project:

    $ make

The binary executable will now be in `src/`, to install it to a sensible
location such as `/usr/local/bin` run the following as root:

    $ make install

For the moment CanAce must be run from the same directory as the ROM image.
So from the current directory run:

    $ canace

Special ACE keys
----------------
To make the emulator easier to use the following keys are mapped:

    F1     - Delete Line
    F4     - Inverse Video
    F9     - Graphics
    F12    - Reset
    Esc    - Break

Loading/Saving External Files
-----------------------------
You can attach a tape by clicking on the _'Attach Tape'_ button.
Once done, from that point any loading or saving is done to the selected file.

_It is important to note that when you save, the rest of the file is
truncated._

Spooling
--------
You can spool text into the emulator from an external file by clicking on the
_'Spool from File'_ button.

Alternatively you can use the -s command line switch e.g.

    $ ./canace -s spool.file

or to spool more quickly, use -S e.g.

    $ ./canace -S spool.file

_The host keyboard response is turned off during spooling to avoid corruption._

The ROM
-------
The distribution includes a copy of the Jupiter ACE ROM image and a copy of an
email from Boldfield Computing which sheds light on the legal position of
distributing the ROM with the emulator: boldcomp.email.txt.

Useful Links
------------
* The [CanAce Development Google Group](http://groups.google.com/group/canace-dev).  Where you can play your part in discussing and shaping the direction of CanAce.
* The [Jupiter Ace Resource Site](http://www.jupiter-ace.co.uk) is highly recommended.  You will find lots of information about the ACE, in addition to an active forum and a great software archive (most of the .TAP files work with CanAce).

Credits
--------
* Steven Vickers and Richard Altwasser
* Edward Patel
* Russell Marks
* Ian Collier

Contributions
-------------
Contributions to CanAce are welcome.  Please make a pull request to the [repo](https://github.com/LawrenceWoodman/canace) on github.  You will also find that there is a growing testsuite in `tests/`.

Contact
-------
CanAce was created by [Lawrence Woodman](http://techtinkering.com) who can be contacted by email at <lwoodman@vlifesystems.com>.

Licence
-------
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.
