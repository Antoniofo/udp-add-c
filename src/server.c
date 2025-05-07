#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <termios.h>

int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    int bytesWaiting;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int main() {
    int listenfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t client_struct_length = sizeof(cliaddr);
    int running = 1;

    printf("Enter the port number:\n");
    int port;
    scanf("%d", &port);
    if (port < 0 || port > 65535) {
        printf("%d is an invalid port number.\n", port);
        return 1;
    }

    // Create socket
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (listenfd < 0) {
        perror("socket creation failed");
        return 1;
    }

    // Set socket to non-blocking mode
    int flags = fcntl(listenfd, F_GETFL, 0);
    fcntl(listenfd, F_SETFL, flags | O_NONBLOCK);

    // Setup server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        close(listenfd);
        return 1;
    }

    printf("Done with binding\n");
    printf("Listening for incoming messages...\nPress Q|q to quit.\n\n");

    while (running) {
        if (kbhit()) {
            int ch = getchar();
            if (ch == 'q' || ch == 'Q') {
                printf("Q|q pressed. Exiting.\n");
                running = 0;
                break;
            }
        }

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        int recvlen = recvfrom(listenfd, buffer, sizeof(buffer), 0,
                               (struct sockaddr*)&cliaddr, &client_struct_length);

        if (recvlen < 0) {
            // No data received, not an error in non-blocking mode
        } else {
            printf("Received message from IP: %s and port: %d\n",
                   inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
            printf("Msg from client: %s\n", buffer);

            int sum = 0;
            char *ptr = strtok(buffer, ",");
            while (ptr != NULL) {
                sum += atoi(ptr);
                ptr = strtok(NULL, ",");
            }

            char newBuffer[32];
            snprintf(newBuffer, sizeof(newBuffer), "%d", sum);

            int sent = sendto(listenfd, newBuffer, strlen(newBuffer), 0,
                              (struct sockaddr*)&cliaddr, client_struct_length);
            if (sent < 0) {
                perror("sendto failed");
            }
        }

        usleep(100000); // Sleep 100 ms
    }

    close(listenfd);
    return 0;
}
