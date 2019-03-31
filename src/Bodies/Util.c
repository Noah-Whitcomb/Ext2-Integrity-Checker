//
// Created by crisc on 1/28/2019.
//

#include "../Headers/Util.h"

void printBytes(uint8_t* buffer, size_t size, char* description)
{
    printf("\n%s\n", description);
    for(size_t i = 0; i<size; i++)
    {
        printf("%X ", buffer[i]);
    }
    printf("\n");
}

void printHeader(Header* head)
{
    printf("PRE HEADER - STRING\n%s\n", head->preHeader);
    printBytes(head->preHeader, 72, "PRE-HEADER - BYTES");

    printf("HEADER SIZE - INT\n%i\n", head->headerSize);
    printBytes((uint8_t *)&head->headerSize, 4, "HEADER SIZE - BYTES");

    printf("IMAGE TYPE - INT\n%i\n", head->imageType);
    printBytes((uint8_t *)&head->imageType, 4, "IMAGE TYPE - BYTES");

    printf("IMAGE FLAGS - INT\n%i\n", head->imageFlags);
    printBytes((uint8_t *)&head->imageFlags, 4, "IMAGE FLAGS - BYTES");

    printBytes(head->imageDescription, 32, "IMAGE DESCRIPTION - BYTES");

    printf("OFFSET BLOCKS - INT\n%i\n", head->offsetPages);
    printBytes((uint8_t *)&head->offsetPages, 4, "OFFSET BLOCKS - BYTES");
}