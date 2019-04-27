#ifndef EXT2_CHECKER_STRUCTS_H
#define EXT2_CHECKER_STRUCTS_H

#include <stdint.h>
#include <stdio.h>

typedef struct DiskGeometry
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
    uint32_t totalInodes;
    uint32_t totalBlocks;
    uint32_t superUserBlocks;
    uint32_t unallocatedBlocks;
    uint32_t unallocatedInodes;
    uint32_t superBlockNumber;
    uint32_t log2BlockSize;
    uint32_t log2FragmentSize;
    uint32_t blocksPerGroup;
    uint32_t fragmentsPerGroup;
    uint32_t inodesPerGroup;
    uint32_t lastMountTime;
    uint32_t lastWriteTime;
    //number of times mounted since last consistency check
    uint16_t timesMounted;
    // number of mounts allowed before consistency check
    uint16_t mountsAllowed;
    // should be 0xEF53
    uint16_t magic;
    // 1 means clean, 2 means system has errors
    uint16_t systemState;
    // what to do when there is an error
    // 1 - ignore error, 2 - remount file system as read only, 3 - kernel panic
    uint16_t errorHandler;
    // combine with major portion to get full version
    uint16_t minorVersion;
    // POSIX time of last consistency check
    uint32_t lastCheck;
    // interval between forced consistency checks
    uint32_t interval;
    // 0 - linux, 1 - GNU HURD, 2 - MASIX, 3 - FREEBSD, 4 - other "lites"
    uint32_t opSysId;
    uint32_t majorPortion;
    // user id that can use reserved blocks
    uint16_t userId;
    // group id that can use reserved blocks
    uint16_t groupId;

    uint32_t numBlockGroups;
    // left shift 1024 by log2PageSize to get page size
    uint32_t blockSize;

    uint8_t fullArray[1024];

} SuperBlock;

typedef struct //BlockGroupDescriptor
{
    uint32_t blockUsageBitmap;
    uint32_t inodeUsageBitmap;
    uint32_t inodeTableAddress;
    uint16_t numUnallocatedBlocks;
    uint16_t numUnallocatediNodes;
    uint16_t numDirectories;
    // some unused bytes here

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
    uint32_t pointers[15];
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
    Inode* inode;
    long long cursor;
    uint8_t* contents;
    char* name;
    uint32_t inodeNumber;
    //opendirectory(inode* inode, )
    // malloc array big enough to hold directory
    // read in stuff
    //rewindDirectory()
    // set cursor to 0 or 24 to skip . and .. directories
    //getNextEntry()
    // return true if there is a next entry, false otherwise
    // uses cursor to go into array thats been read in, get next directory entry (inode and name)
    //   then move cursor to next record
    //   if cursor = filesize return false
    //   else return true
    //

    //
} Directory;

typedef struct
{
    BlockGroupDescriptor ** blockGroupDescriptorTable;
    Header* header;
    int fileDescriptor;
    FILE* f;
    long long cursor;
    SuperBlock* superBlock;

} VDIFile;

#endif //EXT2_CHECKER_STRUCTS_H
