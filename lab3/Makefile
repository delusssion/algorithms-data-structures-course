main: expression_tree.c
	gcc -o main expression_tree.c -lm

tests: tests.c
	gcc -o tests tests.c -lm

run: main
	./main

run_tests: tests
	./tests

clean:
	rm -f main tests *.o
