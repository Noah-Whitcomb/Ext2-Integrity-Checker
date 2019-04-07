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
    //TODO: refactor all of these to camel case
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
    // left shift 1024 by log2PageSize to get page size
    uint32_t pageSize;

} SuperPage;

typedef struct
{
    uint32_t blockUsageBitmap;
    uint32_t inodeUsageBitmap;
    uint32_t inodeTableAddress;
    uint16_t numUnallocatedBlocks;
    uint16_t numUnallocatediNodes;
    uint16_t numDirectories;
    // some unused bytes here

    //TODO: refactor all block shit to page if it matters

}BlockGroupDescriptor;

typedef struct
{
    uint16_t typePermissions;
    uint16_t userId;
    uint32_t lower32BitsSize;
    uint32_t lastAccessTime;
    uint32_t creationTime;
    uint32_t lastModificationTime;
    uint32_t deletionTime;
    uint16_t groupId;
    uint16_t hardLinkCount;
    uint32_t diskSectorCount;
    uint32_t flags;
    uint32_t opSystemValue1;
    uint32_t directPointer0;
    uint32_t directPointer1;
    uint32_t directPointer2;
    uint32_t directPointer3;
    uint32_t directPointer4;
    uint32_t directPointer5;
    uint32_t directPointer6;
    uint32_t directPointer7;
    uint32_t directPointer8;
    uint32_t directPointer9;
    uint32_t directPointer10;
    uint32_t directPointer11;
    uint32_t singlyIndirectBlockPointer;
    uint32_t doublyIndirectBlockPointer;
    uint32_t triplyIndirectBlockPointer;
    uint32_t generationNumber;
    // for ext2 version 0, reserved field, for ext2 version >= 1, extended attribute block
    uint32_t reservedField;
    // for ext2 version 0, reserved field, for ext2 version >= 1, upper 32 bits of file size
    uint32_t reservedField2;
    uint32_t fragmentBlockAddress;
    uint8_t opSysValue2[12];
}Inode;

typedef struct
{
    BlockGroupDescriptor ** blockGroupDescriptorTable;
    Header* header;
    int fileDescriptor;
    FILE* f;
    long long cursor;
    SuperPage* superPage;

} VDIFile;

#endif //EXT2_CHECKER_STRUCTS_H
