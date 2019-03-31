#include "../Headers/IntegrityCheckers.h"

void readSuperBlock(VDIFile* vdi)
{
    uint32_t totalinodes; fread((void*)vdi->superPage->totalinodes, 4, 1, vdi->f);
    uint32_t totalpages;
    uint32_t superuserpages;
    uint32_t unallocatedpages;
    uint32_t unallocatedinodes;
    uint32_t superpageknumber;
    uint32_t log2pagesize;
    uint32_t log2fragmentsize;
    uint32_t pagespergroup;
    uint32_t fragmentspergroup;
    uint32_t inodespergroup;
    uint32_t lastmounttime;
    uint32_t lastwritetime;
    //number of times mounted since last consistency check
    uint16_t timesmounted;
    fread((void*)&vdi->superPage->mountsallowed, 2, 1, vdi->f);
    fread((void*)&vdi->superPage->magic, 2, 1, vdi->f);
    fread((void*)&vdi->superPage->systemstate, 2, 1, vdi->f);
    fread((void*)&vdi->superPage->errorhandler, 2, 1, vdi->f);
    fread((void*)&vdi->superPage->minorversion, 2, 1, vdi->f);
    fread((void*)&vdi->superPage->lastcheck, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->interval, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->opsysid, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->majorportion, 4, 1, vdi->f);
    fread((void*)&vdi->superPage->userid, 2, 1, vdi->f);
    fread((void*)&vdi->superPage->groupid, 2, 1, vdi->f);
}

