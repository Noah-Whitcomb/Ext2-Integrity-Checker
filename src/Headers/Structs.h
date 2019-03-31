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
    uint32_t totalinodes;
    uint32_t totalpages;
    uint32_t superuserpages;
    uint32_t unallocatedpages;
    uint32_t unallocatedinodes;
    uint32_t superpageknumber;
    uint32_t log2pagesize;
    uint32_t log2fragmentsize;
    uint32_t pagespergroup;
    uint32_t fragmentspergroup;
    uint32_t inodespergroup;
    uint32_t lastmounttime;
    uint32_t lastwritetime;
    //number of times mounted since last consistency check
    uint16_t timesmounted;
    // number of mounts allowed before consistency check
    uint16_t mountsallowed;
    // should be 0xEF53
    uint16_t magic;
    // 1 means clean, 2 means system has errors
    uint16_t systemstate;
    // what to do when there is an error
    // 1 - ignore error, 2 - remount file system as read only, 3 - kernel panic
    uint16_t errorhandler;
    // combine with major portion to get full version
    uint16_t minorversion;
    // POSIX time of last consistency check
    uint32_t lastcheck;
    // interval between forced consistency checks
    uint32_t interval;
    // 0 - linux, 1 - GNU HURD, 2 - MASIX, 3 - FREEBSD, 4 - other "lites"
    uint32_t opsysid;
    uint32_t majorportion;
    // user id that can use reserved blocks
    uint16_t userid;
    // group id that can use reserved blocks
    uint16_t groupid;

    uint32_t numpagegroups;

} SuperPage;

typedef struct
{

    Header* header;
    int fileDescriptor;
    FILE* f;
    long long cursor;
    SuperPage* superPage;

} VDIFile;

#endif //EXT2_CHECKER_STRUCTS_H
