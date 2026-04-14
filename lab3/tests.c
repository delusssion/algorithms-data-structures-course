#include <assert.h>
#include <stdio.h>
#include <string.h>

#define main expression_tree_main
#include "expression_tree.c"
#undef main

static void test_create_node(void) {
    Node *node = create_node("test");

    assert(node != NULL);
    assert(strcmp(node->value, "test") == 0);
    assert(node->left == NULL);
    assert(node->right == NULL);

    free(node);
}

static void test_operator_precedence(void) {
    assert(get_precedence("+") == 1);
    assert(get_precedence("-") == 1);
    assert(get_precedence("*") == 2);
    assert(get_precedence("/") == 2);
    assert(get_precedence("^") == 3);
}

static void test_parse_simple_expression(void) {
    Node *root = parse_expression("a + b");

    assert(root != NULL);
    assert(strcmp(root->value, "+") == 0);
    assert(root->left != NULL);
    assert(strcmp(root->left->value, "a") == 0);
    assert(root->right != NULL);
    assert(strcmp(root->right->value, "b") == 0);

    free_tree(root);
}

static void test_transform_expression(void) {
    Node *root = parse_expression("a ^ (b - c)");

    assert(root != NULL);
    assert(strcmp(root->value, "^") == 0);

    root = transform(root);

    assert(root != NULL);
    assert(strcmp(root->value, "/") == 0);
    assert(root->left != NULL);
    assert(strcmp(root->left->value, "^") == 0);
    assert(root->right != NULL);
    assert(strcmp(root->right->value, "^") == 0);

    free_tree(root);
}

static void test_no_transform_without_pattern(void) {
    Node *root = parse_expression("a + b");

    assert(root != NULL);
    assert(strcmp(root->value, "+") == 0);

    root = transform(root);

    assert(root != NULL);
    assert(strcmp(root->value, "+") == 0);

    free_tree(root);
}

int main(void) {
    test_create_node();
    printf("test_create_node passed\n");

    test_operator_precedence();
    printf("test_operator_precedence passed\n");

    test_parse_simple_expression();
    printf("test_parse_simple_expression passed\n");

    test_transform_expression();
    printf("test_transform_expression passed\n");

    test_no_transform_without_pattern();
    printf("test_no_transform_without_pattern passed\n");

    printf("All 5 tests passed.\n");
    return 0;
}
