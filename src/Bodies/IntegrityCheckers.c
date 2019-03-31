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

