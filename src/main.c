//#include <stdio.h>
#include "Headers/VDIFile.h"
#include "Headers/Structs.h"
#include "Headers/Util.h"
#include "Headers/FileSystem.h"
#include "Headers/IntegrityCheckers.h"
#include "Headers/LinkedList.h"

#define PATH "C:\\vdifiles\\VDITestFiles\\Good\\Test-fixed-1k.vdi"
#define PATH_BAD "C:\\vdifiles\\VDITestFiles\\Bad\\inode-map-1k - Copy.vdi"

int main(int argc, char** argv) {

    //TODO: configure command line arguments for file path

    VDIFile* vdi = vdiOpen(PATH_BAD);
    if (vdi == NULL)
    {
        printf("Could not read file.");
        return 1;
    }

    //get superBlock, 1024 is superblock size always
    uint8_t superBlock [SUPERBLOCK_SIZE];
    vdiSeek(vdi, 0x400, VDI_SET);
    vdiRead(vdi, superBlock, SUPERBLOCK_SIZE);
    readSuperBlock(vdi, superBlock);

    //get block descriptor table
    uint8_t blockDescTable [vdi->superBlock->blockSize];
    fetchBlock(vdi, blockDescTable, 2);
    readBlockDescTable(vdi, blockDescTable);

    Bitmaps* bitmaps = initializeBitmaps(vdi);
    struct List* badSuperblocks = initializeList();
    struct List* badBGDescriptors = initializeList();
    struct List* inodesReachable = initializeList();
    struct List* inodesNotReachable = initializeList();
    struct List* blocksReachable = initializeList();
    struct List* blocksNotReachable = initializeList();
    struct List* duplicateBlocks = initializeList();

    if(integrityCheck(vdi, bitmaps, inodesNotReachable, inodesReachable, blocksNotReachable, blocksReachable,
                      badBGDescriptors, badSuperblocks, duplicateBlocks) != 1)
    {
        int count = 0;
        if(inodesNotReachable->size != 0 || blocksNotReachable->size != 0)
        {
            printf("Fixing bitmaps that mark unreachable blocks/inodes as used...\n");
            fixBitmaps(vdi, bitmaps);
            count = 1;
        }
        if(badBGDescriptors->size != 0 || badSuperblocks->size != 0)
        {
            printf("Fixing bad copies of superblock and/or block group descriptor table...\n");
            fixBadCopies(vdi, badSuperblocks, badBGDescriptors);
            count = 1;
        }
        if(count)
        {
            printf("Bad bitmaps and/or superblock/block group descriptor table copies fixed. You're welcome.\n");
        }

    }



    freeBitmaps(bitmaps, vdi);
    freeList(badBGDescriptors);
    freeList(badSuperblocks);
    freeList(inodesReachable);
    freeList(inodesNotReachable);
    freeList(blocksReachable);
    freeList(blocksNotReachable);
    freeList(duplicateBlocks);
    vdiClose(vdi);

    return 0;
}