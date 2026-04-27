#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define main avl_tree_main
#include "avl_tree.c"
#undef main

static int check_avl_property(AVLNode *node) {
    if (!node) return 1;
    int bf = avl_balance_factor(node);
    if (bf < -1 || bf > 1) return 0;
    return check_avl_property(node->left) && check_avl_property(node->right);
}

static void test_create_node(void) {
    AVLNode *node = avl_create_node("abc", 3.14);
    assert(node != NULL);
    assert(strcmp(node->key, "abc") == 0);
    assert(fabs(node->value - 3.14) < 1e-9);
    assert(node->left == NULL);
    assert(node->right == NULL);
    assert(node->height == 1);
    free(node);
}

static void test_height_empty(void) {
    assert(avl_height(NULL) == 0);
}

static void test_insert_single(void) {
    AVLNode *root = NULL;
    root = avl_insert(root, "apple", 1.0);
    assert(root != NULL);
    assert(strcmp(root->key, "apple") == 0);
    assert(fabs(root->value - 1.0) < 1e-9);
    assert(root->height == 1);
    avl_free(root);
}

static void test_insert_keeps_bst_property(void) {
    AVLNode *root = NULL;
    root = avl_insert(root, "b", 2.0);
    root = avl_insert(root, "a", 1.0);
    root = avl_insert(root, "c", 3.0);
    assert(strcmp(root->key, "b") == 0);
    assert(strcmp(root->left->key, "a") == 0);
    assert(strcmp(root->right->key, "c") == 0);
    avl_free(root);
}

static void test_avl_left_rotation(void) {
    AVLNode *root = NULL;
    root = avl_insert(root, "a", 1.0);
    root = avl_insert(root, "b", 2.0);
    root = avl_insert(root, "c", 3.0);
    assert(strcmp(root->key, "b") == 0);
    assert(strcmp(root->left->key, "a") == 0);
    assert(strcmp(root->right->key, "c") == 0);
    assert(avl_height(root) == 2);
    avl_free(root);
}

static void test_avl_right_rotation(void) {
    AVLNode *root = NULL;
    root = avl_insert(root, "c", 3.0);
    root = avl_insert(root, "b", 2.0);
    root = avl_insert(root, "a", 1.0);
    assert(strcmp(root->key, "b") == 0);
    assert(strcmp(root->left->key, "a") == 0);
    assert(strcmp(root->right->key, "c") == 0);
    assert(avl_height(root) == 2);
    avl_free(root);
}

static void test_avl_left_right_rotation(void) {
    AVLNode *root = NULL;
    root = avl_insert(root, "c", 3.0);
    root = avl_insert(root, "a", 1.0);
    root = avl_insert(root, "b", 2.0);
    assert(strcmp(root->key, "b") == 0);
    assert(strcmp(root->left->key, "a") == 0);
    assert(strcmp(root->right->key, "c") == 0);
    assert(avl_height(root) == 2);
    avl_free(root);
}

static void test_avl_right_left_rotation(void) {
    AVLNode *root = NULL;
    root = avl_insert(root, "a", 1.0);
    root = avl_insert(root, "c", 3.0);
    root = avl_insert(root, "b", 2.0);
    assert(strcmp(root->key, "b") == 0);
    assert(strcmp(root->left->key, "a") == 0);
    assert(strcmp(root->right->key, "c") == 0);
    assert(avl_height(root) == 2);
    avl_free(root);
}

static void test_search_found(void) {
    AVLNode *root = NULL;
    root = avl_insert(root, "abc", 42.0);
    root = avl_insert(root, "def", 7.0);
    AVLNode *found = avl_search(root, "abc");
    assert(found != NULL);
    assert(fabs(found->value - 42.0) < 1e-9);
    avl_free(root);
}

static void test_search_not_found(void) {
    AVLNode *root = NULL;
    root = avl_insert(root, "abc", 42.0);
    assert(avl_search(root, "xyz") == NULL);
    avl_free(root);
}

static void test_search_empty_tree(void) {
    assert(avl_search(NULL, "abc") == NULL);
}

