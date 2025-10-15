#include "assert.h"
#include "history.h" // Assuming history.h will be created in T015
#include <stdio.h>
#include <string.h>

void test_history_add_and_recall() {
    History* h = history_create(3); // Max 3 items
    history_add(h, "first");
    history_add(h, "second");
    history_add(h, "third");

    ASSERT_TRUE(strcmp(history_get(h, 0), "first") == 0, "Recall first item failed");
    ASSERT_TRUE(strcmp(history_get(h, 1), "second") == 0, "Recall second item failed");
    ASSERT_TRUE(strcmp(history_get(h, 2), "third") == 0, "Recall third item failed");

    history_destroy(h);
}

void test_history_circular_buffer() {
    History* h = history_create(2); // Max 2 items
    history_add(h, "one");
    history_add(h, "two");
    history_add(h, "three"); // "one" should be overwritten

    ASSERT_TRUE(strcmp(history_get(h, 0), "two") == 0, "Circular buffer failed (item 0)");
    ASSERT_TRUE(strcmp(history_get(h, 1), "three") == 0, "Circular buffer failed (item 1)");

    history_destroy(h);
}

void test_history_clear() {
    History* h = history_create(5);
    history_add(h, "item1");
    history_add(h, "item2");
    history_clear(h);

    ASSERT_EQ(0, history_size(h), "History size after clear should be 0");
    ASSERT_TRUE(history_get(h, 0) == NULL, "History should be empty after clear");

    history_destroy(h);
}

int main() {
    printf("Running History unit tests...\n");
    test_history_add_and_recall();
    test_history_circular_buffer();
    test_history_clear();
    printf("All History unit tests passed!\n");
    return 0;
}
