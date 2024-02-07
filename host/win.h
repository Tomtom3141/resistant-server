#include <Windows.h>
#include <winsock2.h>
#include <iphlpapi.h>

#pragma comment(lib, "IPHLPAPI.lib")

/**
 * @brief Gets the MAC address of the host device for proper identification
 * 
 * @param macAddress MAC address that is returned by the function
 */
void getMacAddress(char *macAddress) {
    PIP_ADAPTER_ADDRESSES adapterAddresses = NULL;
    ULONG bufferSize = 0;

    if (GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &bufferSize) == ERROR_BUFFER_OVERFLOW) {
        adapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(bufferSize);
        if (adapterAddresses == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
    } else {
        perror("Error getting adapter addresses");
        exit(EXIT_FAILURE);
    }

    if (GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapterAddresses, &bufferSize) == NO_ERROR) {
        PIP_ADAPTER_ADDRESSES adapter = adapterAddresses;
        while (adapter) {
            if (adapter->PhysicalAddressLength > 0) {
                for (DWORD i = 0; i < adapter->PhysicalAddressLength; i++) {
                    sprintf(macAddress + i * 3, "%02X:", adapter->PhysicalAddress[i]);
                }
                macAddress[adapter->PhysicalAddressLength * 3 - 1] = '\0'; // Remove the trailing ":"
                break; // Stop after the first non-empty MAC address
            }
            adapter = adapter->Next;
        }
    } else {
        perror("Error getting adapter addresses");
        free(adapterAddresses);
        exit(EXIT_FAILURE);
    }

    free(adapterAddresses);
}