static void test_delete_leaf(void) {
    AVLNode *root = NULL;
    root = avl_insert(root, "b", 2.0);
    root = avl_insert(root, "a", 1.0);
    root = avl_insert(root, "c", 3.0);
    int found = 0;
    root = avl_delete(root, "a", &found);
    assert(found == 1);
    assert(avl_search(root, "a") == NULL);
    assert(avl_search(root, "b") != NULL);
    assert(avl_search(root, "c") != NULL);
    avl_free(root);
}

static void test_delete_not_found(void) {
    AVLNode *root = NULL;
    root = avl_insert(root, "abc", 1.0);
    int found = 0;
    root = avl_delete(root, "xyz", &found);
    assert(found == 0);
    assert(root != NULL);
    avl_free(root);
}

static void test_delete_with_two_children(void) {
    AVLNode *root = NULL;
    root = avl_insert(root, "c", 3.0);
    root = avl_insert(root, "a", 1.0);
    root = avl_insert(root, "e", 5.0);
    root = avl_insert(root, "b", 2.0);
    root = avl_insert(root, "d", 4.0);
    int found = 0;
    root = avl_delete(root, "c", &found);
    assert(found == 1);
    assert(avl_search(root, "c") == NULL);
    assert(avl_search(root, "a") != NULL);
    assert(avl_search(root, "b") != NULL);
    assert(avl_search(root, "d") != NULL);
    assert(avl_search(root, "e") != NULL);
    avl_free(root);
}

static void test_avl_property_maintained(void) {
    AVLNode *root = NULL;
    const char *keys[] = {"f", "b", "g", "a", "d", "h", "c", "e", "i", "j"};
    for (int i = 0; i < 10; i++)
        root = avl_insert(root, keys[i], (double)i);
    assert(check_avl_property(root));
    for (int i = 0; i < 5; i++)
        root = avl_delete(root, keys[i], &(int){0});
    assert(check_avl_property(root));
    avl_free(root);
}

static void test_is_valid_key(void) {
    assert(is_valid_key("abc") == 1);
    assert(is_valid_key("A") == 1);
    assert(is_valid_key("abcdef") == 1);
    assert(is_valid_key("") == 0);
    assert(is_valid_key("abcdefg") == 0);
    assert(is_valid_key("ab1") == 0);
    assert(is_valid_key("ab c") == 0);
}

static void test_update_value(void) {
    AVLNode *root = NULL;
    root = avl_insert(root, "key", 1.0);
    root = avl_insert(root, "key", 2.0);
    AVLNode *found = avl_search(root, "key");
    assert(found != NULL);
    assert(fabs(found->value - 2.0) < 1e-9);
    avl_free(root);
}

int main(void) {
    test_create_node();
    printf("test_create_node passed\n");

    test_height_empty();
    printf("test_height_empty passed\n");

    test_insert_single();
    printf("test_insert_single passed\n");

    test_insert_keeps_bst_property();
    printf("test_insert_keeps_bst_property passed\n");

    test_avl_left_rotation();
    printf("test_avl_left_rotation passed\n");

    test_avl_right_rotation();
    printf("test_avl_right_rotation passed\n");

    test_avl_left_right_rotation();
    printf("test_avl_left_right_rotation passed\n");

    test_avl_right_left_rotation();
    printf("test_avl_right_left_rotation passed\n");

    test_search_found();
    printf("test_search_found passed\n");

    test_search_not_found();
    printf("test_search_not_found passed\n");

    test_search_empty_tree();
    printf("test_search_empty_tree passed\n");

    test_delete_leaf();
    printf("test_delete_leaf passed\n");

    test_delete_not_found();
    printf("test_delete_not_found passed\n");

    test_delete_with_two_children();
    printf("test_delete_with_two_children passed\n");

    test_avl_property_maintained();
    printf("test_avl_property_maintained passed\n");

    test_is_valid_key();
    printf("test_is_valid_key passed\n");

    test_update_value();
    printf("test_update_value passed\n");

    printf("All 17 tests passed.\n");
    return 0;
}
