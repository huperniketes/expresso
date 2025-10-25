#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For pipe, fork, dup2
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024
#define TIMEOUT 5

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

        // Close unused pipe ends
        close(to_child_pipe[0]);
        close(to_child_pipe[1]);
        close(from_child_pipe[0]);
        close(from_child_pipe[1]);

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
            printf("Parent read: %s", buffer);
            assert(strstr(buffer, "expr> ") != NULL);
        }


        // Write "!exit\n" to child's stdin
        const char* exit_command = "!exit\n";
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
            printf("Parent read: %s", buffer);
            // Check for the exit message
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
