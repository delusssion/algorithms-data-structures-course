#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LINE_LEN 1024
#define EPS 1e-9
#define MIN_ELEMENTS 16

typedef struct {
    double *keys;
    char **values;
    int size;
} Table;



char *my_strdup(const char *src) {
    size_t len = strlen(src);
    char *copy = (char *)malloc(len + 1);
    if (copy == NULL) {
        printf("Ошибка: не удалось выделить память под строку.\n");
        exit(1);
    }
    strcpy(copy, src);
    return copy;
}

void remove_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') {
        s[len - 1] = '\0';
    }
}



Table create_table(int size) {
    Table table;
    table.size = size;

    table.keys = (double *)malloc(size * sizeof(double));
    table.values = (char **)malloc(size * sizeof(char *));

    if (table.keys == NULL || table.values == NULL) {
        printf("Ошибка: не удалось выделить память под таблицу.\n");
        exit(1);
    }

    for (int i = 0; i < size; i++) {
        table.values[i] = NULL;
    }

    return table;
}

void free_table(Table *table) {
    if (table == NULL) return;

    if (table->values != NULL) {
        for (int i = 0; i < table->size; i++) {
            free(table->values[i]);
        }
        free(table->values);
        table->values = NULL;
    }

    free(table->keys);
    table->keys = NULL;
    table->size = 0;
}

Table copy_table(const Table *src) {
    Table copy = create_table(src->size);

    for (int i = 0; i < src->size; i++) {
        copy.keys[i] = src->keys[i];
        copy.values[i] = my_strdup(src->values[i]);
    }

    return copy;
}



Table read_table_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Ошибка: не удалось открыть файл '%s'.\n", filename);
        exit(1);
    }

    int n;
    if (fscanf(file, "%d", &n) != 1) {
        printf("Ошибка: не удалось прочитать количество элементов.\n");
        fclose(file);
        exit(1);
    }

    if (n < MIN_ELEMENTS) {
        printf("Ошибка: для структуры №6 требуется минимум %d элементов.\n", MIN_ELEMENTS);
        fclose(file);
        exit(1);
    }

    int ch;
    while ((ch = fgetc(file)) != '\n' && ch != EOF) {
        
    }

    Table table = create_table(n);
    char buffer[MAX_LINE_LEN];

    for (int i = 0; i < n; i++) {
        if (fscanf(file, "%lf", &table.keys[i]) != 1) {
            printf("Ошибка: не удалось прочитать ключ для записи %d.\n", i + 1);
            free_table(&table);
            fclose(file);
            exit(1);
        }

        while ((ch = fgetc(file)) != '\n' && ch != EOF) {
            
        }

        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            printf("Ошибка: не удалось прочитать строку для записи %d.\n", i + 1);
            free_table(&table);
            fclose(file);
            exit(1);
        }

        remove_newline(buffer);
        table.values[i] = my_strdup(buffer);
    }

    fclose(file);
    return table;
}



void print_table(const Table *table, const char *title) {
    printf("\n%s\n", title);
    printf("--------------------------------------------------\n");
    printf("| %-12s | %-30s |\n", "KEY", "VALUE");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < table->size; i++) {
        printf("| %-12.6f | %-30s |\n", table->keys[i], table->values[i]);
    }

    printf("--------------------------------------------------\n");
}



void swap_items(Table *table, int i, int j) {
    double temp_key = table->keys[i];
    table->keys[i] = table->keys[j];
    table->keys[j] = temp_key;

    char *temp_value = table->values[i];
    table->values[i] = table->values[j];
    table->values[j] = temp_value;
}



int double_equal(double a, double b) {
    return fabs(a - b) < EPS;
}



void sift_down(Table *table, int root, int size) {
    while (1) {
        int left = 2 * root + 1;
        int right = 2 * root + 2;
        int largest = root;

        if (left < size && table->keys[left] > table->keys[largest]) {
            largest = left;
        }

        if (right < size && table->keys[right] > table->keys[largest]) {
            largest = right;
        }

        if (largest == root) {
            break;
        }

        swap_items(table, root, largest);
        root = largest;
    }
}

