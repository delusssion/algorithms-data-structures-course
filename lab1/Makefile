main_prog: main.c
	gcc -o main main.c

tests: tests.c main.c
	gcc -o tests tests.c

run: main_prog
	./main

run_tests: tests
	./tests

clean:
	rm -f main tests
