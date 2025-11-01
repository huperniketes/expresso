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

void test_file_input() {
    FILE *fp;
    char buffer[1024];
    int status;
    const char* filename = "temp_input.txt";
    const char* expression = "2 + 3";

    // Create a temporary input file
    FILE* temp_file = fopen(filename, "w");
    ASSERT_TRUE(temp_file != NULL, "Failed to create temporary input file");
    fprintf(temp_file, "%s\n", expression);
    fclose(temp_file);

    // Set up alarm handler
    if (signal(SIGALRM, alarm_handler) == SIG_ERR) {
        ASSERT_TRUE(0, "Failed to set up signal handler");
    }

    if (setjmp(env) == 1) {
        // Timeout occurred
        remove(filename);
        ASSERT_TRUE(0, "File input test timed out after 5 seconds");
    }

    alarm(TIMEOUT_SECONDS);

    // Run expresso with file input redirection
    char command[256];
    sprintf(command, "./expresso < %s", filename);
    fp = popen(command, "r");
    ASSERT_TRUE(fp != NULL, "Failed to run expresso with file input");

    fgets(buffer, sizeof(buffer), fp);

    status = pclose(fp);
    ASSERT_TRUE(status != -1, "Failed to close popen stream");

    alarm(0); // Cancel the alarm

    // Clean up the temporary file
    remove(filename);

    char assert_msg[1024];
    snprintf(assert_msg, sizeof(assert_msg), "Incorrect output for file input. Expected \"5\\n\", got \"%s\"", buffer);
    ASSERT_TRUE(strcmp(buffer, "5\n") == 0, assert_msg);
}

int main() {
    printf("Running non-interactive integration tests...\n");
    test_e_flag();
    test_file_input();
    printf("All non-interactive integration tests passed!\n");
    return 0;
}
