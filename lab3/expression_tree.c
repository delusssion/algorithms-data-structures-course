#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKENS 100
#define MAX_TOKEN_LEN 50
#define MAX_EXPR_LEN 500

// Типы токенов
typedef enum {
    TOKEN_NUMBER,
    TOKEN_VARIABLE,
    TOKEN_OPERATOR,
    TOKEN_LPAREN,
    TOKEN_RPAREN
} TokenType;

// Структура токена
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LEN];
} Token;

// Узел дерева выражений
typedef struct Node {
    char value[MAX_TOKEN_LEN];
    struct Node *left;
    struct Node *right;
} Node;

// Дерево выражений
typedef struct {
    Node *root;
} ExpressionTree;

// Приоритеты операторов
int get_precedence(const char *op) {
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0) return 2;
    if (strcmp(op, "^") == 0) return 3;
    return 0;
}

// Ассоциативность операторов (1 - левая, 0 - правая)
int is_left_associative(const char *op) {
    if (strcmp(op, "^") == 0) return 0;
    return 1;
}

// Проверка, является ли строка числом
int is_number(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

// Проверка, является ли строка переменной
int is_variable(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalpha(str[i]) && !isdigit(str[i])) return 0;
    }
    return isalpha(str[0]);
}

// Создание нового узла
Node* create_node(const char *value) {
    Node *node = (Node*)malloc(sizeof(Node));
    strncpy(node->value, value, MAX_TOKEN_LEN - 1);
    node->value[MAX_TOKEN_LEN - 1] = '\0';
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Копирование узла (глубокая копия)
Node* copy_node(Node *node) {
    if (node == NULL) return NULL;

    Node *new_node = create_node(node->value);
    new_node->left = copy_node(node->left);
    new_node->right = copy_node(node->right);
    return new_node;
}

// Освобождение памяти дерева
void free_tree(Node *node) {
    if (node == NULL) return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

// Разбиение строки на токены
int tokenize(const char *expression, Token tokens[]) {
    int token_count = 0;
    int i = 0;
    int len = strlen(expression);

    while (i < len && token_count < MAX_TOKENS) {
        // Пропускаем пробелы
        while (i < len && expression[i] == ' ') i++;
        if (i >= len) break;

        char ch = expression[i];

        // Число
        if (isdigit(ch)) {
            int j = 0;
            while (i < len && isdigit(expression[i]) && j < MAX_TOKEN_LEN - 1) {
                tokens[token_count].value[j++] = expression[i++];
            }
            tokens[token_count].value[j] = '\0';
            tokens[token_count].type = TOKEN_NUMBER;
            token_count++;
        }
        // Переменная
        else if (isalpha(ch)) {
            int j = 0;
            while (i < len && (isalnum(expression[i]) || expression[i] == '_') && j < MAX_TOKEN_LEN - 1) {
                tokens[token_count].value[j++] = expression[i++];
            }
            tokens[token_count].value[j] = '\0';
            tokens[token_count].type = TOKEN_VARIABLE;
            token_count++;
        }
        // Оператор или скобка
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^' || ch == '(' || ch == ')') {
            tokens[token_count].value[0] = ch;
            tokens[token_count].value[1] = '\0';

            if (ch == '(') tokens[token_count].type = TOKEN_LPAREN;
            else if (ch == ')') tokens[token_count].type = TOKEN_RPAREN;
            else tokens[token_count].type = TOKEN_OPERATOR;

            token_count++;
            i++;
        }
        else {
            printf("Ошибка: неизвестный символ '%c'\n", ch);
            return -1;
        }
    }

    return token_count;
}

// Применение оператора к операндам
void apply_operator(Node *operand_stack[], int *operand_top,
                    char operator_stack[], int *operator_top) {
    if (*operator_top < 0 || *operand_top < 1) {
        printf("Ошибка: недостаточно операндов\n");
        return;
    }

    char op_str[2] = {operator_stack[(*operator_top)--], '\0'};
    Node *right = operand_stack[(*operand_top)--];
    Node *left = operand_stack[(*operand_top)--];

    Node *op_node = create_node(op_str);
    op_node->left = left;
    op_node->right = right;

    operand_stack[++(*operand_top)] = op_node;
}

// Парсинг выражения в дерево
Node* parse_expression(const char *expression) {
    Token tokens[MAX_TOKENS];
    int token_count = tokenize(expression, tokens);

    if (token_count <= 0) {
        printf("Ошибка: пустое или некорректное выражение\n");
        return NULL;
    }

    Node *operand_stack[MAX_TOKENS];
    char operator_stack[MAX_TOKENS];
    int operand_top = -1;
    int operator_top = -1;

    for (int i = 0; i < token_count; i++) {
        Token token = tokens[i];

        if (token.type == TOKEN_NUMBER || token.type == TOKEN_VARIABLE) {
            operand_stack[++operand_top] = create_node(token.value);
        }
        else if (token.type == TOKEN_LPAREN) {
            operator_stack[++operator_top] = '(';
        }
        else if (token.type == TOKEN_RPAREN) {
            while (operator_top >= 0 && operator_stack[operator_top] != '(') {
                apply_operator(operand_stack, &operand_top, operator_stack, &operator_top);
            }
            if (operator_top >= 0 && operator_stack[operator_top] == '(') {
                operator_top--;
            } else {
                printf("Ошибка: несбалансированные скобки\n");
                return NULL;
            }
        }
        else if (token.type == TOKEN_OPERATOR) {
            char current_op = token.value[0];

            while (operator_top >= 0 && operator_stack[operator_top] != '(') {
                char stack_op_str[2] = {operator_stack[operator_top], '\0'};
                char current_op_str[2] = {current_op, '\0'};
                int stack_prec = get_precedence(stack_op_str);
                int current_prec = get_precedence(current_op_str);

                if (stack_prec > current_prec ||
                    (stack_prec == current_prec && is_left_associative(current_op_str))) {
                    apply_operator(operand_stack, &operand_top, operator_stack, &operator_top);
                } else {
                    break;
                }
            }

            operator_stack[++operator_top] = current_op;
        }
    }

    while (operator_top >= 0) {
        if (operator_stack[operator_top] == '(') {
            printf("Ошибка: несбалансированные скобки\n");
            return NULL;
        }
        apply_operator(operand_stack, &operand_top, operator_stack, &operator_top);
    }

    if (operand_top != 0) {
        printf("Ошибка: некорректное выражение\n");
        return NULL;
    }

    return operand_stack[0];
}

// Проверка, является ли узел листом
int is_leaf(Node *node) {
    return node != NULL && node->left == NULL && node->right == NULL;
}

// Преобразование дерева в инфиксную запись
void to_infix_helper(Node *node, const char *parent_op, int is_right_child, char *result) {
    if (node == NULL) return;

    if (is_leaf(node)) {
        strcat(result, node->value);
        return;
    }

    int needs_parens = 0;
    if (strlen(parent_op) > 0) {
        int current_prec = get_precedence(node->value);
        int parent_prec = get_precedence((char*)parent_op);

        if (current_prec < parent_prec) {
            needs_parens = 1;
        } else if (current_prec == parent_prec) {
            if (is_right_child && is_left_associative(node->value)) {
                needs_parens = 1;
            }
        }
    }

    if (needs_parens) {
        strcat(result, "(");
    }

    to_infix_helper(node->left, node->value, 0, result);

    strcat(result, " ");
    strcat(result, node->value);
    strcat(result, " ");

    to_infix_helper(node->right, node->value, 1, result);

    if (needs_parens) {
        strcat(result, ")");
    }
}

char* to_infix(Node *node) {
    static char result[MAX_EXPR_LEN];
    result[0] = '\0';

    if (node == NULL) return result;

    to_infix_helper(node, "", 0, result);

    // Удаляем ведущий пробел если есть
    if (result[0] == ' ') {
        memmove(result, result + 1, strlen(result));
    }

    return result;
}

// Печать дерева
void print_tree_helper(Node *node, int level, const char *prefix) {
    if (node == NULL) return;

    for (int i = 0; i < level; i++) {
        printf("    ");
    }
    printf("%s%s\n", prefix, node->value);

    if (node->left != NULL || node->right != NULL) {
        if (node->left) {
            print_tree_helper(node->left, level + 1, "L--- ");
        } else {
            for (int i = 0; i <= level; i++) printf("    ");
            printf("L--- None\n");
        }

        if (node->right) {
            print_tree_helper(node->right, level + 1, "R--- ");
        } else {
            for (int i = 0; i <= level; i++) printf("    ");
            printf("R--- None\n");
        }
    }
}

void print_tree(Node *node) {
    printf("Root: %s\n", node->value);
    if (node->left != NULL || node->right != NULL) {
        if (node->left) {
            print_tree_helper(node->left, 1, "L--- ");
        } else {
            printf("    L--- None\n");
        }

        if (node->right) {
            print_tree_helper(node->right, 1, "R--- ");
        } else {
            printf("    R--- None\n");
        }
    }
}

// Трансформация дерева: a ^ (b - c) → a ^ b / a ^ c
Node* transform(Node *node) {
    if (node == NULL) return NULL;

    // Рекурсивно преобразуем поддеревья
    node->left = transform(node->left);
    node->right = transform(node->right);

    // Проверяем паттерн: ^ с правым дочерним элементом (-)
    if (strcmp(node->value, "^") == 0 &&
        node->right != NULL &&
        strcmp(node->right->value, "-") == 0 &&
        node->left != NULL) {

        Node *a = node->left;
        Node *b = node->right->left;
        Node *c = node->right->right;

        // Создаем a ^ b
        Node *pow_ab = create_node("^");
        pow_ab->left = copy_node(a);
        pow_ab->right = copy_node(b);

        // Создаем a ^ c
        Node *pow_ac = create_node("^");
        pow_ac->left = copy_node(a);
        pow_ac->right = copy_node(c);

        // Создаем деление: (a ^ b) / (a ^ c)
        Node *div = create_node("/");
        div->left = pow_ab;
        div->right = pow_ac;

        // Освобождаем старое поддерево
        free_tree(node);

        return div;
    }

    return node;
}

// Тестовые примеры
void run_tests() {
    const char *test_cases[] = {
        "a ^ (b - c)",
        "2 ^ (3 - 1)",
        "x ^ (y - z) + 5",
        "(a + b) ^ (c - d)",
        "a ^ b - c",
        "a ^ (b + c)"
    };

    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);

    printf("============================================================\n");
    printf("Лабораторная работа 3. Вариант 26\n");
    printf("Преобразование: a ^ (b - c) → a ^ b / a ^ c\n");
    printf("============================================================\n\n");

    for (int i = 0; i < num_tests; i++) {
        printf("Исходное выражение: %s\n", test_cases[i]);

        Node *root = parse_expression(test_cases[i]);
        if (root == NULL) {
            printf("Ошибка при парсинге выражения\n");
            printf("------------------------------------------------------------\n\n");
            continue;
        }

        printf("\nДерево до преобразования:\n");
        print_tree(root);

        root = transform(root);

        printf("\nДерево после преобразования:\n");
        print_tree(root);

        char *result = to_infix(root);
        printf("\nРезультат: %s\n", result);

        printf("------------------------------------------------------------\n\n");

        free_tree(root);
    }
}

int main() {
    run_tests();
    return 0;
}
