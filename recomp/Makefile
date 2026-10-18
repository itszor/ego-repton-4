# Static recompilation of Ego: Repton 4: the translated program (gen/, from
# superspider gen) and game.c, linked with libroger (the RISC OS runtime).
ROGER   ?= $(HOME)/stuff/libroger
SS      ?= $(HOME)/stuff/superspider
CC      ?= gcc
CFLAGS  ?= -O2 -g -w -fno-strict-aliasing $(if $(TRACE),-DRECOMP_TRACE)
GEN     := $(wildcard gen/code*.c) gen/entries.c
OBJ     := $(GEN:.c=.o) game.o
LIBS    := $(shell pkg-config --libs sdl3) -lm -lpthread
INC     := -I$(ROGER)/include
IMG     := ../game/!Repton4/ObjCode,00008000-0001e9b8
SSB     := $(SS)/_build/default/bin/main.exe

repton4: $(OBJ) $(ROGER)/libroger.a
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(ROGER)/libroger.a $(LIBS)

$(ROGER)/libroger.a: FORCE
	$(MAKE) -C $(ROGER) libroger.a

gen/%.o: gen/%.c $(ROGER)/include/roger_cpu.h gen/gen.h
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

game.o: game.c $(ROGER)/include/roger.h
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# ObjCode, analysed from its execution address (&1E9B8: the protection
# check appended to the program) and from &8000, where the check goes on to
# (the program's start-up clears its BSS, the check among it, so the
# analysis can't follow the jump). r12 is the program's workspace pointer
# throughout (--global-r12).
regen:
	cd $(SS) && dune build
	touch ../analysis/objcode.roots
	$(SSB) analyse '$(IMG)' 8000 --exec 1e9b8 --entry 8000 --full --global-r12 --roots ../analysis/objcode.roots -o ../analysis/objcode.json
	rm -rf gen
	$(SSB) gen '$(IMG)' 8000 ../analysis/objcode.json --c gen

# "make roots" merges the gap entries a --trace-gaps run logged
# (untranslated.log, or LOG=...) into ../analysis/objcode.roots.
LOG ?= untranslated.log

roots:
	python3 -I $(SS)/tools/gap_roots.py '$(IMG)' 8000 ../analysis/objcode.roots $(LOG)

clean:
	rm -f gen/*.o game.o repton4

.PHONY: FORCE regen roots clean
