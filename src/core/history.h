#ifndef EXPRESO_HISTORY_H
#define EXPRESO_HISTORY_H

#include <stddef.h> // For size_t

typedef struct {
    char** entries;
    size_t capacity;
    size_t size;
    size_t head; // Index of the oldest entry
    size_t tail; // Index of the newest entry
} History;

// Create a new history buffer with a given capacity
History* history_create(size_t capacity);

// Destroy the history buffer and free all allocated memory
void history_destroy(History* h);

// Add an entry to the history buffer
void history_add(History* h, const char* entry);

// Get an entry from the history buffer by its relative index (0 is oldest, size-1 is newest)
// Returns NULL if index is out of bounds or history is empty
const char* history_get(History* h, size_t index);

// Get the current number of entries in the history
size_t history_size(History* h);

// Clear all entries from the history
void history_clear(History* h);

#endif // EXPRESO_HISTORY_H
