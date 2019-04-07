//#include <stdio.h>
#include "Headers/VDIFile.h"
#include "Headers/Structs.h"
#include "Headers/Util.h"
#include "Headers/IntegrityCheckers.h"

#define PATH "C:\\vdifiles\\VDITestFiles\\Good\\Test-fixed-1k.vdi"
#define PATH_U "C:\\Users\\crisc\\VirtualBox VMs\\Ubuntu\\Ubuntu.vdi"

int main(int argc, char** argv) {

    //TODO: configure command line arguments for file path

    VDIFile* vdi = vdiOpen(PATH);
    if (vdi == NULL)
    {
        printf("Could not read file, fix it you fucking idiot");
        return 1;
    }

    //get superBlock, 1024 is superblock size always
    uint8_t superBlock [0x400];
    vdiSeek(vdi, 0x400, VDI_SET);
    vdiRead(vdi, superBlock, 0x400);
    readSuperBlock(vdi, superBlock);

    // check magic of ext2 filesystem
    if (vdi->superPage->magic != 0xef53)
    {
        printBytes((uint8_t*)&vdi->superPage->magic, 2, "Magic");
        printf("fuck, you suck at programming and life. The magic isn't right. \n holy fuck you should have gotten this already");
        vdiClose(vdi);
        return 1;
    }

    printf("ext2 page size: %d\n",vdi->superPage->pageSize);

    //TODO: ask kramer about where block group descriptor table is depending on block size

    //get block descriptor table
    uint8_t blockDescTable [vdi->superPage->pageSize];
    fetchBlock(vdi, blockDescTable, 2);
    readBlockDescTable(vdi, blockDescTable);

    for(size_t i = 0;i<vdi->superPage->numpagegroups;i++)
    {
        printf("inode table address of block group %d: %d\n", i, vdi->blockGroupDescriptorTable[i]->inodeTableAddress);
    }


    uint8_t iNodeBuffer[128];
    fetchInode(vdi, iNodeBuffer, 2);

    vdiClose(vdi);
    return 0;
}