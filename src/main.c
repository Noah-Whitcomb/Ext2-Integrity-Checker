//#include <stdio.h>
#include "Headers/VDIFile.h"
#include "Headers/Structs.h"
#include "Headers/Util.h"

#define PATH "C:\\Users\\crisc\\VirtualBox VMs\\Ubuntu\\Ubuntu.vdi"

int main() {
    VDIFile* vdi = vdiOpen(PATH);
    if (vdi == NULL) return 1;
    printBytes(vdi->header->UUID, 16,"UUID");

    vdiClose(vdi);
    return 0;
}