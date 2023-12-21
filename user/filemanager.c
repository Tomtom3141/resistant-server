#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This is intended to be used by the linux command line only
// Windows support must be added in later
int main(int argc, char **argv)
{
    wrapText("Welcome to the distributed server user hosting\n");
    printf("");
    return EXIT_SUCCESS;
}

// Wraps the given text in - character both before and after
void wrapText(const char* inputString) {
    // Calculate the length of the input string
    size_t length = strlen(inputString);

    // Print the line of "-" characters before the string
    for (size_t i = 0; i < length; i++) printf("-");
    printf("\n");

    // Print the input string
    printf("%s\n", inputString);

    // Print the line of "-" characters after the string
    for (size_t i = 0; i < length; i++) printf("-");
    printf("\n");
}