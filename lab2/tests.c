#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

typedef struct {
    double *keys;
    char **values;
    int size;
} Table;

Table read_table_from_file(const char *filename);
Table create_table(int size);
void free_table(Table *table);
char *my_strdup(const char *src);
int double_equal(double a, double b);
void heap_sort(Table *table);
int binary_search(const Table *table, double target);

void write_test_input_file(const char *filename) {
    FILE *f = fopen(filename, "w");
    assert(f != NULL);

    fprintf(f, "16\n");
    fprintf(f, "3.14\nvalue0\n");
    fprintf(f, "1.10\nvalue1\n");
    fprintf(f, "2.20\nvalue2\n");
    fprintf(f, "4.40\nvalue3\n");
    fprintf(f, "5.50\nvalue4\n");
    fprintf(f, "6.60\nvalue5\n");
    fprintf(f, "7.70\nvalue6\n");
    fprintf(f, "8.80\nvalue7\n");
    fprintf(f, "9.90\nvalue8\n");
    fprintf(f, "10.01\nvalue9\n");
    fprintf(f, "11.11\nvalue10\n");
    fprintf(f, "12.12\nvalue11\n");
    fprintf(f, "13.13\nvalue12\n");
    fprintf(f, "14.14\nvalue13\n");
    fprintf(f, "15.15\nvalue14\n");
    fprintf(f, "16.16\nvalue15\n");

    fclose(f);
}


void test_read_table_from_file() {
    const char *filename = "test_input.txt";
    write_test_input_file(filename);
    Table t = read_table_from_file(filename);

    assert(t.size == 16);
    assert(double_equal(t.keys[0], 3.14));
    assert(t.values[0] != NULL);

    free_table(&t);
    remove(filename);
    printf("test_read_table_from_file passed\n");
}


void test_heap_sort_unsorted() {
    Table t = create_table(5);

    t.keys[0] = 5.5; t.values[0] = my_strdup("a");
    t.keys[1] = 1.1; t.values[1] = my_strdup("b");
    t.keys[2] = 3.3; t.values[2] = my_strdup("c");
    t.keys[3] = 2.2; t.values[3] = my_strdup("d");
    t.keys[4] = 4.4; t.values[4] = my_strdup("e");

    heap_sort(&t);

    assert(double_equal(t.keys[0], 1.1));
    assert(double_equal(t.keys[1], 2.2));
    assert(double_equal(t.keys[2], 3.3));
    assert(double_equal(t.keys[3], 4.4));
    assert(double_equal(t.keys[4], 5.5));

    free_table(&t);
    printf("test_heap_sort_unsorted passed\n");
}


void test_heap_sort_sorted() {
    Table t = create_table(4);

    t.keys[0] = 1.0; t.values[0] = my_strdup("a");
    t.keys[1] = 2.0; t.values[1] = my_strdup("b");
    t.keys[2] = 3.0; t.values[2] = my_strdup("c");
    t.keys[3] = 4.0; t.values[3] = my_strdup("d");

    heap_sort(&t);

    assert(double_equal(t.keys[0], 1.0));
    assert(double_equal(t.keys[1], 2.0));
    assert(double_equal(t.keys[2], 3.0));
    assert(double_equal(t.keys[3], 4.0));

    free_table(&t);
    printf("test_heap_sort_sorted passed\n");
}


void test_binary_search_found() {
    Table t = create_table(5);

    t.keys[0] = 1.0; t.values[0] = my_strdup("a");
    t.keys[1] = 2.0; t.values[1] = my_strdup("b");
    t.keys[2] = 3.0; t.values[2] = my_strdup("c");
    t.keys[3] = 4.0; t.values[3] = my_strdup("d");
    t.keys[4] = 5.0; t.values[4] = my_strdup("e");

    int index = binary_search(&t, 3.0);

    assert(index == 2);
    assert(double_equal(t.keys[index], 3.0));

    free_table(&t);
    printf("test_binary_search_found passed\n");
}


void test_binary_search_not_found() {
    Table t = create_table(5);

    t.keys[0] = 1.0; t.values[0] = my_strdup("a");
    t.keys[1] = 2.0; t.values[1] = my_strdup("b");
    t.keys[2] = 3.0; t.values[2] = my_strdup("c");
    t.keys[3] = 4.0; t.values[3] = my_strdup("d");
    t.keys[4] = 5.0; t.values[4] = my_strdup("e");

    int index = binary_search(&t, 7.0);

    assert(index == -1);

    free_table(&t);
    printf("test_binary_search_not_found passed\n");
}

int main() {
    test_read_table_from_file();
    test_heap_sort_unsorted();
    test_heap_sort_sorted();
    test_binary_search_found();
    test_binary_search_not_found();

    printf("All tests passed successfully!\n");
    return 0;
}
