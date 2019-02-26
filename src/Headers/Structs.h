#ifndef EXT2_CHECKER_STRUCTS_H
#define EXT2_CHECKER_STRUCTS_H

#include <stdint.h>
#include <stdio.h>

typedef struct
{
    uint32_t cylinders;
    uint32_t heads;
    uint32_t sectors;
    uint32_t sectorSize;
} DiskGeometry;

typedef struct
{
    //block = pages
    // TODO: make sure all types are correct
    uint8_t preHeader[72];
    uint32_t headerSize;
    uint32_t imageType;
    uint32_t imageFlags;
    uint8_t imageDescription[32];
    DiskGeometry* diskGeometry;
    uint32_t offsetPages;
    uint32_t offsetData;
    uint32_t unused;
    long long diskSize;
    uint32_t pageSize;
    uint32_t pageExtraData;
    uint32_t pagesInHDD;
    uint32_t pagesAllocated;
    uint8_t UUID[16];
    uint8_t UUIDLastSnap[16];
    uint8_t UUIDLink[16];
    uint8_t UUIDParent[16];
    uint8_t shit[56];

} Header;

typedef struct
{

    Header* header;
    int fileDescriptor;
    FILE* f;
    long long cursor;

} VDIFile;

#endif //EXT2_CHECKER_STRUCTS_H
