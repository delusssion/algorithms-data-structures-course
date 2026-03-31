# Отчёт по лабораторной работе

**ФИО:** Шалыгин Ярослав Алексеевич

**Группа:** М8О-102БВ-25

**Вариант:** 6

## Формулировка задания

Реализовать таблицу (ключ — вещественное число, значение — строка) с возможностью:
- Чтения таблицы из файла
- Сортировки таблицы методом пирамидальной сортировки (heap sort)
- Поиска элемента по ключу методом бинарного поиска

Программа демонстрирует работу алгоритмов на трёх случаях: уже упорядоченная таблица, таблица в обратном порядке, неупорядоченная таблица.

## Описание выполненной работы

### Структура проекта

Проект состоит из следующих файлов:

#### 1. `main.c` - Основная программа и реализация алгоритмов

Содержит структуру `Table`, все функции для работы с таблицей, реализацию сортировки и поиска.

Структура таблицы:
```c
typedef struct {
    double *keys;
    char **values;
    int size;
} Table;
```

**Основные функции:**
- `create_table()` — создание таблицы заданного размера
- `free_table()` — освобождение памяти таблицы
- `copy_table()` — копирование таблицы
- `read_table_from_file()` — чтение таблицы из файла
- `print_table()` — вывод таблицы на экран
- `swap_items()` — обмен элементов таблицы
- `sift_down()` — просеивание элемента вниз в куче
- `heap_sort()` — пирамидальная сортировка таблицы
- `binary_search()` — бинарный поиск по ключу
- `make_sorted_copy()` — создание отсортированной копии через qsort
- `reverse_table()` — реверс таблицы
- `process_case()` — обработка одного случая (сортировка + интерактивный поиск)

Реализация пирамидальной сортировки:
```c
void sift_down(Table *table, int root, int size) {
    while (1) {
        int left = 2 * root + 1;
        int right = 2 * root + 2;
        int largest = root;

        if (left < size && table->keys[left] > table->keys[largest])
            largest = left;
        if (right < size && table->keys[right] > table->keys[largest])
            largest = right;
        if (largest == root) break;

        swap_items(table, root, largest);
        root = largest;
    }
}

void heap_sort(Table *table) {
    int n = table->size;
    for (int i = n / 2 - 1; i >= 0; i--)
        sift_down(table, i, n);
    for (int end = n - 1; end > 0; end--) {
        swap_items(table, 0, end);
        sift_down(table, 0, end);
    }
}
```

Реализация бинарного поиска:
```c
int binary_search(const Table *table, double target) {
    int left = 0;
    int right = table->size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (double_equal(table->keys[mid], target))
            return mid;
        else if (table->keys[mid] < target)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}
```

#### 2. `tests.c` - Тесты

Тесты для проверки функций таблицы. Подключает `main.c` через `#include` с флагом `-DUNIT_TEST`.

Тест-кейсы:
- `test_read_table_from_file` — чтение таблицы из файла
- `test_heap_sort_unsorted` — сортировка неупорядоченной таблицы
- `test_heap_sort_sorted` — сортировка уже упорядоченной таблицы
- `test_binary_search_found` — поиск существующего элемента
- `test_binary_search_not_found` — поиск отсутствующего элемента

#### 3. `Makefile` - Сборка проекта

## Как запустить

**Запуск основной программы:**
```bash
make run
```

**Запуск тестов:**
```bash
make run_tests
```
