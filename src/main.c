//#include <stdio.h>
#include "Headers/VDIFile.h"
#include "Headers/Structs.h"
#include "Headers/Util.h"

#define PATH "C:\\vdifiles\\VDITestFiles\\Good\\Test-fixed-1k.vdi"
#define PATH_U "C:\\Users\\crisc\\VirtualBox VMs\\Ubuntu\\Ubuntu.vdi"

int main(int argc, char** argv) {
    VDIFile* vdi = vdiOpen(PATH);
    if (vdi == NULL)
    {
        printf("Could not read file, fix it you fucking idiot");
        return 1;
    }

    //get superblock
    vdiSeek(vdi, 0x400, VDI_SET);
    uint8_t superblock[1024];
    vdiRead(vdi, superblock, 1024);
    printBytes(superblock, 1024, "Superblock");

    //get block descriptor table
    vdiSeek(vdi,0x800, VDI_SET);
    // uint8_t blockdescriptor[]

    vdiClose(vdi);
    return 0;
}