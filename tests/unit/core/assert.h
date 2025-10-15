#ifndef CUSTOM_ASSERT_H
#define CUSTOM_ASSERT_H

#include <stdio.h>
#include <stdlib.h>

#define ASSERT_TRUE(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "Assertion failed: %s (%s:%d) - %s\n", \
                    #condition, __FILE__, __LINE__, message); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

#define ASSERT_FALSE(condition, message) \
    ASSERT_TRUE(!(condition), message)

#define ASSERT_EQ(expected, actual, message) \
    do { \
        if (!((expected) == (actual))) { \
            fprintf(stderr, "Assertion failed: Expected %d, got %d (%s:%d) - %s\n", \
                    (int)(expected), (int)(actual), __FILE__, __LINE__, message); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

// Add more as needed, e.g., ASSERT_NE, ASSERT_GT, etc.

#endif // CUSTOM_ASSERT_H

