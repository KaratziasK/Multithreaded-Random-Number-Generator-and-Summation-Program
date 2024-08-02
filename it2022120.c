#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define NTHREADS 4
#define LINES 100
#define LINE_NUMS 50
#define LINES_PER_THREAD (LINES / NTHREADS)
#define REMAINING_LINES (LINES % NTHREADS)
#define SIZE_OF_LINES (LINE_NUMS * sizeof(int) + LINE_NUMS * 1 + 1)

typedef struct {  // use this struct at thread_func
    int fd, thread, sum, lines;
} threads_arguments;

int global_sum = 0;  // Variable added all numbers
int fd;

pthread_mutex_t mutexSum = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexRead = PTHREAD_MUTEX_INITIALIZER;

void* thread_func(void* args) {
    char eat;
    threads_arguments* ptr = args;  // getting the struct
    int local_sum, fd, thread, num_read;
    local_sum = 0;
    fd = ptr->fd;
    thread = ptr->thread;

    int buf[LINE_NUMS];

    for (int cur_line = LINES_PER_THREAD * thread;
         cur_line < (thread + 1) * LINES_PER_THREAD;
         cur_line++) {                   // for every thread for run 25 times
        pthread_mutex_lock(&mutexRead);  // Critical area for reading file

        lseek(fd, cur_line * SIZE_OF_LINES,
              SEEK_SET);  // setting the cursor to the right place

        for (int i = 0; i < LINE_NUMS; i++) {
            read(fd, &buf[i], sizeof(int));  // reading data and save in buf[]
            read(fd, &eat, 1);               // removes space
        }
        read(fd, &eat, 1);  // removes \n
        pthread_mutex_unlock(&mutexRead);

        for (int i = 0; i < LINE_NUMS; i++) {
            local_sum += buf[i];  // adding all read data from buf[] to
                                  // local_sum
            buf[i] = 0;  // reset buf
        }
        num_read++;
    }
    if (REMAINING_LINES != 0) {  // Check that all lines was read
    }

    pthread_mutex_lock(
        &mutexSum);  // adding sum to global variable...we need mutex (lock)
    global_sum += local_sum;
    pthread_mutex_unlock(&mutexSum);  //(unlock)
    ptr->sum = local_sum;             // set to struct data
    ptr->lines = num_read;
    pthread_exit(NULL);  // thread return
}

void signal_management(int sig) {
    char ch;
    if (sig == SIGINT) {
        printf("Received SIGINT signal\nDo you want to stop executing?(Y/N)");
        scanf("%c", &ch);
        if ((ch == 'Y') || (ch == 'y')) {
            exit(2);
        }
    } else if (sig == SIGTERM) {
        printf("Received SIGTERM signal\nDo you want to stop executing?(Y/N)");
        scanf("%c", &ch);
        if ((ch == 'Y') || (ch == 'y')) {
            exit(15);
        }
    }
}

int main(int argc, char** argv) {
    // Signals managment
    signal(SIGINT, signal_management);  // setting actions for signals
    signal(SIGTERM, signal_management);

    int check_counter = 0;

    // Variables for genarate the random numbers
    int randomNum;

    // Variables for the fork
    int pid;
    int status;

    // Variables for the semaphore and create semaphore
    sem_t *semA, *semB;
    const char* semName1 = "it2022120A";
    const char* semName2 = "it2022120B";
    semA = sem_open(semName1, O_CREAT | O_TRUNC, 0600, 1);  // Open semaphores
    semB = sem_open(semName2, O_CREAT | O_TRUNC, 0600, 0);

    // Variables for threads
    pthread_t threads[NTHREADS];

    pid = fork();  // Fork and check for error (creating new process, child
                   // process)
    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    
    /*The PARENT process*/
    if (pid != 0) {
        srand(time(NULL));  // Initialize rand numbers.

        fd = open("data.txt", O_WRONLY | O_CREAT | O_TRUNC,
                  0644);  // Open file and check for error to open
        if (fd == -1) {
            perror("Error opening file!\n");
            exit(1);
        }

        printf("Parent process!\n");
        sem_wait(semA);  // semaphore block

        for (int i = 0; i < LINES; i++) {
            for (int j = 0; j < LINE_NUMS; j++) {
                randomNum =
                    rand() % 101;  // generating random number based in time
                check_counter += randomNum;  // keep the check_counter to check
                                             // the output later
                write(fd, &randomNum, sizeof(int));  // Writes a number
                write(fd, " ", 1);  // Add space between numbers
            }
            write(fd, "\n", 1);  // Add endline after write 50 numbers
        }
        printf("Writing in file ended!\n");
        close(fd);  // close file
        printf("Output must be: %d\n", check_counter);
        sem_post(semB);                    // unblock - set available
        waitpid(pid, &status, WUNTRACED);  // wait for the child
        sem_close(semA);                   // close semaphores
        sem_close(semB);
        sem_unlink(semName1);  // remove a named semaphore from the system
        sem_unlink(semName2);
        //Here the program end
    } else { /*The child process*/

        printf("Child Process!\n");
        fd = open("data.txt", O_RDONLY);
        if (fd == -1) {
            perror("Error opening file!\n");
            exit(1);
        }

        threads_arguments arg_thr[NTHREADS];  // Struct to be able to use more
                                              // variables in thread_func
        pthread_t threads[NTHREADS];
        sem_wait(semB);  // semaphore block

        for (int k = 0; k < NTHREADS; ++k) {
            arg_thr[k].fd = fd;  // setting data to struct to send them in
                                 // function thread_func
            arg_thr[k].thread = k;
            if (pthread_create(&threads[k], NULL, thread_func, &arg_thr[k]) !=0 ) {  // create threads
                perror("Error create thread");
                exit(1);
            }
        }
        sem_post(semA);                       // unblock semaphore
        for (int k = 0; k < NTHREADS; k++) {  // Wait for all threads to finish
            if (pthread_join(threads[k], NULL) !=
                0) {  // wait threads to join (come back from thread_func)
                perror("Error joining thread");
                exit(1);
            }
        }

        for (int i = 0; i < NTHREADS; i++) {  // print details for every thread
            printf("Thread: %d\nRead lines: %d\nIts local sum is: %d\n", i + 1,
                   arg_thr[i].lines, arg_thr[i].sum);
        }
        printf("The total sum for all threads is:%d\n",
               global_sum);  // print the sum of the random generated numbers
        close(fd);           // close file
    }
}