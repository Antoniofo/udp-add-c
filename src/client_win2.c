#include <stdio.h>
#include <string.h>
#include <winsock2.h>

int main() {

    // Définition variable
    WSADATA wsa;
    SOCKET sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[65508];
    for (int i = 0; i < sizeof(buffer); i++){
        if(i % 2 == 0){
	    buffer[i] = '2';
	}else{
	    buffer[i] = ',';
	}
    }

    int port;

    printf("Enter port number\n");
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
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Error creating socket. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    printf("Socket created successfully\n");

    // Setup server address structure
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

/*
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(port+1);
    cliaddr.sin_addr.s_addr = INADDR_ANY;

    // Binding
    if (bind(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
*/
    printf("Sending message\n");

    // Send UDP packet
    if (sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        printf("Couldn't send. Error Code: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    printf("Message sent\n");

    char recbuffer[1024];
    memset(recbuffer, 0, sizeof(recbuffer));
    int recvlen = recvfrom(sockfd, recbuffer, sizeof(recbuffer), 0, NULL, NULL);

    if (recvlen == SOCKET_ERROR) {
       int err = WSAGetLastError();
       if (err != WSAEWOULDBLOCK){
            printf("recvfrom() failed with error code: %d\n", err);

       }
    } else if (recvlen > 0) {
       printf("Received message: %s\n", recbuffer);

    }
    // Close socket and clean up
    closesocket(sockfd);
    WSACleanup();

    return 0;
}
