//#include <stdio.h>
#include "Headers/VDIFile.h"
#include "Headers/Structs.h"
#include "Headers/Util.h"
#include "Headers/FileSystem.h"
#include "Headers/IntegrityCheckers.h"
#include "Headers/LinkedList.h"

#define PATH "C:\\vdifiles\\VDITestFiles\\Good\\Test-fixed-1k.vdi"
#define PATH_U "C:\\Users\\crisc\\VirtualBox VMs\\Ubuntu\\Ubuntu.vdi"

int main(int argc, char** argv) {

    //TODO: configure command line arguments for file path

    VDIFile* vdi = vdiOpen(PATH);
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
                      badBGDescriptors, badSuperblocks, duplicateBlocks))
    {
        // do more stuff
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