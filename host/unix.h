#include <ifaddrs.h>

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
