#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

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

// ==================== ТЕСТЫ ====================

// Тест 1: Проверка создания узла
void test_create_node() {
    printf("Test 1: Create node... ");

    Node *node = create_node("test");
    assert(node != NULL);
    assert(strcmp(node->value, "test") == 0);
    assert(node->left == NULL);
    assert(node->right == NULL);

    free(node);
    printf("PASSED\n");
}

// Тест 2: Проверка приоритета операторов
void test_operator_precedence() {
    printf("Test 2: Operator precedence... ");

    assert(get_precedence("+") == 1);
    assert(get_precedence("-") == 1);
    assert(get_precedence("*") == 2);
    assert(get_precedence("/") == 2);
    assert(get_precedence("^") == 3);

    printf("PASSED\n");
}

// Тест 3: Проверка парсинга простого выражения
void test_parse_simple_expression() {
    printf("Test 3: Parse simple expression (a + b)... ");

    Node *root = parse_expression("a + b");
    assert(root != NULL);
    assert(strcmp(root->value, "+") == 0);
    assert(root->left != NULL);
    assert(strcmp(root->left->value, "a") == 0);
    assert(root->right != NULL);
    assert(strcmp(root->right->value, "b") == 0);

    free_tree(root);
    printf("PASSED\n");
}

// Тест 4: Проверка трансформации выражения a ^ (b - c)
void test_transform_expression() {
    printf("Test 4: Transform expression (a ^ (b - c))... ");

    Node *root = parse_expression("a ^ (b - c)");
    assert(root != NULL);
    assert(strcmp(root->value, "^") == 0);

    // Применяем трансформацию
    root = transform(root);

    // После трансформации корень должен быть "/"
    assert(root != NULL);
    assert(strcmp(root->value, "/") == 0);

    // Левый ребенок должен быть "^" (a ^ b)
    assert(root->left != NULL);
    assert(strcmp(root->left->value, "^") == 0);

    // Правый ребенок должен быть "^" (a ^ c)
    assert(root->right != NULL);
    assert(strcmp(root->right->value, "^") == 0);

    free_tree(root);
    printf("PASSED\n");
}

// Тест 5: Проверка, что выражение без паттерна не меняется
void test_no_transform_without_pattern() {
    printf("Test 5: No transform without pattern (a + b)... ");

    Node *root = parse_expression("a + b");
    assert(root != NULL);
    assert(strcmp(root->value, "+") == 0);

    // Применяем трансформацию (должна остаться без изменений)
    root = transform(root);

    // Корень должен остаться "+"
    assert(root != NULL);
    assert(strcmp(root->value, "+") == 0);

    free_tree(root);
    printf("PASSED\n");
}

// Главная функция запуска всех тестов
int main() {
    printf("========================================\n");
    printf("Running unit tests for lab3\n");
    printf("========================================\n\n");

    test_create_node();
    test_operator_precedence();
    test_parse_simple_expression();
    test_transform_expression();
    test_no_transform_without_pattern();

    printf("\n========================================\n");
    printf("All tests PASSED!\n");
    printf("========================================\n");

    return 0;
}
