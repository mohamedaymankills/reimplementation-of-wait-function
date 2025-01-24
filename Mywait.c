#include <stdio.h>   // For printf() and perror()
#include <stdlib.h>  // For exit() and EXIT_FAILURE
#include <unistd.h>  // For fork(), getpid(), and sleep()
#include <signal.h>  // For signal handling functions and constants
#include <sys/types.h> // For pid_t
#include <sys/wait.h>  // For waitpid() and related macros

// Signal handler for SIGCHLD using SA_SIGINFO
void handle_sigchld(int signo, siginfo_t *info, void *context) {
    (void)context; // Context parameter is unused, so we explicitly cast it to void to avoid compiler warnings

    // Check the reason why the SIGCHLD signal was sent using the si_code field
    if (info->si_code == CLD_EXITED) {
        // Child process exited normally (e.g., called exit())
        printf("Child with PID %d exited normally with status: %d\n",
               info->si_pid, info->si_status);
    } else if (info->si_code == CLD_KILLED) {
        // Child process was killed by a signal (e.g., SIGKILL or SIGTERM)
        printf("Child with PID %d was killed by signal: %d\n",
               info->si_pid, info->si_status);
    } else if (info->si_code == CLD_DUMPED) {
        // Child process was terminated and produced a core dump
        printf("Child with PID %d was terminated and dumped core (signal: %d)\n",
               info->si_pid, info->si_status);
    } else {
        // Handle any other unexpected termination codes
        printf("Child with PID %d terminated unexpectedly (code: %d)\n",
               info->si_pid, info->si_code);
    }
}

int main() {
    // Set up the SIGCHLD signal handler
    struct sigaction sa; // Define a sigaction structure to configure the handler

    sa.sa_sigaction = handle_sigchld; // Assign the handler function that uses the extended siginfo_t structure
    sa.sa_flags = SA_SIGINFO | SA_RESTART; // Enable extended signal info and restart interrupted syscalls
    sigemptyset(&sa.sa_mask); // Clear the signal mask to ensure no signals are blocked during the handler

    // Register the signal handler for SIGCHLD
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction"); // Print an error message if sigaction fails
        exit(EXIT_FAILURE);  // Exit the program with failure status
    }

    // Fork multiple child processes
    for (int i = 0; i < 3; i++) {
        pid_t pid = fork(); // Create a new child process

        if (pid == 0) {
            // This block is executed in the child process
            printf("Child process (PID: %d) is running...\n", getpid());
            sleep(2 + i); // Simulate some work by sleeping for a few seconds
            exit(10 + i); // Exit with a unique status (10, 11, or 12)
        } else if (pid < 0) {
            // If fork() returns a negative value, it means the fork failed
            perror("fork"); // Print an error message
            exit(EXIT_FAILURE); // Exit the program with failure status
        }
        // Parent process will continue executing the loop to create more children
    }

    // Parent process: After forking children, it waits for them to terminate
    printf("Parent process (PID: %d) is waiting for children to terminate...\n", getpid());

    // Simulate work in the parent process
    while (1) {
        // The parent process continues to run indefinitely, simulating ongoing work
        printf("Parent is doing some work...\n");
        sleep(1); // Sleep for 1 second before printing the message again
    }

    return 0; // This point is never reached because of the infinite loop
}

