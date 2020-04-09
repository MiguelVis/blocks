Blocks
======

This game is a clon (more or less) of [SameGame](https://en.wikipedia.org/wiki/SameGame), a tile-matching puzzle video game, originally
released under the name Chain Shot! in 1985 by Kuniaki Moribe (Morisuke).

![Blocks](http://www.floppysoftware.es/images/cpm-blocks.jpg)

v2.0 - 09 April 2020.

Copyright (c) 2012-2020 Miguel Garcia / FloppySoftware, Spain.

www.floppysoftware.es

floppysoftware@gmail.com


Introduction
------------

This game it's derived from my own previous Blocks for CP/M, now ported
to the new [KS library](https://github.com/MiguelVis/ks).

It should run in any version of the CP/M operating system, running in any
computer with a supported CRT, and a Z80 CPU.


Running the game
----------------

To start the game, type at the CP/M prompt the following:

```
blocks TTY_NAME
```

where `TTY_NAME` is one of the supported TTY names,
and follow the instructions.

To know the supported TTYs, just type `blocks` at the CP/M prompt - ie:

```
A>blocks
Usage: blocks tty_name

Supported TTY names:
VT52
VT100
PCW
CPC
SPECTRUM
KAYPRO
```

The game
--------

The object of the game is to remove all the
blocks on the board.

You can select just one block, or a group of
blocks of the same type.

Then, if you are in the normal mode, you can place all
the blocks against a side (top, bottom, left or right).

If you are in the automatic mode, the blocks rows will be
placed on the bottom side of the board, and the columns
will be placed on the middle of the board.

The partial score is the square of the number of
selected blocks.

The special block 'X', multiplies that result by 10.

Good luck!


Technical notes
---------------

This program was developed using MESCC (Mike's Enhanced Small C Compiler - my
own version of Small C) and my KS (Keyboard & Screen) libraries for CP/M.


License
-------

This software is copyright of Miguel Garcia / FloppySoftware, Spain. All rights
reserved.

This program is freeware, and it's licensed under the GNU General Public License.

See the license file for more details.
