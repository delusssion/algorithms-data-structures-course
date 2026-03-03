#include <assert.h>
#include <stdio.h>

#define main tree_menu_main
#include "main.c"
#undef main

static void test_create_root(void) {
    Node* root = NULL;
    int ok = 0;

    root = insert_node(root, 10, 0, &ok);

    assert(ok == 1);
    assert(root != NULL);
    assert(root->key == 10);
    assert(root->parent == NULL);
    assert(root->firstChild == NULL);
    free_subtree(root);
}

static void test_insert_child(void) {
    Node* root = NULL;
    int ok = 0;

    root = insert_node(root, 10, 0, &ok);
    assert(ok == 1);

    root = insert_node(root, 20, 10, &ok);
    assert(ok == 1);

    Node* child = find_node(root, 20);
    assert(child != NULL);
    assert(child->parent == root);
    assert(root->firstChild == child);
    free_subtree(root);
}

static void test_reject_duplicate_key(void) {
    Node* root = NULL;
    int ok = 0;

    root = insert_node(root, 10, 0, &ok);
    assert(ok == 1);

    root = insert_node(root, 20, 10, &ok);
    assert(ok == 1);

    root = insert_node(root, 20, 10, &ok);
    assert(ok == 0);

    assert(root->firstChild != NULL);
    assert(root->firstChild->nextSibling == NULL);
    free_subtree(root);
}

static void test_delete_subtree(void) {
    Node* root = NULL;
    int ok = 0;

    root = insert_node(root, 10, 0, &ok);
    root = insert_node(root, 20, 10, &ok);
    root = insert_node(root, 30, 10, &ok);
    root = insert_node(root, 40, 20, &ok);

    root = delete_by_key(root, 20, &ok);
    assert(ok == 1);
    assert(find_node(root, 20) == NULL);
    assert(find_node(root, 40) == NULL);
    assert(find_node(root, 30) != NULL);
    free_subtree(root);
}

static void test_count_leaves(void) {
    Node* root = NULL;
    int ok = 0;

    root = insert_node(root, 10, 0, &ok);
    root = insert_node(root, 20, 10, &ok);
    root = insert_node(root, 30, 10, &ok);
    root = insert_node(root, 40, 20, &ok);

    assert(count_leaves(root) == 2);
    free_subtree(root);
}

int main(void) {
    test_create_root();
    printf("test_create_root passed\n");

    test_insert_child();
    printf("test_insert_child passed\n");

    test_reject_duplicate_key();
    printf("test_reject_duplicate_key passed\n");

    test_delete_subtree();
    printf("test_delete_subtree passed\n");

    test_count_leaves();
    printf("test_count_leaves passed\n");

    printf("All 5 tests passed.\n");
    return 0;
}
