#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
    char studentID[50];
    int times[3];
    int pipe1[2], pipe2[2];

    printf("Enter Student ID: ");
    scanf("%s", studentID);
    printf("Enter Task 1 Execution Time: ");
    scanf("%d", &times[0]);
    printf("Enter Task 2 Execution Time: ");
    scanf("%d", &times[1]);
    printf("Enter Task 3 Execution Time: ");
    scanf("%d", &times[2]);

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe failed");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // Child Process
        close(pipe1[1]);
        close(pipe2[0]);
        int received[3];
        read(pipe1[0], received, sizeof(received));
        close(pipe1[0]);

        int total = received[0] + received[1] + received[2];
        int max = received[0];
        int longestTask = 1;
        for (int i = 1; i < 3; i++) {
            if (received[i] > max) {
                max = received[i];
                longestTask = i + 1;
            }
        }
        int result[3];
        result[0] = total;
        result[1] = longestTask;
        result[2] = max;
        write(pipe2[1], result, sizeof(result));
        close(pipe2[1]);
        exit(0);
    } else {
        // Parent Process
        close(pipe1[0]);
        close(pipe2[1]);
        write(pipe1[1], times, sizeof(times));
        close(pipe1[1]);

        int result[3];
        read(pipe2[0], result, sizeof(result));
        close(pipe2[0]);
        wait(NULL);

        int total = result[0];
        int longestTask = result[1];
        int max = result[2];

        printf("\n--- Task Processing Report ---\n");
        printf("Student ID: %s\n", studentID);
        printf("Task 1: %d units\n", times[0]);
        printf("Task 2: %d units\n", times[1]);
        printf("Task 3: %d units\n", times[2]);
        printf("Total Execution Time: %d units\n", total);
        printf("Longest Task: Task %d\n", longestTask);
        printf("Maximum Execution Time: %d units\n", max);

        char filename[100];
        sprintf(filename, "%s_report.txt", studentID);
        int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("open failed");
            return 1;
        }
        char buffer[600];
        int len = sprintf(buffer, "--- Task Processing Report ---\nStudent ID: %s\nTask 1: %d units\nTask 2: %d units\nTask 3: %d units\nTotal Execution Time: %d units\nLongest Task: Task %d\nMaximum Execution Time: %d units\n",
            studentID, times[0], times[1], times[2], total, longestTask, max);
        write(fd, buffer, len);
        close(fd);
        printf("Report saved to: %s\n", filename);
    }
    return 0;
}
