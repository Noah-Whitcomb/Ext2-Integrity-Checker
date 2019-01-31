//
// Created by crisc on 1/28/2019.
//

#ifndef EXT2_CHECKER_UTIL_H
#define EXT2_CHECKER_UTIL_H

#include "../Headers/Util.h"
#include "../Headers/Structs.h"
#include "stdint.h"
#include "stdio.h"

void printBytes(uint8_t* buffer, size_t size, char* description);
void printHeader(Header* head);

#endif //EXT2_CHECKER_UTIL_H
