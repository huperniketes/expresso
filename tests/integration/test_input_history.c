#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For pipe, fork, dup2
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024
#define TIMEOUT 15

pid_t child_pid;

void handle_alarm(int sig) {
    printf("Test timed out after %d seconds.\n", TIMEOUT);
    kill(child_pid, SIGKILL);
    exit(EXIT_FAILURE);
}

int main() {
    int to_child_pipe[2];
    int from_child_pipe[2];

    if (pipe(to_child_pipe) == -1 || pipe(from_child_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) { // Child process
        // Redirect stdin
        if (dup2(to_child_pipe[0], STDIN_FILENO) == -1) {
            perror("dup2 stdin");
            exit(EXIT_FAILURE);
        }

        // Redirect stdout
        if (dup2(from_child_pipe[1], STDOUT_FILENO) == -1) {
            perror("dup2 stdout");
            exit(EXIT_FAILURE);
        }

        close(to_child_pipe[1]);
        close(from_child_pipe[0]);

        // // Close unused pipe ends
        // close(to_child_pipe[0]);
        // close(to_child_pipe[1]);
        // close(from_child_pipe[0]);
        // close(from_child_pipe[1]);

        // Execute the expresso CLI
        // The executable should be in the same directory as the test
        execlp("./expresso", "expresso", (char *)NULL);

        // If execlp returns, it must have failed
        perror("execlp");
        exit(EXIT_FAILURE);
    } else { // Parent process
        char buffer[BUFFER_SIZE];
        ssize_t count;

        // Install signal handler
        signal(SIGALRM, handle_alarm);

        // Close unused pipe ends
        close(to_child_pipe[0]);
        close(from_child_pipe[1]);

        // Set alarm
        alarm(TIMEOUT);

        // Read initial prompt
        count = read(from_child_pipe[0], buffer, BUFFER_SIZE - 1);
        if (count > 0) {
            buffer[count] = '\0';
            printf("\nParent read #1: %s", buffer);
            fflush(stdout);
			assert(strstr(buffer, "expr> ") != NULL);
		}

        // --- Test History Navigation ---

        // 1. Write some commands to populate history
        const char* cmd1 = "1 + 1\n";
        write(to_child_pipe[1], cmd1, strlen(cmd1));
        usleep(100000); // Give time for processing
        count = read(from_child_pipe[0], buffer, BUFFER_SIZE - 1);
        buffer[count] = '\0';
        printf("\nParent read #2: %s|", buffer);
        fflush(stdout);
        assert(strstr(buffer, "expr> ") != NULL);

        const char* cmd2 = "2 + 2\n";
        write(to_child_pipe[1], cmd2, strlen(cmd2));
        usleep(100000);
        count = read(from_child_pipe[0], buffer, BUFFER_SIZE - 1);
        buffer[count] = '\0';
        printf("\nParent read #3: %s|", buffer);
        fflush(stdout);
        assert(strstr(buffer, "expr> ") != NULL);

        // 2. Navigate up
        const char* up_arrow = "\x1b[A";
        write(to_child_pipe[1], up_arrow, strlen(up_arrow));
        usleep(100000);
        count = read(from_child_pipe[0], buffer, BUFFER_SIZE - 1);
        buffer[count] = '\0';
        printf("\nParent read #4: %s|", buffer);
        fflush(stdout);
        assert(strstr(buffer, "2 + 2") != NULL);

        // 3. Navigate up again
        write(to_child_pipe[1], up_arrow, strlen(up_arrow));
        usleep(100000);
        count = read(from_child_pipe[0], buffer, BUFFER_SIZE - 1);
        buffer[count] = '\0';
        printf("\nParent read #5: %s|", buffer);
        fflush(stdout);
        assert(strstr(buffer, "1 + 1") != NULL);

        // 4. Navigate down
        const char* down_arrow = "\x1b[B";
        write(to_child_pipe[1], down_arrow, strlen(down_arrow));
        usleep(100000);
        count = read(from_child_pipe[0], buffer, BUFFER_SIZE - 1);
        buffer[count] = '\0';
        printf("\nParent read #6: %s|", buffer);
        fflush(stdout);
        assert(strstr(buffer, "2 + 2") != NULL);

        // --- End Test History Navigation ---

        // Write "!exit\n" to child's stdin
        const char* exit_command = "\n!exit\n";
        if (write(to_child_pipe[1], exit_command, strlen(exit_command)) == -1) {
            perror("write to child");
            // Kill child before exiting
            kill(child_pid, SIGKILL);
            exit(EXIT_FAILURE);
        }

        // Read from child's stdout
        count = read(from_child_pipe[0], buffer, BUFFER_SIZE - 1);
        if (count > 0) {
            buffer[count] = '\0';
            printf("\nParent read #7: %s|", buffer);
            // Check for the exit message
            fflush(stdout);
            assert(strstr(buffer, "Exiting Expresso.") != NULL);
        }

        // Close pipes
        close(to_child_pipe[1]);
        close(from_child_pipe[0]);

        // Wait for child to terminate
        int status;
        waitpid(child_pid, &status, 0);

        // Disable alarm
        alarm(0);

        if (WIFEXITED(status)) {
            printf("Child exited with status %d\n", WEXITSTATUS(status));
            assert(WEXITSTATUS(status) == 0);
        } else {
            printf("Child terminated abnormally\n");
            exit(EXIT_FAILURE);
        }

        printf("Interactive session test for '!exit' passed.\n");
    }

    return 0;
}
