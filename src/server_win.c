#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <winsock2.h>
#include <conio.h>

int main() {
    WSADATA wsa;
    SOCKET listenfd;
    struct sockaddr_in servaddr, cliaddr;
    int client_struct_length = sizeof(cliaddr);
    int running = 1;

    printf("Enter the port number\n");

    int port;
    scanf("%d", &port);
    if(port < 0 || port > 65534){
        printf("%d is an invalid number\n");
        return 1;
    }

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (listenfd == INVALID_SOCKET) {
        printf("Could not create socket. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    u_long mode = 1;
    ioctlsocket(listenfd, FIONBIO, &mode); // Non blocking

    printf("Socket created successfully\n");

    // Setup server address structure
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d\n", WSAGetLastError());
        closesocket(listenfd);
        WSACleanup();
        return 1;
    }

    printf("Done with binding\n");
    printf("Listening for incoming messages...\nPress Q|q to quit.\n\n");

    while (running) {
        if (_kbhit()) {
            int ch = _getch(); // consume the key
            if (ch == 'q' || ch == 'Q') {
                printf("Q|q Pessed. Exiting\n");
                running = 0;
            }
        }
		// Received Buffer Setup
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int recvlen = recvfrom(listenfd, buffer, sizeof(buffer), 0,
            (struct sockaddr*)&cliaddr, &client_struct_length);

        if (recvlen == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err != WSAEWOULDBLOCK && err!= WSAECONNRESET){
                printf("recvfrom() failed with error code: %d\n", err);
                break;
            }
			if (err == WSAECONNRESET){
				printf("ConnReset\n");
				
			}
        } else if (recvlen > 0) {
            //buffer[recvlen] = '\0';
            printf("Received message from IP: %s and port: %d\n",
            inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
            printf("Msg from client: %s\n", buffer);
            char *ptr;
            int sum = 0;
            ptr = strtok(buffer, ",");
            while (ptr != NULL){
                int i = atoi(ptr);
                sum += i;
                ptr = strtok(NULL, ",");
            }
            char newBuffer[10];
            memset(newBuffer, 0, sizeof(newBuffer));
            itoa(sum, newBuffer, 10);
            int sendvlen = sendto(listenfd, newBuffer, sizeof(newBuffer), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
            if (sendvlen == SOCKET_ERROR){
                printf("Error sending back: %d\n", WSAGetLastError());
            }
        }
        Sleep(100);

    }

    // Close socket
    closesocket(listenfd);
    WSACleanup();

    return 0;
}
