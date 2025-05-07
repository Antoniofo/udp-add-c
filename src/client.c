#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>              // for close()
#include <arpa/inet.h>           // for inet_addr(), htons()
#include <sys/socket.h>          // for socket(), sendto()
#include <netinet/in.h>          // for sockaddr_in

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[1024] = "8,8,8";
    char recbuffer[1024];
    int port;

    printf("Enter port number:\n");
    scanf("%d", &port);
    if (port < 0 || port > 65535) {
        printf("%d is an invalid port number.\n", port);
        return 1;
    }

    printf("Enter a sequence of numbers (e.g., 2,3,4,5):\n");
    scanf("%s", buffer);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 1;
    }

    printf("Socket created successfully\n");

    // Setup server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Setup optional client address (not required to bind for UDP client, but if needed:)
    /*
    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(port + 1);
    cliaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        return 1;
    }
    */

    printf("Sending message...\n");

    // Send UDP message
    if (sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Couldn't send");
        close(sockfd);
        return 1;
    }

    printf("Message sent\n");

    // Set timeout for receiving (optional but helpful)
    struct timeval timeout = {2, 0};  // 2 seconds
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    socklen_t len = sizeof(servaddr);
    memset(recbuffer, 0, sizeof(recbuffer));

    int recvlen = recvfrom(sockfd, recbuffer, sizeof(recbuffer), 0,
                           (struct sockaddr*)&servaddr, &len);

    if (recvlen < 0) {
        perror("recvfrom() failed or timed out");
    } else {
        printf("Received message: %s\n", recbuffer);
    }

    close(sockfd);
    return 0;
}
