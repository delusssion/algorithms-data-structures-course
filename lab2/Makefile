main_prog: main.c
	gcc -o main main.c -lm

tests: tests.c main.c
	gcc -DUNIT_TEST -o tests tests.c main.c -lm

run: main_prog
	./main

run_tests: tests
	./tests

clean:
	rm -f main tests
