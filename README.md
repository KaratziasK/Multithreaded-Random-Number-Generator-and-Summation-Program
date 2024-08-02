# Multithreaded Random Number Generator and Summation Program

This program demonstrates a multithreaded approach to generating random numbers, writing them to a file, and then reading and summing them using threads in a child process. The program uses semaphores for synchronization between processes and mutexes for safe access to shared data.

**NOTE:** This project was part of a university course on Operating Systems at Harokopio University of Athens.

## Features

- **Processes and Forking:** The program uses `fork()` to create a child process. The parent process generates random numbers and writes them to a file, while the child process reads these numbers and computes their sum using multiple threads.

- **Threads:** Four threads are used in the child process to read and sum parts of the file concurrently.

- **Synchronization:** Semaphores are used to synchronize the parent and child processes. Mutexes ensure safe access to shared resources by multiple threads.

- **Signal Handling:** The program handles `SIGINT` and `SIGTERM` signals gracefully, allowing the user to terminate the program if desired.

## Program Structure

The program is divided into the following main parts:

1. **Parent Process:**
   - Generates random numbers.
   - Writes the numbers to a file.
   - Each thread reads a specific section of the file and computes the sum of numbers in that section.
   - Each thread updates the global sum safely using mutexes.

2. **Child Process:**
   - Waits for the parent process to complete writing.
   - Uses multiple threads to read numbers from the file and compute their sum.
   - Outputs the results, including the sum computed by each thread.

3. **Threads:**
   - Each thread reads a specific section of the file and computes the sum of numbers in that section.
   - Each thread updates the global sum safely using mutexes.

## Build and Run Instructions

### Prerequisites

- GCC compiler
- POSIX-compliant environment

### Compilation

To compile the program, run the following command in the terminal:

```sh
gcc -o random_sum_program random_sum_program.c -lpthread
```
## Execution
After successful compilation, execute the program with:

```sh
./random_sum_program
```

## Signal Handling
The program can handle SIGINT (Ctrl+C) and SIGTERM signals.
If you send a SIGINT or SIGTERM to the program, it will prompt you to confirm whether you want to terminate the execution.

## Detailed Explanation
### Constants and Definitions
- **`NTHREADS:`** Number of threads used in the child process.
- **`LINES:`** Total number of lines written to the file.
- **`LINE_NUMS:`** Number of random numbers per line.
- **`LINES_PER_THREAD:`** Number of lines each thread processes.
- **`SIZE_OF_LINES:`** Size of each line in the file (including numbers and delimiters).

### Data Structures
**`threads_arguments:`** A structure that holds the file descriptor, thread number, local sum, and the number of lines read by a thread.
### Mutex and Semaphore
- **`mutexSum:`** Protects access to the global sum variable.
- **`mutexRead:`** Ensures exclusive access to file reading operations.
- **`semA and semB:`** Named semaphores for synchronization between parent and child processes.
## Output
The program prints the following information:

- Total sum of all random numbers (computed by threads).
- Local sum and the number of lines processed by each thread.
- Verification of the output by comparing it with the sum computed in the parent process.
## Notes
- The program creates a file named data.txt to store random numbers temporarily. Ensure your environment has permissions to create and write to this file.
- The program uses named semaphores, which persist in the system until explicitly removed. Ensure that semaphores are unlinked after use to prevent resource leaks.
