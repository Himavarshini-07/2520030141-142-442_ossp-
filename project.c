#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

#define LOG_FILE "argument_log.txt"

/* Thread synchronization */
pthread_mutex_t mutex;
sem_t semaphore;

int shared_data = 0;

/* ---------------- SIGNAL ---------------- */

void signal_handler(int sig)
{
    if (sig == SIGUSR1)
        printf("Signal received: SIGUSR1\n");
}

/* ---------------- ARGUMENT PASSING ---------------- */

void argument_passing(int argc, char *argv[])
{
    printf("Number of arguments: %d\n", argc - 1);

    for (int i = 1; i < argc; i++)
    {
        printf("Argument %d: %s\n", i, argv[i]);
    }
}

/* ---------------- SYSTEM CALLS ---------------- */

void system_calls()
{
    printf("Process ID       : %d\n", getpid());
    printf("Parent Process ID: %d\n", getppid());

    printf("Shell Output:\n");
    system("echo Hello from Linux");

    printf("System call demonstration completed.\n");
}

/* ---------------- PROCESS ---------------- */

void process_demo()
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        printf("Child Process\n");
        printf("Child PID : %d\n", getpid());

        execlp("echo",
               "echo",
               "Child process executed successfully",
               NULL);

        perror("exec");
        exit(1);
    }
    else
    {
        printf("Parent Process\n");
        printf("Parent PID: %d\n", getpid());

        waitpid(pid, NULL, 0);

        printf("Child process terminated.\n");
    }
}

/* ---------------- PROCESS INFORMATION ---------------- */

void process_info()
{
    printf("Process ID      : %d\n", getpid());
    printf("Parent Process  : %d\n", getppid());
    printf("Process Group   : %d\n", getpgrp());
    printf("Session ID      : %d\n", getsid(0));
}

/* ---------------- PIPE ---------------- */

void pipe_demo()
{
    int fd[2];

    if (pipe(fd) == -1)
    {
        perror("pipe");
        return;
    }

    pid_t pid = fork();

    if (pid == 0)
    {
        close(fd[1]);

        char buffer[100];

        read(fd[0], buffer, sizeof(buffer));

        printf("Message received: %s\n", buffer);

        close(fd[0]);
        exit(0);
    }
    else
    {
        close(fd[0]);

        char message[] =
            "Hello from parent through pipe";

        write(fd[1],
              message,
              strlen(message) + 1);

        close(fd[1]);

        waitpid(pid, NULL, 0);
    }
}

/* ---------------- SIGNAL ---------------- */

void signal_demo()
{
    signal(SIGUSR1, signal_handler);

    printf("Sending SIGUSR1...\n");

    kill(getpid(), SIGUSR1);
}

/* ---------------- DYNAMIC MEMORY ---------------- */

