#include "../Headers/IntegrityCheckers.h"

void readSuperBlock(VDIFile* vdi)
{
    fread((void*)&vdi->superPage->totalinodes, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->totalpages, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->superuserpages, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->unallocatedpages, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->unallocatedinodes, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->superpageknumber, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->log2pagesize, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->log2fragmentsize, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->pagespergroup, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->fragmentspergroup, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->inodespergroup, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->lastmounttime, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->lastwritetime, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->timesmounted, 2, 1, vdi->f);
}

