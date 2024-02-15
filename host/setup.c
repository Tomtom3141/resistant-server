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
#ifdef _WIN32
    #include "win.h"
#elif __unix__
    #include "unix.h"
#else
    #error "Operating system detection is not supported on this platform."
#endif

/**
 * @brief runs the main program for hosting on a unix based os and runs the intial setup if no other cohost is found
 * 
 * @param argc void
 * @param argv void
 * @return int runtime status is 1 if errors occured
 */
int main(int argc, char **argv)
{
    // Open the roster for writing
    FILE* roster = fopen("roster.txt", "r+");

    // Check if the file was opened successfully
    if (roster == NULL) {
        perror("Error creating roster file");
        return EXIT_FAILURE;
    }

    // The total number of hosts that exist or that have registered
    int totalHosts;
    // The current number of running hosts
    int liveHosts;
    // The last obtained time from the nearest host peer
    time_t peerTime;
    //Gets the status  of connecting to peers
    int connectionFail = 0;

    // Check if the roster file existed before
    fseek(roster, 0, SEEK_END);
    if (ftell(roster) == 0) { //File is empty meaning this host has never hosted before
        // Fill with other host information if roster file did not exist
        // Connect to other peers to get information
        connectionFail = peerSync(totalHosts, liveHosts, peerTime);
        // Set the important variables obtained from the connection
    }

    // If unable to connect to other peers
    if(connectionFail){
        perror("Error connecting to peers");
        return EXIT_FAILURE;
    }
    // There are no hosts present and this is the first server
    if(totalHosts == 0) printf("First time startup detected...\n");
    // There are no hosts online currently meaning the distributed server is completely offline
    if(liveHosts == 0) printf("Server completely offline. Initializing...\n");

    // Get the MAC address from the host device
    // For the case of windows host machines
    #ifdef _WIN32
        PIP_ADAPTER_ADDRESSES adapterAddresses = NULL;
        ULONG bufferSize = 0;
        DWORD result = 0;

        // Call GetAdaptersAddresses with a NULL buffer to get the required buffer size
        result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &bufferSize);
        if (result == ERROR_BUFFER_OVERFLOW) {
            adapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(bufferSize);
            if (adapterAddresses == NULL) {
                perror("Memory allocation failed");
                return EXIT_FAILURE;
            }
            // Call GetAdaptersAddresses again with the allocated buffer
            result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapterAddresses, &bufferSize);
            if (result == NO_ERROR) {
                PIP_ADAPTER_ADDRESSES adapter = adapterAddresses;
                while (adapter) {
                    printf("Interface: %S\n", adapter->FriendlyName);
                    adapter = adapter->Next;
                }
            } else {
                perror("Error getting adapter addresses");
                free(adapterAddresses);
                return EXIT_FAILURE;
            }
        } else {
            perror("Error getting adapter addresses");
            return EXIT_FAILURE;
        }

    free(adapterAddresses);
    // For the case of unix and linux machines
    #elif __unix__
        struct ifaddrs *ifap, *ifa;
        char host[NI_MAXHOST];

        // Get the list of network interfaces
        if (getifaddrs(&ifap) == -1) {
            perror("Failure to get network interfaces");
            exit(EXIT_FAILURE);
        }

        // Iterate over the list of interfaces and print their names
        for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == NULL) continue;

            printf("Interface: %s\n", ifa->ifa_name);

            // Optionally, you can print the address of the interface
            // if (getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) == 0) {
            //     printf("Address: %s\n", host);
            // }
        }

        // Free the memory allocated by getifaddrs
        freeifaddrs(ifap);
    #endif
    char macAddress[18];
    #ifdef _WIN32
        getMacAddress(macAddress);
    #elif __unix__
        // Get the specified interface requested
        printf("Please select the desired interface to connect to the server: ");
        char interface[20];
        // Read user input using fgets
        if (fgets(interface, sizeof(interface), stdin) != NULL) {
            // Remove the newline character if present
            char *newline = strchr(interface, '\n');
            if (newline != NULL) {
                *newline = '\0';
            }

            printf("You selected the interface: %s\n", interface);
        } else {
            printf("Error reading interface input\n");
            return EXIT_FAILURE;
        }
        getMacAddress(interface, macAddress);
    #endif

    // Get the current time
    time_t currentTime = time(NULL);

    // Check if the time retrieval was successful
    if (currentTime == -1) {
        perror("Error getting current time");
        return EXIT_FAILURE;
    }

    // Convert the time to a string representation
    char timestamp[25];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&currentTime));

    // Write the timestamp and MAC address to the file
    fprintf(roster, "Startup time for device %s at %s\n", macAddress, timestamp);


    // Setup storage medium with proper formatting
    printf("Formatting the entire drive into NTFS format...\n");

    // Construct the command to format the drive
    char command[100];
    // Format for windows systems
    #ifdef _WIN32
        // Assume the drive letter and partition number you want to format
        char driveLetter = 'C'; //Must be changed

        sprintf(command, "format %c: /FS:NTFS /Q", driveLetter);

        // Execute the command
        int result = system(command);

        // Check the result of the formatting operation
        if (result == 0) {
            printf("Drive formatted successfully!\n");
        } else {
            printf("Formatting failed. Please format drive into NTFS.\n");
            return EXIT_FAILURE;
        }
    // Assume the device path you want to format (e.g., /dev/sda1)
    #elif __unix__
        char devicePath[] = "/dev/sda1"; //Change if needed (default currently at /dev/sda1)

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
    #endif


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

/**
 * @brief This function is used to count the number of servers that are online using recursion
 * 
 * @param lastcount the number of online hosts last counted
 * @return int the host currently running plus a rollcall to other servers
 */
int rollcall(int lastcount){
    int count = lastcount;
    // Connect to closes peer to increment the count
    return count;
}