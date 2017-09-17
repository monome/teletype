.PHONY: clean test
CFLAGS = -std=c99 -g -Wall -fno-common -DSIM -I../src -I../libavr32/src

tests: main.o \
	match_token_tests.o op_mod_tests.o \
	parser_tests.o process_tests.o \
	turtle_tests.o \
	../src/teletype.o ../src/command.o ../src/helpers.o \
	../src/match_token.o ../src/scanner.o \
	../src/state.o ../src/table.o \
	../src/ops/op.o ../src/ops/ansible.c ../src/ops/controlflow.o \
	../src/ops/delay.o ../src/ops/earthsea.o ../src/ops/hardware.o \
	../src/ops/justfriends.o ../src/ops/meadowphysics.o \
	../src/ops/metronome.o ../src/ops/maths.o ../src/ops/orca.o \
	../src/ops/patterns.o ../src/ops/queue.o ../src/ops/stack.o \
	../src/ops/telex.o ../src/ops/variables.o  ../src/ops/whitewhale.c \
	../src/ops/turtle.o \
	../libavr32/src/euclidean/data.o ../libavr32/src/euclidean/euclidean.o \
	../libavr32/src/util.o
	$(CC) -o $@ $^ $(CFLAGS)

../src/match_token.c: ../src/match_token.rl
	ragel -C -G2 ../src/match_token.rl -o ../src/match_token.c

../src/scanner.c: ../src/scanner.rl
	ragel -C -G2 ../src/scanner.rl -o ../src/scanner.c

test: tests
	@./tests | greatest/greenest

clean:
	rm -f tests
	rm -rf tests.dSYM
	rm -f *.o
	rm -f ../src/*.o
	rm -f ../src/ops/*.o
	rm -f ../libavr32/src/euclidean/*.o
	rm -f ../libavr32/src/*.o
	rm -f ../src/match_token.c
	rm -f ../src/scanner.c
