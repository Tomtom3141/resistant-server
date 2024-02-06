#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <curl/curl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

/**
 * @brief Gets the MAC address of the host device for proper identification
 * 
 * @param interface connection interface that is being used by the host
 * @param macAddress the MAC address that is returned by the function
 */
void getMacAddress(char *interface, char *macAddress) {
    struct ifreq ifr;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);

    if (ioctl(sock, SIOCGIFHWADDR, &ifr) == -1) {
        perror("Error getting MAC address");
        close(sock);
        exit(EXIT_FAILURE);
    }

    close(sock);

    // Format the MAC address
    sprintf(macAddress, "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned char) ifr.ifr_hwaddr.sa_data[0],
            (unsigned char) ifr.ifr_hwaddr.sa_data[1],
            (unsigned char) ifr.ifr_hwaddr.sa_data[2],
            (unsigned char) ifr.ifr_hwaddr.sa_data[3],
            (unsigned char) ifr.ifr_hwaddr.sa_data[4],
            (unsigned char) ifr.ifr_hwaddr.sa_data[5]);
}

/**
 * @brief runs the main program for hosting on a unix based os and runs the intial setup if no other cohost is found
 * 
 * @param argc void
 * @param argv void
 * @return int runtime status
 */
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
    char timestamp[25];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&currentTime));

    // Open a file for writing
    FILE* roster = fopen("roster.txt", "w");

    // Check if the file was opened successfully
    if (roster == NULL) {
        perror("Error creating roster file");
        return EXIT_FAILURE;
    }

    // Get the MAC address from the host device
    char interface[] = "eth0"; //This must be changed to the proper network device of the host machine (default wired)
    char macAddress[18];
    getMacAddress(interface, macAddress);

    // Write the timestamp and MAC address to the file
    fprintf(roster, "Startup time for device %s at %s\n", macAddress, timestamp);

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
    printf("Formatting the entire drive into NTFS format...\n");

    // Assume the device path you want to format (e.g., /dev/sda1)
    char devicePath[] = "/dev/sda1"; //Change if needed (default currently at /dev/sda1)

    // Construct the command to format the drive
    char command[100];
    sprintf(command, "mkfs.ntfs %s", devicePath);
    // Execute the command
    int result = system(command);
    // Check the result of the formatting operation
    if (result == 0) {
        printf("Drive formatted successfully!\n");
    } else {
        //Exit the program due to format failure
        perror("Formatting failed. Please reformat drive to NTFS.\n");
        return EXIT_FAILURE;
    }


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

/**
 * @brief Connect to peers to obtain relevant server information
 * 
 * @param total the total number of hosts that currently exist
 * @param live the number of live hosts currently online and hosting
 * @param lastTime last timestamp of closest peer
 * @return int negative if error found
 */
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