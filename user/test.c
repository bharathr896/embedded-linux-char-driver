#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <uapi/msg_board_uapi.h>

#define DEVICE_PATH "/dev/msgboard"


void print_menu() {
    printf("\n==== Message Board IOCTL Menu ====\n");
    printf("1. Set Message\n");
    printf("2. Get Message\n");
    printf("3. Clear Board\n");
    printf("4. Get Open Count\n");
    printf("5. Exit\n");
    printf("Choose an option: ");
}

int main() {
    int fd, choice;
    char message[BUFFER_SIZE];
    int open_count;

    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        switch (choice) {
            case 1:
                printf("Enter message: ");
                getchar(); 
                if (fgets(message, BUFFER_SIZE, stdin) == NULL) {
                    printf("Error reading input.\n");
                    continue;
                }
                message[strcspn(message, "\n")] = '\0'; 

                if (ioctl(fd, MSGB_SET_MESSAGE, message) == -1) {
                    perror("ioctl MSGB_SET_MESSAGE");
                } else {
                    printf("Message set successfully.\n");
                }
                break;

            case 2:
                memset(message, 0, sizeof(message));
                if (ioctl(fd, MSGB_GET_MESSAGE, message) == -1) {
                    perror("ioctl MSGB_GET_MESSAGE");
                } else {
                    printf("Message from board: %s\n", message);
                }
                break;

            case 3:
                if (ioctl(fd, MSGB_CLEAR_BOARD) == -1) {
                    perror("ioctl MSGB_CLEAR_BOARD");
                } else {
                    printf("Board cleared successfully.\n");
                }
                break;

            case 4:
                if (ioctl(fd, MSGB_GET_OPEN_COUNT, &open_count) == -1) {
                    perror("ioctl MSGB_GET_OPEN_COUNT");
                } else {
                    printf("Open count: %d\n", open_count);
                }
                break;

            case 5:
                close(fd);
                printf("Exiting.\n");
                return EXIT_SUCCESS;

            default:
                printf("Invalid choice.\n");
                break;
        }
    }

    close(fd);

    return EXIT_SUCCESS;
}
