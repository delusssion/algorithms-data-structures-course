#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define main fw_main
#include "floyd_warshall.c"
#undef main

static long long *make_matrix(int n) {
    long long *m = (long long *)malloc(sizeof(long long) * n * n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            m[(long long)i * n + j] = (i == j) ? 0 : INF;
    return m;
}

static void set_edge(long long *m, int n, int i, int j, long long w) {
    m[(long long)i * n + j] = w;
}

static long long get(const long long *m, int n, int i, int j) {
    return m[(long long)i * n + j];
}

static void test_read_matrix_basic(void) {
    const char *path = "/tmp/fw_test_basic.txt";
    FILE *f = fopen(path, "w");
    fprintf(f, "3\n0 5 -1\n-1 0 2\n1 -1 0\n");
    fclose(f);
    f = fopen(path, "r");
    int n;
    long long *dist = NULL;
    int ok = fw_read_matrix(f, &n, &dist);
    fclose(f);
    assert(ok == 1);
    assert(n == 3);
    assert(get(dist, n, 0, 0) == 0);
    assert(get(dist, n, 0, 1) == 5);
    assert(get(dist, n, 0, 2) == INF);
    assert(get(dist, n, 1, 0) == INF);
    assert(get(dist, n, 1, 2) == 2);
    assert(get(dist, n, 2, 0) == 1);
    free(dist);
    remove(path);
}

static void test_run_relax_through_intermediate(void) {
    int n = 4;
    long long *m = make_matrix(n);
    set_edge(m, n, 0, 1, 1);
    set_edge(m, n, 1, 2, 1);
    set_edge(m, n, 2, 3, 1);
    set_edge(m, n, 0, 3, 100);
    fw_run(n, m);
    assert(get(m, n, 0, 2) == 2);
    assert(get(m, n, 0, 3) == 3);
    free(m);
}

static void test_run_no_path(void) {
    int n = 4;
    long long *m = make_matrix(n);
    set_edge(m, n, 0, 1, 1);
    set_edge(m, n, 2, 3, 1);
    fw_run(n, m);
    assert(get(m, n, 0, 1) == 1);
    assert(get(m, n, 2, 3) == 1);
    assert(get(m, n, 0, 2) == INF);
    assert(get(m, n, 1, 3) == INF);
    free(m);
}

static void test_negative_edges_no_cycle(void) {
    int n = 3;
    long long *m = make_matrix(n);
    set_edge(m, n, 0, 1, 4);
    set_edge(m, n, 0, 2, 5);
    set_edge(m, n, 1, 2, -3);
    fw_run(n, m);
    assert(get(m, n, 0, 2) == 1);
    assert(fw_has_negative_cycle(n, m) == 0);
    free(m);
}

static void test_negative_cycle_detected(void) {
    int n = 3;
    long long *m = make_matrix(n);
    set_edge(m, n, 0, 1, 1);
    set_edge(m, n, 1, 2, -3);
    set_edge(m, n, 2, 0, -3);
    fw_run(n, m);
    assert(fw_has_negative_cycle(n, m) == 1);
    free(m);
}

int main(void) {
    test_read_matrix_basic();
    printf("test_read_matrix_basic passed\n");

    test_run_relax_through_intermediate();
    printf("test_run_relax_through_intermediate passed\n");

    test_run_no_path();
    printf("test_run_no_path passed\n");

    test_negative_edges_no_cycle();
    printf("test_negative_edges_no_cycle passed\n");

    test_negative_cycle_detected();
    printf("test_negative_cycle_detected passed\n");

    printf("All 5 tests passed.\n");
    return 0;
}
