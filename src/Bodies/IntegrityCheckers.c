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
    //number of times mounted since last consistency
    fread((void*)&vdi->superPage->timesmounted, 2, 1, vdi->f);
    // number of mounts allowed before consistency check
    uint16_t mountsallowed;
    // should be 0xEF53
    uint16_t magic;
    // 1 means clean, 2 means system has errors
    uint16_t systemstate;
    // what to do when there is an error
    // 1 - ignore error, 2 - remount file system as read only, 3 - kernel panic
    uint16_t errorhandler;
    // combine with major portion to get full version
    uint16_t minorversion;
    // POSIX time of last consistency check
    uint32_t lastcheck;
    // interval between forced consistency checks
    uint32_t interval;
    // 0 - linux, 1 - GNU HURD, 2 - MASIX, 3 - FREEBSD, 4 - other "lites"
    uint32_t opsysid;
    uint32_t majorportion;
    // user id that can use reserved blocks
    uint16_t userid;
    // group id that can use reserved blocks
    uint16_t groupid;
}