void heap_sort(Table *table) {
    int n = table->size;

    for (int i = n / 2 - 1; i >= 0; i--) {
        sift_down(table, i, n);
    }

    for (int end = n - 1; end > 0; end--) {
        swap_items(table, 0, end);
        sift_down(table, 0, end);
    }
}



void reverse_table(Table *table) {
    for (int i = 0; i < table->size / 2; i++) {
        swap_items(table, i, table->size - 1 - i);
    }
}



typedef struct {
    double key;
    char *value;
} Pair;

int compare_pairs(const void *a, const void *b) {
    const Pair *pa = (const Pair *)a;
    const Pair *pb = (const Pair *)b;

    if (pa->key < pb->key) return -1;
    if (pa->key > pb->key) return 1;
    return 0;
}

Table make_sorted_copy(const Table *src) {
    int n = src->size;
    Pair *pairs = (Pair *)malloc(n * sizeof(Pair));
    if (pairs == NULL) {
        printf("Ошибка: не удалось выделить память.\n");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        pairs[i].key = src->keys[i];
        pairs[i].value = src->values[i];
    }

    qsort(pairs, n, sizeof(Pair), compare_pairs);

    Table result = create_table(n);
    for (int i = 0; i < n; i++) {
        result.keys[i] = pairs[i].key;
        result.values[i] = my_strdup(pairs[i].value);
    }

    free(pairs);
    return result;
}



int binary_search(const Table *table, double target) {
    int left = 0;
    int right = table->size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (double_equal(table->keys[mid], target)) {
            return mid;
        } else if (table->keys[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}



void process_case(const Table *source, const char *case_name) {
    Table table = copy_table(source);

    print_table(&table, case_name);

    heap_sort(&table);
    print_table(&table, "После heap sort:");

    printf("\nВведите ключи для поиска в отсортированной таблице.\n");
    printf("Для завершения введите слово stop.\n");

    while (1) {
        char input[128];
        printf("Ключ: ");

        if (scanf("%127s", input) != 1) {
            printf("Ошибка ввода.\n");
            continue;
        }

        if (strcmp(input, "stop") == 0) {
            break;
        }

        char *endptr;
        double key = strtod(input, &endptr);

        if (*endptr != '\0') {
            printf("Некорректный вещественный ключ.\n");
            continue;
        }

        int index = binary_search(&table, key);
        if (index >= 0) {
            printf("Найден элемент: key = %.6f, value = %s\n",
                   table.keys[index], table.values[index]);
        } else {
            printf("Элемент с ключом %.6f не найден.\n", key);
        }
    }

    free_table(&table);
}



#ifndef UNIT_TEST
int main(void) {
    Table base = read_table_from_file("input.txt");

    Table sorted_case = make_sorted_copy(&base);
    Table reverse_case = copy_table(&sorted_case);
    reverse_table(&reverse_case);
    Table unsorted_case = copy_table(&base);

    printf("Лабораторная работа №2. Сортировка и поиск\n");
    printf("Номер в списке: 24\n");
    printf("Метод сортировки: №8 — древесная сортировка с просеиванием (heap sort)\n");
    printf("Структура таблицы: №6\n");
    printf("Тип ключа: вещественный (double)\n");
    printf("Хранение: отдельно\n");
    printf("Минимальное число элементов: %d\n", MIN_ELEMENTS);

    process_case(&sorted_case, "Случай 1: таблица уже упорядочена");
    process_case(&reverse_case, "Случай 2: таблица в обратном порядке");
    process_case(&unsorted_case, "Случай 3: таблица неупорядочена");

    free_table(&base);
    free_table(&sorted_case);
    free_table(&reverse_case);
    free_table(&unsorted_case);

    return 0;
}
#endif
