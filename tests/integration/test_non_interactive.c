#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assert.h"

#define TIMEOUT 5

void test_e_flag() {
    FILE *fp;
    char buffer[1024];
    int status;

    fp = popen("./expresso -e \"1 + 1\"", "r");
    ASSERT_TRUE(fp != NULL, "Failed to run expresso with -e flag");

    fgets(buffer, sizeof(buffer), fp);

    status = pclose(fp);
    ASSERT_TRUE(status != -1, "Failed to close popen stream");

    ASSERT_TRUE(strcmp(buffer, "2\n") == 0, "Incorrect output for -e flag");
}

int main() {
    printf("Running non-interactive integration tests...\n");
    test_e_flag();
    printf("All non-interactive integration tests passed!\n");
    return 0;
}
