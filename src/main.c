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
        vdiClose(vdi);
        return 1;
    }

    //get superblock
    vdiSeek(vdi, 0x400, VDI_SET);
    uint8_t* superblock = (uint8_t*)malloc(1024);
    vdiRead(vdi, superblock, 1024);
    readSuperBlock(vdi, superblock);
    free(superblock);


    int i = 1024 << vdi->superPage->log2pagesize;
    printf("page size from superblock: %d\n", i);
    printf("number of page descriptors: %d\n", vdi->superPage->numpagegroups);

    if (vdi->superPage->magic != 0xef53)
    {
        printBytes((uint8_t*)&vdi->superPage->magic, 2, "Magic");
        printf("fuck, you suck at programming and life");
        vdiClose(vdi);
        return 1;
    }

    //get block descriptor table
    vdiSeek(vdi,0x800, VDI_SET);

    vdiClose(vdi);
    return 0;
}