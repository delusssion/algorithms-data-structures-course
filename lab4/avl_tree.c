#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_KEY_LEN 7

typedef struct AVLNode {
    char key[MAX_KEY_LEN];
    double value;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;

int avl_height(AVLNode *node) {
    return node ? node->height : 0;
}

int avl_balance_factor(AVLNode *node) {
    return node ? avl_height(node->left) - avl_height(node->right) : 0;
}

static void avl_update_height(AVLNode *node) {
    if (!node) return;
    int lh = avl_height(node->left);
    int rh = avl_height(node->right);
    node->height = (lh > rh ? lh : rh) + 1;
}

AVLNode *avl_create_node(const char *key, double value) {
    AVLNode *node = (AVLNode *)malloc(sizeof(AVLNode));
    strncpy(node->key, key, MAX_KEY_LEN - 1);
    node->key[MAX_KEY_LEN - 1] = '\0';
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

static AVLNode *avl_rotate_right(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;
    x->right = y;
    y->left = T2;
    avl_update_height(y);
    avl_update_height(x);
    return x;
}

static AVLNode *avl_rotate_left(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;
    y->left = x;
    x->right = T2;
    avl_update_height(x);
    avl_update_height(y);
    return y;
}

static AVLNode *avl_do_balance(AVLNode *node) {
    avl_update_height(node);
    int bf = avl_balance_factor(node);
    if (bf > 1) {
        if (avl_balance_factor(node->left) < 0)
            node->left = avl_rotate_left(node->left);
        return avl_rotate_right(node);
    }
    if (bf < -1) {
        if (avl_balance_factor(node->right) > 0)
            node->right = avl_rotate_right(node->right);
        return avl_rotate_left(node);
    }
    return node;
}

AVLNode *avl_insert(AVLNode *node, const char *key, double value) {
    if (!node) return avl_create_node(key, value);
    int cmp = strcmp(key, node->key);
    if (cmp < 0)
        node->left = avl_insert(node->left, key, value);
    else if (cmp > 0)
        node->right = avl_insert(node->right, key, value);
    else
        node->value = value;
    return avl_do_balance(node);
}

static AVLNode *avl_find_min(AVLNode *node) {
    while (node->left) node = node->left;
    return node;
}

AVLNode *avl_delete(AVLNode *node, const char *key, int *found) {
    if (!node) { *found = 0; return NULL; }
    int cmp = strcmp(key, node->key);
    if (cmp < 0)
        node->left = avl_delete(node->left, key, found);
    else if (cmp > 0)
        node->right = avl_delete(node->right, key, found);
    else {
        *found = 1;
        if (!node->left || !node->right) {
            AVLNode *temp = node->left ? node->left : node->right;
            free(node);
            return temp;
        }
        AVLNode *min_r = avl_find_min(node->right);
        strncpy(node->key, min_r->key, MAX_KEY_LEN);
        node->value = min_r->value;
        int dummy = 0;
        node->right = avl_delete(node->right, min_r->key, &dummy);
    }
    return avl_do_balance(node);
}

AVLNode *avl_search(AVLNode *node, const char *key) {
    if (!node) return NULL;
    int cmp = strcmp(key, node->key);
    if (cmp == 0) return node;
    return cmp < 0 ? avl_search(node->left, key) : avl_search(node->right, key);
}

void avl_free(AVLNode *node) {
    if (!node) return;
    avl_free(node->left);
    avl_free(node->right);
    free(node);
}

static void print_subtree(AVLNode *node, int level, const char *prefix, FILE *out) {
    if (!node) return;
    for (int i = 0; i < level; i++) fprintf(out, "    ");
    fprintf(out, "%s%s: %.6g\n", prefix, node->key, node->value);
    if (node->left || node->right) {
        if (node->left)
            print_subtree(node->left, level + 1, "L--- ", out);
        else {
            for (int i = 0; i <= level; i++) fprintf(out, "    ");
            fprintf(out, "L--- (empty)\n");
        }
        if (node->right)
            print_subtree(node->right, level + 1, "R--- ", out);
        else {
            for (int i = 0; i <= level; i++) fprintf(out, "    ");
            fprintf(out, "R--- (empty)\n");
        }
    }
}

void avl_print(AVLNode *root, FILE *out) {
    if (!root) {
        fprintf(out, "(empty tree)\n");
        return;
    }
    fprintf(out, "Root: %s: %.6g\n", root->key, root->value);
    if (root->left || root->right) {
        if (root->left)
            print_subtree(root->left, 1, "L--- ", out);
        else
            fprintf(out, "    L--- (empty)\n");
        if (root->right)
            print_subtree(root->right, 1, "R--- ", out);
        else
            fprintf(out, "    R--- (empty)\n");
    }
}

int is_valid_key(const char *key) {
    int len = strlen(key);
    if (len == 0 || len > 6) return 0;
    for (int i = 0; i < len; i++) {
        if (!isalpha((unsigned char)key[i])) return 0;
    }
    return 1;
}

static void process_line(const char *line, AVLNode **root, FILE *out, FILE *log_file) {
    char cmd[256];
    strncpy(cmd, line, sizeof(cmd) - 1);
    cmd[sizeof(cmd) - 1] = '\0';
    int len = strlen(cmd);
    while (len > 0 && (cmd[len-1] == '\n' || cmd[len-1] == '\r' || cmd[len-1] == ' '))
        cmd[--len] = '\0';
    if (len == 0) return;

    if (log_file) fprintf(log_file, "%s\n", cmd);

    int op;
    char key[32];
    double value;

    if (sscanf(cmd, "%d", &op) != 1) {
        fprintf(out, "Ошибка: неверный формат команды\n");
        if (log_file) fprintf(log_file, "Ошибка: неверный формат команды\n\n");
        return;
    }

    switch (op) {
        case 1: {
            if (sscanf(cmd, "%d %31s %lf", &op, key, &value) != 3) {
                fprintf(out, "Ошибка: формат: 1 <ключ> <значение>\n");
                if (log_file) fprintf(log_file, "Ошибка: формат: 1 <ключ> <значение>\n\n");
                return;
            }
            if (!is_valid_key(key)) {
                fprintf(out, "Ошибка: ключ '%s' — должно быть 1-6 латинских букв\n", key);
                if (log_file) fprintf(log_file, "Ошибка: ключ '%s' — должно быть 1-6 латинских букв\n\n", key);
                return;
            }
            int updating = avl_search(*root, key) != NULL;
            *root = avl_insert(*root, key, value);
            if (updating) {
                fprintf(out, "Обновлён ключ '%s': %.6g\n", key, value);
                if (log_file) fprintf(log_file, "Обновлён ключ '%s': %.6g\n\n", key, value);
            } else {
                fprintf(out, "Добавлен ключ '%s': %.6g\n", key, value);
                if (log_file) fprintf(log_file, "Добавлен ключ '%s': %.6g\n\n", key, value);
            }
            break;
        }
        case 2: {
            if (sscanf(cmd, "%d %31s", &op, key) != 2) {
                fprintf(out, "Ошибка: формат: 2 <ключ>\n");
                if (log_file) fprintf(log_file, "Ошибка: формат: 2 <ключ>\n\n");
                return;
            }
            int found = 0;
            *root = avl_delete(*root, key, &found);
            if (found) {
                fprintf(out, "Удалён ключ '%s'\n", key);
                if (log_file) fprintf(log_file, "Удалён ключ '%s'\n\n", key);
            } else {
                fprintf(out, "Ключ '%s' не найден\n", key);
                if (log_file) fprintf(log_file, "Ключ '%s' не найден\n\n", key);
            }
            break;
        }
        case 3: {
            avl_print(*root, out);
            if (log_file) { avl_print(*root, log_file); fprintf(log_file, "\n"); }
            break;
        }
        case 4: {
            if (sscanf(cmd, "%d %31s", &op, key) != 2) {
                fprintf(out, "Ошибка: формат: 4 <ключ>\n");
                if (log_file) fprintf(log_file, "Ошибка: формат: 4 <ключ>\n\n");
                return;
            }
            AVLNode *found_node = avl_search(*root, key);
            if (found_node) {
                fprintf(out, "%s = %.6g\n", key, found_node->value);
                if (log_file) fprintf(log_file, "%s = %.6g\n\n", key, found_node->value);
            } else {
                fprintf(out, "Ключ '%s' не найден\n", key);
                if (log_file) fprintf(log_file, "Ключ '%s' не найден\n\n", key);
            }
            break;
        }
        default:
            fprintf(out, "Неизвестная операция: %d\n", op);
            if (log_file) fprintf(log_file, "Неизвестная операция: %d\n\n", op);
    }
}

int main(int argc, char *argv[]) {
    AVLNode *root = NULL;

    if (argc >= 2) {
        FILE *in = fopen(argv[1], "r");
        if (!in) {
            fprintf(stderr, "Ошибка: не удалось открыть '%s'\n", argv[1]);
            return 1;
        }
        FILE *log_file = NULL;
        if (argc >= 3) {
            log_file = fopen(argv[2], "w");
            if (!log_file) {
                fprintf(stderr, "Ошибка: не удалось открыть '%s' для записи\n", argv[2]);
                fclose(in);
                return 1;
            }
        }
        char line[256];
        while (fgets(line, sizeof(line), in))
            process_line(line, &root, stdout, log_file);
        fclose(in);
        if (log_file) fclose(log_file);
    } else {
        printf("AVL-дерево. Операции: 1 key val | 2 key | 3 | 4 key\n");
        printf("Для выхода: Ctrl+D\n");
        char line[256];
        while (fgets(line, sizeof(line), stdin))
            process_line(line, &root, stdout, NULL);
    }

    avl_free(root);
    return 0;
}
