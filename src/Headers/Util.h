//
// Created by crisc on 1/28/2019.
//

#ifndef EXT2_CHECKER_UTIL_H
#define EXT2_CHECKER_UTIL_H

#include "../Headers/Structs.h"
#include "stdint.h"
#include "stdio.h"

void printBytes(const uint8_t* buffer, size_t size, char* description);
uint32_t getNumBlockGroups(const VDIFile* vdi);
uint32_t power(uint32_t base, uint32_t power);
int checkPowerOf(uint32_t power, uint32_t number);
uint32_t numEntriesBitmap(const uint8_t* bitmap, uint32_t numEntries);

#endif //EXT2_CHECKER_UTIL_H
