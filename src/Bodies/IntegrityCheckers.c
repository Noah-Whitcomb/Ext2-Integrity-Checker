#include "../Headers/IntegrityCheckers.h"

void readSuperBlock(VDIFile* vdi)
{
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

