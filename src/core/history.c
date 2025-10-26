#include "history.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // For fprintf

History* history_create(size_t capacity) {
    if (capacity == 0) {
        fprintf(stderr, "Error: History capacity cannot be 0.\n");
        return NULL;
    }

    History* h = (History*)malloc(sizeof(History));
    if (!h) {
        fprintf(stderr, "Fatal Error: Memory allocation failed for history.\n");
        exit(EXIT_FAILURE);
    }

    h->entries = (char**)malloc(sizeof(char*) * capacity);
    if (!h->entries) {
        fprintf(stderr, "Fatal Error: Memory allocation failed for history entries.\n");
        free(h);
        exit(EXIT_FAILURE);
    }

    h->capacity = capacity;
    h->size = 0;
    h->head = 0;
    h->tail = 0;

    for (size_t i = 0; i < capacity; ++i) {
        h->entries[i] = NULL;
    }

    return h;
}

void history_destroy(const History* h) {
    if (!h) return;

    for (size_t i = 0; i < h->capacity; ++i) {
        free(h->entries[i]);
    }
    free(h->entries);
    free(h);
}

void history_add(History* const h, const char* entry) {
    if (!h || !entry) return;

    // Strip trailing whitespace as per NFR-007
    char* trimmed_entry = strdup(entry);
    if (!trimmed_entry) {
        fprintf(stderr, "Fatal Error: Memory allocation failed for history entry.\n");
        exit(EXIT_FAILURE);
    }
    size_t len = strlen(trimmed_entry);
    while (len > 0 && (trimmed_entry[len - 1] == ' ' || trimmed_entry[len - 1] == '\t' || trimmed_entry[len - 1] == '\n' || trimmed_entry[len - 1] == '\r')) {
        trimmed_entry[--len] = '\0';
    }

    // If the trimmed entry is empty, do not add it to history
    if (len == 0) {
        free(trimmed_entry);
        return;
    }

    // Check for uniqueness (case-sensitive) as per NFR-007
    for (size_t i = 0; i < h->size; ++i) {
        if (strcmp(history_get(h, i), trimmed_entry) == 0) {
            free(trimmed_entry);
            return; // Entry already exists, do not add
        }
    }

    // Free old entry if overwriting
    if (h->entries[h->tail] != NULL) {
        free(h->entries[h->tail]);
    }

    h->entries[h->tail] = trimmed_entry;

    if (h->size < h->capacity) {
        h->size++;
    } else {
        h->head = (h->head + 1) % h->capacity; // Move head forward
    }
    h->tail = (h->tail + 1) % h->capacity;
}

const char* history_get(const History* h, size_t index) {
    if (!h || index >= h->size) {
        return NULL;
    }
    return h->entries[(h->head + index) % h->capacity];
}

size_t history_size(const History* h) {
    return h->size;
}

void history_clear(History* const h) {
    if (!h) return;

    for (size_t i = 0; i < h->capacity; ++i) {
        free(h->entries[i]);
        h->entries[i] = NULL;
    }
    h->size = 0;
    h->head = 0;
    h->tail = 0;
}
