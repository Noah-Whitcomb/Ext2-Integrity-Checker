#include "../Headers/IntegrityCheckers.h"

void readSuperBlock(VDIFile* vdi, uint8_t* superblock)
{
    memcpy(&vdi->superPage->totalinodes, superblock, 4);
    memcpy(&vdi->superPage->totalpages, superblock+4, 4);
    memcpy(&vdi->superPage->superuserpages, superblock+8, 4);
    memcpy(&vdi->superPage->unallocatedpages, superblock+12, 4);
    memcpy(&vdi->superPage->unallocatedinodes, superblock + 16, 4);
    memcpy(&vdi->superPage->superpageknumber, superblock + 20, 4);
    memcpy(&vdi->superPage->log2pagesize, superblock + 24, 4);
    memcpy(&vdi->superPage->log2fragmentsize, superblock + 28, 4);
    memcpy(&vdi->superPage->pagespergroup, superblock + 32, 4);
    memcpy(&vdi->superPage->fragmentspergroup, superblock + 36, 4);
    memcpy(&vdi->superPage->inodespergroup, superblock + 40, 4);
    memcpy(&vdi->superPage->lastmounttime, superblock + 44, 4);
    memcpy(&vdi->superPage->lastwritetime, superblock + 48, 4);
    memcpy(&vdi->superPage->timesmounted, superblock + 52, 2);
    memcpy(&vdi->superPage->mountsallowed, superblock + 54, 2);
    memcpy(&vdi->superPage->magic, superblock + 56, 2);
    memcpy(&vdi->superPage->systemstate, superblock + 58, 2);
    memcpy(&vdi->superPage->errorhandler, superblock + 60, 2);
    memcpy(&vdi->superPage->minorversion, superblock + 62, 2);
    memcpy(&vdi->superPage->lastcheck, superblock + 66, 4);
    memcpy(&vdi->superPage->interval, superblock + 70, 4);
    memcpy(&vdi->superPage->opsysid, superblock + 74, 4);
    memcpy(&vdi->superPage->majorportion, superblock + 78, 4);
    memcpy(&vdi->superPage->userid, superblock + 80, 2);
    memcpy(&vdi->superPage->groupid, superblock + 82, 2);

    vdi->superPage->numpagegroups = getNumBlockGroups(vdi);
    if(vdi->superPage->numpagegroups == 0)
    {
        printf("did not get page group number because your mom is dumb");
        assert(1);
    }

    vdi->superPage->pageSize = 1024u << vdi->superPage->log2pagesize;

    //TODO: if majorportion >= 1, read in extended vdi shit (ask kramer)
//    fread((void*)&vdi->superPage->totalinodes, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->totalpages, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->superuserpages, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->unallocatedpages, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->unallocatedinodes, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->superpageknumber, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->log2pagesize, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->log2fragmentsize, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->pagespergroup, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->fragmentspergroup, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->inodespergroup, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->lastmounttime, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->lastwritetime, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->timesmounted, 2, 1, vdi->f);
//    fread((void*)&vdi->superPage->mountsallowed, 2, 1, vdi->f);
//    fread((void*)&vdi->superPage->magic, 2, 1, vdi->f);
//    fread((void*)&vdi->superPage->systemstate, 2, 1, vdi->f);
//    fread((void*)&vdi->superPage->errorhandler, 2, 1, vdi->f);
//    fread((void*)&vdi->superPage->minorversion, 2, 1, vdi->f);
//    fread((void*)&vdi->superPage->lastcheck, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->interval, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->opsysid, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->majorportion, 4, 1, vdi->f);
//    fread((void*)&vdi->superPage->userid, 2, 1, vdi->f);
//    fread((void*)&vdi->superPage->groupid, 2, 1, vdi->f);
}

void readBlockDescTable(VDIFile* vdi, uint8_t* blockDescTable)
{
    vdi->blockGroupDescriptorTable = (BlockGroupDescriptor**)calloc(vdi->superPage->numpagegroups, sizeof(BlockGroupDescriptor));
    for(size_t i = 0; i < vdi->superPage->numpagegroups; i++)
    {
        vdi->blockGroupDescriptorTable[i] = (BlockGroupDescriptor*)malloc(sizeof(BlockGroupDescriptor));
    }

    for(size_t i = 0;i<vdi->superPage->numpagegroups;i++)
    {
        memcpy(&vdi->blockGroupDescriptorTable[i]->blockUsageBitmap, blockDescTable + i*32, 4);
        memcpy(&vdi->blockGroupDescriptorTable[i]->inodeUsageBitmap, blockDescTable + i*32 + 4, 4);
        memcpy(&vdi->blockGroupDescriptorTable[i]->inodeTableAddress, blockDescTable + i*32 + 8, 4);
        memcpy(&vdi->blockGroupDescriptorTable[i]->numUnallocatedBlocks, blockDescTable + i*32 + 12, 2);
        memcpy(&vdi->blockGroupDescriptorTable[i]->numUnallocatediNodes, blockDescTable + i*32 + 14, 2);
        memcpy(&vdi->blockGroupDescriptorTable[i]->numDirectories, blockDescTable + i*32 + 16, 2);
    }
}
