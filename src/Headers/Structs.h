#ifndef EXT2_CHECKER_STRUCTS_H
#define EXT2_CHECKER_STRUCTS_H

#include <stdint.h>
#include <stdio.h>

typedef struct
{
    uint8_t preHeader[72];
    int headerSize;
    int imageType;
    int imageFlags;
    uint8_t imageDescription[32];
    //everything below might be in a different struct
    int offsetPages;

} Header;

typedef struct
{

    Header* header;
    int fileDescriptor;
    FILE* f;

} VDIFile;

#endif //EXT2_CHECKER_STRUCTS_H
