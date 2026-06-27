# Add a rule to build match_token.c from match_token.rl
../src/gen/match_token.c: ../src/match_token.rl ../src/gen/op_enum.h
	ragel -C -G2 ../src/match_token.rl -o ../src/gen/match_token.c

# Add a rule to build scanner.c from scanner.rl
../src/gen/scanner.c: ../src/scanner.rl
	ragel -C -G2 ../src/scanner.rl -o ../src/gen/scanner.c

# Add a rule to build op_enum.h from op.c and make it an additional dependency of object files that include it
../src/gen/op_enum.h: ../src/ops/op.c
	python3 ../utils/op_enums.py

../src/ops/op.h: ../src/gen/op_enum.h
../src/ops/op.o: ../src/gen/op_enum.h
../src/command.o: ../src/gen/op_enum.h
