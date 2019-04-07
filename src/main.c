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

    //get superBlock
    vdiSeek(vdi, 0x400, VDI_SET);
    uint8_t* superBlock = (uint8_t*)malloc(SUPERBLOCK_SIZE);
    vdiRead(vdi, superBlock, SUPERBLOCK_SIZE);
    readSuperBlock(vdi, superBlock);
    free(superBlock);

    // check magic of ext2 filesystem
    if (vdi->superPage->magic != 0xef53)
    {
        printBytes((uint8_t*)&vdi->superPage->magic, 2, "Magic");
        printf("fuck, you suck at programming and life. The magic isn't right. \n holy fuck you should have gotten this already");
        vdiClose(vdi);
        return 1;
    }

    printf("ext2 page size: %d\n",vdi->superPage->pageSize);

    //get block descriptor table
    vdiSeek(vdi,vdi->superPage->pageSize, VDI_CUR);
    uint8_t* blockDescTable = (uint8_t*)calloc(BLOCK_DESCRIPTOR_SIZE, vdi->superPage->numpagegroups);
    vdiRead(vdi, blockDescTable, BLOCK_DESCRIPTOR_SIZE*vdi->superPage->numpagegroups);
    readBlockDescTable(vdi, blockDescTable);
    free(blockDescTable);

    for(size_t i = 0;i<vdi->superPage->numpagegroups;i++)
    {
        printf("number of directories in group %d: %d\n", i, vdi->blockGroupDescriptorTable[i]->numDirectories);
    }

    vdiClose(vdi);
    return 0;
}