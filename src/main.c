//#include <stdio.h>
#include "Headers/VDIFile.h"
#include "Headers/Structs.h"
#include "Headers/Util.h"

#define PATH "C:\\Users\\crisc\\VirtualBox VMs\\Ubuntu\\Ubuntu.vdi"

int main(int argc, char** argv) {
    VDIFile* vdi = vdiOpen(PATH);
    if (vdi == NULL) return 1;
    printBytes(vdi->header->UUID, 16,"UUID");
    printBytes(vdi->header->shit, 56, "garbage");

    vdiClose(vdi);
    return 0;
}