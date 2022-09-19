compile:
	gcc -O0 -g src/main.c src/jp.c src/arguments.c -o bin/jp

test: compile
	./bin/jp data.json

test-indent: compile
	./bin/jp --indent=4 data.json

test-indent-zero: compile
	./bin/jp --indent=0 data.json

test-compact: compile
	./bin/jp --compact data.json