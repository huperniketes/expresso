#include "assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

static jmp_buf env;
static void alarm_handler(int signo) {
    longjmp(env, 1);
}

#define TIMEOUT_SECONDS 5

void test_e_flag() {
    FILE *fp;
    char buffer[1024];
    int status;

    // Set up alarm handler
    if (signal(SIGALRM, alarm_handler) == SIG_ERR) {
        ASSERT_TRUE(0, "Failed to set up signal handler");
    }

    if (setjmp(env) == 1) {
        // Timeout occurred
        ASSERT_TRUE(0, "Test timed out after 5 seconds");
    }

    alarm(TIMEOUT_SECONDS);

    fp = popen("./expresso -e \"1 + 1\"", "r");
    ASSERT_TRUE(fp != NULL, "Failed to run expresso with -e flag");

    fgets(buffer, sizeof(buffer), fp);

    status = pclose(fp);
    ASSERT_TRUE(status != -1, "Failed to close popen stream");

    alarm(0); // Cancel the alarm

    ASSERT_TRUE(strcmp(buffer, "2\n") == 0, "Incorrect output for -e flag");
}

int main() {
    printf("Running non-interactive integration tests...\n");
    test_e_flag();
    printf("All non-interactive integration tests passed!\n");
    return 0;
}
