main: floyd_warshall.c
	gcc -o main floyd_warshall.c

tests: tests.c
	gcc -o tests tests.c

run: main
	./main

run_file: main
	./main input.txt output.txt

run_small: main
	./main input_small.txt output_small.txt

run_tests: tests
	./tests

clean:
	rm -f main tests *.o output.txt output_small.txt
