#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define NO_EDGE_MARKER (-1)
#define INF (LLONG_MAX / 4)

static inline long long *cell(long long *m, int n, int i, int j) {
    return m + (long long)i * n + j;
}

int fw_read_matrix(FILE *in, int *out_n, long long **out_dist) {
    int n;
    if (fscanf(in, "%d", &n) != 1 || n <= 0) return 0;
    long long *dist = (long long *)malloc(sizeof(long long) * n * n);
    if (!dist) return 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            long long w;
            if (fscanf(in, "%lld", &w) != 1) {
                free(dist);
                return 0;
            }
            if (i == j) {
                *cell(dist, n, i, j) = 0;
            } else if (w == NO_EDGE_MARKER) {
                *cell(dist, n, i, j) = INF;
            } else {
                *cell(dist, n, i, j) = w;
            }
        }
    }
    *out_n = n;
    *out_dist = dist;
    return 1;
}

void fw_run(int n, long long *dist) {
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            long long ik = *cell(dist, n, i, k);
            if (ik >= INF) continue;
            for (int j = 0; j < n; j++) {
                long long kj = *cell(dist, n, k, j);
                if (kj >= INF) continue;
                long long nd = ik + kj;
                long long *cur = cell(dist, n, i, j);
                if (nd < *cur) *cur = nd;
            }
        }
    }
}

int fw_has_negative_cycle(int n, const long long *dist) {
    for (int i = 0; i < n; i++) {
        if (dist[(long long)i * n + i] < 0) return 1;
    }
    return 0;
}

void fw_print_matrix(FILE *out, int n, const long long *dist) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            long long v = dist[(long long)i * n + j];
            if (j) fputc(' ', out);
            if (v >= INF) fputs("INF", out);
            else fprintf(out, "%lld", v);
        }
        fputc('\n', out);
    }
}

int main(int argc, char *argv[]) {
    FILE *in = stdin;
    FILE *out = stdout;
    if (argc >= 2) {
        in = fopen(argv[1], "r");
        if (!in) {
            fprintf(stderr, "Ошибка: не удалось открыть '%s'\n", argv[1]);
            return 1;
        }
    }
    if (argc >= 3) {
        out = fopen(argv[2], "w");
        if (!out) {
            fprintf(stderr, "Ошибка: не удалось открыть '%s' для записи\n", argv[2]);
            if (in != stdin) fclose(in);
            return 1;
        }
    }

    int n;
    long long *dist = NULL;
    if (!fw_read_matrix(in, &n, &dist)) {
        fprintf(stderr, "Ошибка: некорректный формат входных данных\n");
        if (in != stdin) fclose(in);
        if (out != stdout) fclose(out);
        return 1;
    }

    fw_run(n, dist);

    if (fw_has_negative_cycle(n, dist)) {
        fprintf(out, "В графе обнаружен отрицательный цикл\n");
    } else {
        fprintf(out, "Матрица кратчайших расстояний:\n");
        fw_print_matrix(out, n, dist);
    }

    free(dist);
    if (in != stdin) fclose(in);
    if (out != stdout) fclose(out);
    return 0;
}
