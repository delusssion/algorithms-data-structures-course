#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int key;
    struct Node* parent;
    struct Node* firstChild;
    struct Node* nextSibling;
} Node;

static Node* create_node(int key, Node* parent) {
    Node* n = (Node*)malloc(sizeof(Node));
    if (!n) {
        perror("malloc");
        exit(1);
    }
    n->key = key;
    n->parent = parent;
    n->firstChild = NULL;
    n->nextSibling = NULL;
    return n;
}

static Node* find_node(Node* root, int key) {
    if (!root) return NULL;
    if (root->key == key) return root;

    Node* child = root->firstChild;
    while (child) {
        Node* found = find_node(child, key);
        if (found) return found;
        child = child->nextSibling;
    }
    return NULL;
}

static int contains_key(Node* root, int key) {
    return find_node(root, key) != NULL;
}

static void add_child(Node* parent, Node* child) {
    if (!parent->firstChild) {
        parent->firstChild = child;
        return;
    }
    Node* cur = parent->firstChild;
    while (cur->nextSibling) cur = cur->nextSibling;
    cur->nextSibling = child;
}

static Node* insert_node(Node* root, int key, int parentValue, int* ok) {
    *ok = 0;

    if (!root) {
        root = create_node(key, NULL);
        *ok = 1;
        return root;
    }

    if (contains_key(root, key)) {
        return root;
    }

    Node* parent = find_node(root, parentValue);
    if (!parent) {
        return root;
    }

    Node* child = create_node(key, parent);
    add_child(parent, child);
    *ok = 1;
    return root;
}

static void print_tree_recursive(Node* node, int depth, int* isLastPath) {
    if (!node) return;

    if (depth == 0) {
        printf("%d\n", node->key);
    } else {
        for (int i = 0; i < depth - 1; i++) {
            if (isLastPath[i]) {
                printf("    ");
            } else {
                printf("|   ");
            }
        }
        if (isLastPath[depth - 1]) {
            printf("`-- ");
        } else {
            printf("|-- ");
        }
        printf("%d\n", node->key);
    }

    Node* child = node->firstChild;
    while (child) {
        Node* next = child->nextSibling;
        isLastPath[depth] = (next == NULL);
        print_tree_recursive(child, depth + 1, isLastPath);
        child = next;
    }
}

static void print_tree(Node* root, int depth) {
    (void)depth;
    int isLastPath[256] = {0};
    print_tree_recursive(root, 0, isLastPath);
}

static void free_subtree(Node* root) {
    if (!root) return;

    Node* child = root->firstChild;
    while (child) {
        Node* next = child->nextSibling;
        free_subtree(child);
        child = next;
    }

    free(root);
}

static void detach_from_parent(Node* node) {
    if (!node || !node->parent) return;

    Node* parent = node->parent;
    Node* cur = parent->firstChild;

    if (cur == node) {
        parent->firstChild = node->nextSibling;
        return;
    }

    while (cur && cur->nextSibling != node) {
        cur = cur->nextSibling;
    }
    if (cur && cur->nextSibling == node) {
        cur->nextSibling = node->nextSibling;
    }
}

static Node* delete_by_key(Node* root, int key, int* ok) {
    *ok = 0;
    if (!root) return NULL;

    Node* target = find_node(root, key);
    if (!target) return root;

    if (target == root) {
        free_subtree(root);
        *ok = 1;
        return NULL;
    }

    detach_from_parent(target);
    target->nextSibling = NULL;
    free_subtree(target);

    *ok = 1;
    return root;
}

static int count_leaves(Node* root) {
    if (!root) return 0;

    if (root->firstChild == NULL) {
        return 1;
    }

    int sum = 0;
    Node* child = root->firstChild;
    while (child) {
        sum += count_leaves(child);
        child = child->nextSibling;
    }
    return sum;
}

static void flush_input(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int main(void) {
    Node* root = NULL;

    for (;;) {
        printf("\n=== Меню (дерево общего вида) ===\n");
        printf("1) Добавить узел\n");
        printf("2) Показать дерево\n");
        printf("3) Удалить узел (удаляется поддерево)\n");
        printf("4) Посчитать число листьев\n");
        printf("0) Выход\n");
        printf("Выбор: ");

        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("Ошибка ввода.\n");
            flush_input();
            continue;
        }

        if (choice == 0) {
            break;
        } else if (choice == 1) {
            int key;
            printf("Введите значение нового узла (int): ");
            if (scanf("%d", &key) != 1) {
                printf("Ошибка ввода.\n");
                flush_input();
                continue;
            }

            if (!root) {
                int ok = 0;
                root = insert_node(root, key, 0, &ok);
                if (ok) printf("Создан корень %d.\n", key);
                else printf("Не удалось создать корень.\n");
            } else {
                int parentValue;
                printf("Введите значение родителя: ");
                if (scanf("%d", &parentValue) != 1) {
                    printf("Ошибка ввода.\n");
                    flush_input();
                    continue;
                }
                int ok = 0;
                root = insert_node(root, key, parentValue, &ok);
                if (ok) printf("Узел %d добавлен к родителю %d (как самый младший).\n", key, parentValue);
                else printf("Не удалось добавить: либо родитель не найден, либо ключ уже существует.\n");
            }

        } else if (choice == 2) {
            if (!root) {
                printf("(дерево пустое)\n");
            } else {
                printf("\nДерево:\n");
                print_tree(root, 0);
            }

        } else if (choice == 3) {
            int key;
            printf("Введите значение узла для удаления (удалится все поддерево): ");
            if (scanf("%d", &key) != 1) {
                printf("Ошибка ввода.\n");
                flush_input();
                continue;
            }
            int ok = 0;
            root = delete_by_key(root, key, &ok);
            if (ok) printf("Удаление выполнено.\n");
            else printf("Узел не найден.\n");

        } else if (choice == 4) {
            int leaves = count_leaves(root);
            printf("Число листьев: %d\n", leaves);

        } else {
            printf("Неизвестный пункт меню.\n");
        }
    }

    free_subtree(root);
    return 0;
}
