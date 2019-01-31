//
// Created by crisc on 1/28/2019.
//

#ifndef EXT2_CHECKER_STRUCTS_H
#define EXT2_CHECKER_STRUCTS_H

#include <stdint.h>

typedef struct header Header;

struct header
{
    uint8_t preHeader[72];
    int headerSize;
    int imageType;
    int imageFlags;
    uint8_t imageDescription[32];
    //everything below might be in a different struct
    int offsetBlocks;

};

#endif //EXT2_CHECKER_STRUCTS_H
