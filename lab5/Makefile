main: floyd_warshall.c
	gcc -o main floyd_warshall.c

tests: tests.cpp
	g++ -std=c++17 -o tests tests.cpp -I/opt/homebrew/opt/googletest/include -L/opt/homebrew/opt/googletest/lib -lgtest -lgtest_main

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
