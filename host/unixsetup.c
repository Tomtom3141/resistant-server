#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char **argv)
{
    // Generate roaster file
    time_t currentTime = time(NULL);

    // Check if the time retrieval was successful
    if (currentTime == -1) {
        perror("Error getting current time");
        return EXIT_FAILURE;
    }

    // Convert the time to a string representation
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&currentTime));

    // Open a file for writing
    FILE* roster = fopen("roster.txt", "w");

    // Check if the file was opened successfully
    if (roster == NULL) {
        perror("Error creating roster file");
        return EXIT_FAILURE;
    }

    // Write the timestamp to the file
    fprintf(roster, "%s\n", timestamp);

    //Get the host information
    int totalHosts;
    int liveHosts;
    time_t peerTime;
    int connectionFail = peerSync(totalHosts, liveHosts, peerTime);
    if(connectionFail){
        perror("Error connecting to peers");
        return EXIT_FAILURE;
    }
    if(totalHosts == 0) printf("First time startup detected...\n");
    if(liveHosts == 0) printf("Server completely offline. Initializing...\n");

    // Setup storage medium with proper formatting
    // ...

    // Load storage medium with any server information
    // Load balancing effort
    // ...

    // Establish protocol for regular synchronization
    // ...

    // Finish setup
    // ...
    // Close the file
    fclose(roster);
    printf("Roster has been successfully created\n");
    printf("Successfully run host setup\n");
    return EXIT_SUCCESS;
}

// Connect to peers to obtain relevant server information
int peerSync(int total, int live, time_t lastTime){
    // IP address and port to connect to
    // Placeholder only for peers and must be updated later
    const char* serverIP = "69.69.69.69";
    const int serverPort = 6969;

    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error creating socket");
        return EXIT_FAILURE;
    }

    // Set up the server address structure
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, serverIP, &(serverAddr.sin_addr)) <= 0) {
        perror("Error converting IP address");
        close(clientSocket);
        return EXIT_FAILURE;
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to the server");
        close(clientSocket);
        return EXIT_FAILURE;
    }

    printf("Connected to %s on port %d\n", serverIP, serverPort);

    // Additional code for sending/receiving data can be added here

    // Close the socket
    close(clientSocket);

    return EXIT_SUCCESS;
}

// When for peers connect to obtain relevant host info
int peerSend(){
    CURL* curl;
    CURLcode res;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Create a curl handle
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Error initializing libcurl\n");
        return EXIT_FAILURE;
    }

    // Set the URL to test site
    curl_easy_setopt(curl, CURLOPT_URL, "http://httpbin.org/ip");

    // Create a buffer to store the response
    char responseBuffer[256];
    responseBuffer[0] = '\0';

    // Set the write callback function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, responseBuffer);

    // Perform the HTTP request
    res = curl_easy_perform(curl);

    // Check for errors
    if (res != CURLE_OK) {
        fprintf(stderr, "Failed to obtain machine ip address: %s\n", curl_easy_strerror(res));
    } else {
        // Print the external IP address
        printf("External IP Address: %s\n", responseBuffer);
    }

    // Clean up
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return EXIT_SUCCESS;
}

// Callback function for libcurl to handle the response
size_t writeCallback(void* contents, size_t size, size_t nmemb, char* buffer) {
    size_t realsize = size * nmemb;
    strcat(buffer, (char*)contents);
    return realsize;
}