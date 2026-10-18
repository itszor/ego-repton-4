Ego: Repton 4 (Superior Software, 1992) - static recompilation
==============================================================

A recipe: this repository holds no game data. It rebuilds the game as a
native executable from your own copy of the disc, recognised by content
(recipe.json), so it can be shared without distributing the game.

    ./build.sh "EGO - Repton 4 (1992)(Superior Software).adf"
    cd recomp && ./repton4

build.sh runs recipe.json's steps with SuperSpider's tools/weave.py
(~/stuff/superspider, or $SUPERSPIDER): it recognises the disc, extracts
it (keeping the image itself: extracted/disc1.img), analyses and
translates the program, and links it with libroger (~/stuff/libroger, or
make ROGER=...). It takes a few seconds. Needs python3, OCaml/dune, gcc
and SDL3.

Input: the TOSEC set's `EGO - Repton 4 (1992)(Superior Software).adf`.
That image is an 800K E format disc whose dump stops one track short
(795K): the last track, which is the copy protection (below).
SuperSpider's ADFSlib pads such an image to the size its disc record
gives to read the files; libroger, given the image as the disc in the
drive, has that track unformatted.

What it makes:

    extracted/disc1          the disc's files (!Repton4; Passwords, the levels'
                             passwords; VTJoystick, a joystick module !Run doesn't load)
    extracted/disc1.img      the disc image (weave.py's "raw": true)
    game/!Repton4            the application, libroger's appdir
    analysis/objcode.json    SuperSpider's analysis of ObjCode
    recomp/gen/              its C translation
    recomp/repton4           the game

Committed: recipe.json, build.sh, analysis/objcode.roots, recomp/
(Makefile, game.c), this file.

How the disc starts the game
----------------------------

!Run sets a 640K WimpSlot and Repton4$Dir and runs !RunImage, a BASIC
program of four lines: MODE 13, OFF, *ScreenLoad LoadScreen (the title
picture) and *Run ObjCode. game.c does the first three (init) and ObjCode
is the program: an absolute at &8000, entered at &1E9B8. The program is
hand-written assembler, keeping its workspace pointer in r12 throughout
(--global-r12); it reads its data from Resources (Font, Sprites, Panel,
Fronta/b, Allmaps - the maps - and Faces, the jigsaw pictures), and !Help.

The protection: at &1E9B8, appended to the program, a check verifies the
disc's last track (ADFS_DiscOp 0, disc address &C6C00), which has to fail
- the original's last track is unformatted - and then looks for
adfs::Repton4.$.!Repton4.Resources.Panel (OS_File 13: the disc by its
name). Either failing hangs the program; passing, it jumps to &8000. Both
run here, and pass as they did on the original: game.c gives libroger the
disc image (roger_config.floppy), whose missing last track is unformatted,
and maps the disc's name onto the application. The program's start-up
then clears its BSS from &1E9BC, the check among it (the ten "store to
compiled code" notes at start), so the analysis can't follow the check's
jump: &8000 is given as an entry too.

The analysis: 9,318 code words, 342 functions. One table of routines
(&1E14C, called from &9FB8) has three null entries in the middle; the
analysis's scan stops at them, and its five entries after them are roots
(analysis/objcode.roots). Played headless with --trace-gaps - the attract
mode (title, credits, high scores, the demo), the options, Define Keys,
statistics, a password, levels 1-12 (the eleven passwords on the disc),
losing every life, Ctrl-Q - there are no other gaps.

The game is paced by the vsync (it waits on a counter its event handler
counts), so it runs at the original's speed at any CPU speed. Its menus'
wipes run as fast as the host does.

Controls
--------

Z left, X right, ' up, / down, Return drop (Options, 7 redefines them).
At the title: O options, P password, S statistics, Return starts, Ctrl-Q
quits. Ctrl with 1-6 does the options' first six items at any time
(control, music and effects volume, fade).

A game controller holds those keys: the stick or D-pad moves, A or B
drop, start is Return. The game can also read Acorn's joystick
(Joystick_Read), when it finds one at start and "1. Control" in the
options is set to Stick 1; run with --joystick for that (the controller is
then the joystick, not keys).

A headless scenario (start a game; Ego walks right, then down;
screenshot-N.bmp):

    ./repton4 --limit=1e8 --full-os --script=12000:key:74:200,30000:shot,\
    32000:key:67:3000,36000:shot,37000:key:105:3000,41000:shot

Times in a headless run are emulated (2 MIPS), so the menus' wipes take a
few seconds each: keys pressed while a screen is still being drawn are
cleared when its menu starts reading them, as on the original.

Not done
--------

- Levels 13-30 have only been seen in the demo: the disc gives no
  passwords for them.
- After a game or the demo ends, the high score table sometimes shows a
  few stray pixels in the middle of the screen, the colours of Ego's
  sprite where Ego stands in play: probably the game's own sprite drawing
  racing the screen being cleared (in headless runs' timing at least). Not
  looked into further.
- The program calls an unknown SWI (&280, X form) at start and passes its
  result back at quit; libroger returns an error for it, as RISC OS 3.1
  would, and the game ignores it.
