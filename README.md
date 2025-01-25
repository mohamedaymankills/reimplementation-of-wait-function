# Reimplementation-of-wait-function
# Non-Blocking Wait Alternative

## Overview

This project demonstrates an alternative to the traditional `wait()` system call by implementing a **non-blocking wait** approach in a Linux environment. The main goal is to manage child process terminations without blocking the parent process, ensuring that terminated child processes do not become zombie processes. Instead of relying on `wait()` or `waitpid()`, the parent process handles child terminations asynchronously using signals.

When a child process terminates, the parent process will print out:
- The **PID** (Process ID) of the terminated child process.
- The **exit status** of the child process.
- The **reason for termination**, whether due to a normal exit or by being killed with a signal.

The project uses the `SIGCHLD` signal and the `sigaction()` system call to asynchronously manage child terminations and prevent the accumulation of zombie processes.

## Features

- **Non-blocking operation**: The parent process does not wait for child processes to finish, allowing it to perform other tasks.
- **Signal handling**: Uses the `SIGCHLD` signal to catch child termination events in real-time.
- **Zombie prevention**: No child process is left in a zombie state. The parent process immediately handles terminated children.
- **Termination details**: For each terminated child, the parent prints out the child’s PID, exit status, and the reason for termination (normal exit or signal termination).

## Motivation

The traditional `wait()` system call can block the parent process, causing inefficiency in applications that need to continue executing other tasks while managing child processes. This project addresses this inefficiency by implementing a signal-based, non-blocking approach to child process management. It provides a more scalable and responsive way to handle multiple child processes, especially in long-running applications.

## How It Works

### 1. Signal Handler

The heart of the non-blocking approach is the signal handler for `SIGCHLD`. When a child process terminates, it sends a `SIGCHLD` signal to the parent. The parent has a signal handler set up to capture this signal. Upon receiving `SIGCHLD`, the parent process does not block or wait for the child’s termination explicitly but immediately processes the information about the child’s exit status and termination reason.

The handler function uses the `siginfo_t` structure, which is passed as part of the signal information, to extract:
- **Termination reason**: The `si_code` field indicates whether the child exited normally, was killed by a signal, or generated a core dump.
- **Exit status**: The `si_status` field holds the exit status code or the signal number that caused the child to terminate.

### 2. Forking Child Processes

The parent process forks multiple child processes. Each child simulates some work (via `sleep()`) before exiting with a specific status code. The parent does not wait for the children synchronously but instead continues to do its work, handling terminated children asynchronously as they exit.

The parent process uses the `sigaction()` function to set up the signal handler for `SIGCHLD`. This ensures that when any child terminates, the appropriate information is captured, and no child becomes a zombie.

### 3. Parent Process Workflow

While the children perform their tasks, the parent process remains active, continuously doing its own work. It doesn’t need to wait for child termination explicitly. The signal handler is responsible for handling the termination of children and printing their details. The parent process can thus continue performing other tasks while keeping track of the children’s state.

### 4. Preventing Zombie Processes

Without proper handling, child processes that terminate can become zombie processes, which consume system resources unnecessarily. The use of the signal handler ensures that terminated children are properly cleaned up, and their resources are freed, thus preventing zombies from accumulating.

## Key Components

### 1. **Signal Handling**

The key technique used in this project is **signal handling**. By using the `sigaction()` system call, we configure the parent to receive `SIGCHLD` signals whenever a child process terminates. This is a more efficient and responsive approach compared to polling with `wait()`.

### 2. **Child Process Creation**

The parent process uses the `fork()` system call to create multiple child processes. Each child simulates work by sleeping for a few seconds and then exits with a different status code. These child processes are monitored asynchronously, and their exit status and termination reason are printed by the parent.

### 3. **Exit Status and Termination Reasons**

The signal handler examines the `si_code` field of the `siginfo_t` structure to determine how the child process terminated. The possible values include:
- `CLD_EXITED`: The child process exited normally (via `exit()`).
- `CLD_KILLED`: The child was killed by a signal (e.g., `SIGKILL` or `SIGTERM`).
- `CLD_DUMPED`: The child produced a core dump when terminated by a signal.
- Other codes represent unexpected terminations, which are handled as exceptions.

## Benefits of This Approach

- **Asynchronous Processing**: The parent can continue executing tasks without being blocked by child process terminations.
- **Efficient Resource Management**: By immediately collecting the exit status and avoiding zombie processes, system resources are efficiently managed.
- **Improved Performance**: In scenarios with multiple child processes, this method improves performance by eliminating the need for the parent to wait synchronously for each child to terminate.

## Limitations

- **Signal Overhead**: Signal handling, while efficient, adds some complexity and overhead. This method may not be suitable for scenarios where high-frequency signals are needed.
- **Signal Delivery Reliability**: If the signal handler is too slow or there are too many children, the system may experience delays or race conditions. In such cases, other methods of child process management may be more appropriate.

## Getting Started

### Prerequisites

- A Linux-based operating system with a C compiler (GCC) installed.
- Basic knowledge of process management, signal handling, and system calls in Linux.

### Installation

1. Clone or download the repository containing the source code.
2. Compile the program with `gcc`:
   ```bash
   gcc -o Mywait Mywait.c
