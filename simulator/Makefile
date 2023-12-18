.PHONY: clean
CFLAGS=-std=c99 -g -Wall -fno-common -DSIM -I. -I../src -I../libavr32/src
DEPS =
OBJ = tt.o ../src/teletype.o ../src/command.o ../src/helpers.o ../src/drum_helpers.o \
	../src/every.o ../src/match_token.o ../src/scanner.o \
	../src/scale.o ../src/scene_serialization.o \
	../src/state.o ../src/table.o ../src/turtle.o ../src/chaos.o \
	../src/ops/op.o ../src/ops/ansible.c ../src/ops/controlflow.o \
	../src/ops/delay.o ../src/ops/earthsea.o ../src/ops/hardware.o \
	../src/ops/justfriends.o ../src/ops/meadowphysics.o ../src/ops/turtle.o \
	../src/ops/metronome.o ../src/ops/maths.o ../src/ops/orca.o \
	../src/ops/patterns.o ../src/ops/queue.o ../src/ops/stack.o \
	../src/ops/telex.o ../src/ops/variables.o  ../src/ops/whitewhale.o \
	../src/ops/init.o ../src/ops/grid_ops.o ../src/ops/er301.o \
	../src/ops/fader.o ../src/ops/matrixarchate.o ../src/ops/wslash.o \
	../src/ops/seed.o ../src/ops/i2c.o ../src/ops/disting.o \
	../src/ops/wslashtape.o ../src/ops/wslashsynth.o ../src/ops/wslashdelay.o \
	../src/ops/wslash_shared.o ../src/ops/crow.o ../src/ops/i2c2midi.o\
	../libavr32/src/euclidean/euclidean.o ../libavr32/src/euclidean/data.o \
	../libavr32/src/music.o ../libavr32/src/util.o ../libavr32/src/random.o \
        ../src/ops/midi.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tt: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

../src/match_token.c: ../src/match_token.rl
	ragel -C -G2 ../src/match_token.rl -o ../src/match_token.c

../src/scanner.c: ../src/scanner.rl
	ragel -C -G2 ../src/scanner.rl -o ../src/scanner.c

clean:
	rm -f tt
	rm -rf tt.dSYM
	rm -f *.o
	rm -f ../src/*.o
	rm -f ../src/ops/*.o
	rm -f ../libavr32/src/euclidean/*.o
	rm -f ../libavr32/src/*.o
	rm -f ../src/match_token.c
	rm -f ../src/scanner.c
