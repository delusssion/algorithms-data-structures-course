#include <gtest/gtest.h>
#include <cstdio>
#include <cstdlib>

extern "C" {
#define main fw_main
#include "floyd_warshall.c"
#undef main
}

TEST(FloydWarshall, ReadMatrixBasic) {
    FILE *f = fopen("/tmp/fw_test_basic_gtest.txt", "w");
    fprintf(f, "3\n0 5 -1\n-1 0 2\n1 -1 0\n");
    fclose(f);

    f = fopen("/tmp/fw_test_basic_gtest.txt", "r");
    int n;
    long long *dist = NULL;
    int ok = fw_read_matrix(f, &n, &dist);
    fclose(f);

    EXPECT_EQ(ok, 1);
    EXPECT_EQ(n, 3);
    EXPECT_EQ(dist[0 * 3 + 1], 5);
    EXPECT_EQ(dist[0 * 3 + 2], INF);
    EXPECT_EQ(dist[2 * 3 + 0], 1);

    free(dist);
}

TEST(FloydWarshall, RunRelaxThroughIntermediate) {
    int n = 4;
    long long m[16] = {
        0,   1,   INF, 100,
        INF, 0,   1,   INF,
        INF, INF, 0,   1,
        INF, INF, INF, 0
    };

    fw_run(n, m);

    EXPECT_EQ(m[0 * 4 + 2], 2);
    EXPECT_EQ(m[0 * 4 + 3], 3);
}

TEST(FloydWarshall, RunNoPath) {
    int n = 4;
    long long m[16] = {
        0,   1,   INF, INF,
        INF, 0,   INF, INF,
        INF, INF, 0,   1,
        INF, INF, INF, 0
    };

    fw_run(n, m);

    EXPECT_EQ(m[0 * 4 + 1], 1);
    EXPECT_EQ(m[2 * 4 + 3], 1);
    EXPECT_EQ(m[0 * 4 + 2], INF);
    EXPECT_EQ(m[1 * 4 + 3], INF);
}

TEST(FloydWarshall, NegativeEdgesNoCycle) {
    int n = 3;
    long long m[9] = {
        0,   4,   5,
        INF, 0,   -3,
        INF, INF, 0
    };

    fw_run(n, m);

    EXPECT_EQ(m[0 * 3 + 2], 1);
    EXPECT_EQ(fw_has_negative_cycle(n, m), 0);
}

TEST(FloydWarshall, NegativeCycleDetected) {
    int n = 3;
    long long m[9] = {
        0,   1,   INF,
        INF, 0,   -3,
        -3,  INF, 0
    };

    fw_run(n, m);

    EXPECT_EQ(fw_has_negative_cycle(n, m), 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