void memory_demo(int n)
{
    int *ptr;

    ptr = (int *)malloc(n * sizeof(int));

    if (ptr == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    for (int i = 0; i < n; i++)
        ptr[i] = (i + 1) * 10;

    printf("Allocated memory values:\n");

    for (int i = 0; i < n; i++)
        printf("%d ", ptr[i]);

    printf("\n");

    free(ptr);

    printf("Memory released successfully.\n");
}

/* ---------------- MEMORY MAPPING ---------------- */

void mmap_demo()
{
    int size = 4096;

    char *memory =
        mmap(NULL,
             size,
             PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS,
             -1,
             0);

    if (memory == MAP_FAILED)
    {
        perror("mmap");
        return;
    }

    strcpy(memory,
           "Data stored in mapped memory");

    printf("%s\n", memory);

    munmap(memory, size);

    printf("Memory unmapped successfully.\n");
}

/* ---------------- FILE WRITE ---------------- */

void file_write(char *text)
{
    int fd;

    fd = open(LOG_FILE,
              O_CREAT | O_WRONLY | O_APPEND,
              0644);

    if (fd < 0)
    {
        perror("open");
        return;
    }

    write(fd, text, strlen(text));

    close(fd);

    printf("Data written successfully.\n");
}

/* ---------------- FILE READ ---------------- */

void file_read()
{
    int fd;

    fd = open(LOG_FILE, O_RDONLY);

    if (fd < 0)
    {
        perror("open");
        return;
    }

    char buffer[500];

    int n =
        read(fd,
             buffer,
             sizeof(buffer) - 1);

    if (n > 0)
    {
        buffer[n] = '\0';

        printf("%s", buffer);
    }

    close(fd);
}

/* ---------------- FILE INFORMATION ---------------- */

void file_info()
{
    struct stat st;

    if (stat(LOG_FILE, &st) < 0)
    {
        perror("stat");
        return;
    }

    printf("File Name : %s\n", LOG_FILE);
    printf("File Size : %ld bytes\n",
           (long)st.st_size);
    printf("Inode     : %ld\n",
           (long)st.st_ino);
}

/* ---------------- THREAD ---------------- */

void *thread_function(void *arg)
{
    pthread_mutex_lock(&mutex);

    printf("Thread is running\n");

    shared_data++;

    printf("Shared data: %d\n",
           shared_data);

    pthread_mutex_unlock(&mutex);

    sem_post(&semaphore);

    return NULL;
}

/* ---------------- THREAD SYNCHRONIZATION ---------------- */

void thread_demo()
{
    pthread_t t1, t2;

    pthread_mutex_init(&mutex, NULL);

    sem_init(&semaphore, 0, 0);

    pthread_create(&t1,
                   NULL,
                   thread_function,
                   NULL);

    pthread_create(&t2,
                   NULL,
                   thread_function,
                   NULL);

    sem_wait(&semaphore);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex);

    sem_destroy(&semaphore);

    printf("Thread synchronization completed.\n");
}

/* ---------------- MAIN ---------------- */

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: ./project <command> [arguments]\n");
        return 1;
    }

    /* Argument Passing */
    if (strcmp(argv[1], "args") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: ./project args <arguments>\n");
            return 1;
        }

        argument_passing(argc - 1, argv + 1);
    }

    /* System Calls */
    else if (strcmp(argv[1], "system") == 0)
    {
        system_calls();
    }

    /* Process */
    else if (strcmp(argv[1], "process") == 0)
    {
        process_demo();
    }

    /* Process Information */
    else if (strcmp(argv[1], "info") == 0)
    {
        process_info();
    }

    /* Pipe */
    else if (strcmp(argv[1], "pipe") == 0)
    {
        pipe_demo();
    }

    /* Signal */
    else if (strcmp(argv[1], "signal") == 0)
    {
        signal_demo();
    }

    /* Dynamic Memory */
    else if (strcmp(argv[1], "memory") == 0)
    {
        int n = 5;

        if (argc >= 3)
            n = atoi(argv[2]);

        memory_demo(n);
    }

    /* Memory Mapping */
    else if (strcmp(argv[1], "mmap") == 0)
    {
        mmap_demo();
    }

    /* File Write */
    else if (strcmp(argv[1], "write") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: ./project write <text>\n");
            return 1;
        }

        file_write(argv[2]);
    }

    /* File Read */
    else if (strcmp(argv[1], "read") == 0)
    {
        file_read();
    }

    /* File Information */
    else if (strcmp(argv[1], "fileinfo") == 0)
    {
        file_info();
    }

    /* Threads */
    else if (strcmp(argv[1], "thread") == 0)
    {
        thread_demo();
    }

    else
    {
        printf("Unknown command: %s\n",
               argv[1]);

        printf("\nAvailable commands:\n");
        printf("args\n");
        printf("system\n");
        printf("process\n");
        printf("info\n");
        printf("pipe\n");
        printf("signal\n");
        printf("memory\n");
        printf("mmap\n");
        printf("write\n");
        printf("read\n");
        printf("fileinfo\n");
        printf("thread\n");
    }

    return 0;
}
