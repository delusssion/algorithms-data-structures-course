all: app tests

app: main.c
	gcc main.c -o app -lm

tests: tests.c main.c
	gcc -DUNIT_TEST tests.c main.c -o tests -lm

test: tests
	./tests

clean:
	rm -f app tests
