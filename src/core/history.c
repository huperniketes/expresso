/*
 * Expresso
 * history.c
 *
 * Functions to manage a history of lines input by the user
 *
 * Expresso is an interactive command-line tool for evaluating expressions.
 * Expresso supports basic data types (integers, characters, strings),
 * literals, operators (arithmetic, logical, comparison, bitwise, string
 * manipulation), and nested expressions using C syntax (e.g., precedence,
 * parentheses). Key constraint: No mutable state—every input is a pure
 * expression that evaluates to an immutable value.
 *
 * ========================================================================
 *
 * © Copyright 2025 Alfonso Guerra
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  *  Neither the name of the copyright holder nor the names of
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
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
