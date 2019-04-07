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
        memcpy(&vdi->blockGroupDescriptorTable[i]->blockUsageBitmap, blockDescTable + i*BLOCK_DESCRIPTOR_SIZE, 4);
        memcpy(&vdi->blockGroupDescriptorTable[i]->inodeUsageBitmap, blockDescTable + i*BLOCK_DESCRIPTOR_SIZE + 4, 4);
        memcpy(&vdi->blockGroupDescriptorTable[i]->inodeTableAddress, blockDescTable + i*BLOCK_DESCRIPTOR_SIZE + 8, 4);
        memcpy(&vdi->blockGroupDescriptorTable[i]->numUnallocatedBlocks, blockDescTable + i*BLOCK_DESCRIPTOR_SIZE + 12, 2);
        memcpy(&vdi->blockGroupDescriptorTable[i]->numUnallocatediNodes, blockDescTable + i*BLOCK_DESCRIPTOR_SIZE + 14, 2);
        memcpy(&vdi->blockGroupDescriptorTable[i]->numDirectories, blockDescTable + i*BLOCK_DESCRIPTOR_SIZE + 16, 2);
    }
}

void fetchInode(VDIFile* vdi, uint8_t* iNodeBuffer, uint32_t iNodeNumber)
{
    //TODO: check with kramer about inode size (version 0?)
    uint32_t iNodeSize = 128;
    uint32_t blockGroup = (iNodeNumber-1)/vdi->superPage->inodespergroup;
    uint32_t index = (iNodeNumber-1)%vdi->superPage->inodespergroup;
    uint32_t containingBlock = (index*iNodeSize)/vdi->superPage->pageSize;

    Inode* inode = (Inode*)malloc(sizeof(Inode));

    vdiSeek(vdi, vdi->superPage->pageSize*vdi->blockGroupDescriptorTable[blockGroup]->inodeTableAddress + index*iNodeSize, VDI_SET);
    vdiRead(vdi, iNodeBuffer, iNodeSize);

//    uint8_t buf[1024];
//    fetchBlock(vdi, buf, containingBlock+vdi->blockGroupDescriptorTable[blockGroup]->inodeTableAddress);
//
//    printBytes(iNodeBuffer, 1024, "inodebuffer");

    index = index % 8;

   // memcpy(iNodeBuffer, buf+(index)*128,128);

    memcpy(&inode->typePermissions, iNodeBuffer, 2);
    memcpy(&inode->userId, iNodeBuffer + 2, 2);
    memcpy(&inode->lower32BitsSize, iNodeBuffer + 4, 4);
    memcpy(&inode->lastAccessTime, iNodeBuffer + 8, 4);
    memcpy(&inode->creationTime, iNodeBuffer + 12, 4);
    memcpy(&inode->lastModificationTime, iNodeBuffer + 16, 4);
    memcpy(&inode->deletionTime, iNodeBuffer + 20, 4);
    memcpy(&inode->groupId, iNodeBuffer + 24, 2);
    memcpy(&inode->hardLinkCount, iNodeBuffer + 26, 2);
    memcpy(&inode->diskSectorCount, iNodeBuffer + 28, 4);
    memcpy(&inode->flags, iNodeBuffer + 32, 4);
    memcpy(&inode->opSystemValue1, iNodeBuffer + 36, 4);
    memcpy(&inode->directPointer0, iNodeBuffer + 40, 4);
    memcpy(&inode->directPointer1, iNodeBuffer + 44, 4);
    memcpy(&inode->directPointer2, iNodeBuffer + 48, 4);
    memcpy(&inode->directPointer3, iNodeBuffer + 52, 4);
    memcpy(&inode->directPointer4, iNodeBuffer + 56, 4);
    memcpy(&inode->directPointer5, iNodeBuffer + 60, 4);
    memcpy(&inode->directPointer6, iNodeBuffer + 64, 4);
    memcpy(&inode->directPointer7, iNodeBuffer + 68, 4);
    memcpy(&inode->directPointer8, iNodeBuffer + 72, 4);
    memcpy(&inode->directPointer9, iNodeBuffer + 76, 4);
    memcpy(&inode->directPointer10, iNodeBuffer + 80, 4);
    memcpy(&inode->directPointer11, iNodeBuffer + 84, 4);
    memcpy(&inode->singlyIndirectBlockPointer, iNodeBuffer + 88, 4);
    memcpy(&inode->doublyIndirectBlockPointer, iNodeBuffer + 92, 4);
    memcpy(&inode->triplyIndirectBlockPointer, iNodeBuffer + 96, 4);
    memcpy(&inode->generationNumber, iNodeBuffer + 100, 4);
    memcpy(&inode->reservedField, iNodeBuffer + 104, 4);
    memcpy(&inode->reservedField2, iNodeBuffer + 108, 4);
    memcpy(&inode->fragmentBlockAddress, iNodeBuffer + 112, 4);
    memcpy(inode->opSysValue2, iNodeBuffer + 116, 12);

//    memcpy(&inode->typePermissions, iNodeBuffer + (index-1)*iNodeSize, 2);
//    memcpy(&inode->userId, iNodeBuffer + (index-1)*iNodeSize + 2, 2);
//    memcpy(&inode->lower32BitsSize, iNodeBuffer + (index-1)*iNodeSize + 4, 4);
//    memcpy(&inode->lastAccessTime, iNodeBuffer + (index-1)*iNodeSize + 8, 4);
//    memcpy(&inode->creationTime, iNodeBuffer + (index-1)*iNodeSize + 12, 4);
//    memcpy(&inode->lastModificationTime, iNodeBuffer + (index-1)*iNodeSize + 16, 4);
//    memcpy(&inode->deletionTime, iNodeBuffer + (index-1)*iNodeSize + 20, 4);
//    memcpy(&inode->groupId, iNodeBuffer + (index-1)*iNodeSize + 24, 2);
//    memcpy(&inode->hardLinkCount, iNodeBuffer + (index-1)*iNodeSize + 26, 2);
//    memcpy(&inode->diskSectorCount, iNodeBuffer + (index-1)*iNodeSize + 28, 4);
//    memcpy(&inode->flags, iNodeBuffer + (index-1)*iNodeSize + 32, 4);
//    memcpy(&inode->opSystemValue1, iNodeBuffer + (index-1)*iNodeSize + 36, 4);
//    memcpy(&inode->directPointer0, iNodeBuffer + (index)*iNodeSize + 40, 4);
//    memcpy(&inode->directPointer1, iNodeBuffer + (index)*iNodeSize + 44, 4);
//    memcpy(&inode->directPointer2, iNodeBuffer + (index)*iNodeSize + 48, 4);
//    memcpy(&inode->directPointer3, iNodeBuffer + (index)*iNodeSize + 52, 4);
//    memcpy(&inode->directPointer4, iNodeBuffer + (index-1)*iNodeSize + 56, 4);
//    memcpy(&inode->directPointer5, iNodeBuffer + (index-1)*iNodeSize + 60, 4);
//    memcpy(&inode->directPointer6, iNodeBuffer + (index-1)*iNodeSize + 64, 4);
//    memcpy(&inode->directPointer7, iNodeBuffer + (index-1)*iNodeSize + 68, 4);
//    memcpy(&inode->directPointer8, iNodeBuffer + (index-1)*iNodeSize + 72, 4);
//    memcpy(&inode->directPointer9, iNodeBuffer + (index-1)*iNodeSize + 76, 4);
//    memcpy(&inode->directPointer10, iNodeBuffer + (index-1)*iNodeSize + 80, 4);
//    memcpy(&inode->directPointer11, iNodeBuffer + (index-1)*iNodeSize + 84, 4);
//    memcpy(&inode->singlyIndirectBlockPointer, iNodeBuffer + (index-1)*iNodeSize + 88, 4);
//    memcpy(&inode->doublyIndirectBlockPointer, iNodeBuffer + (index-1)*iNodeSize + 92, 4);
//    memcpy(&inode->triplyIndirectBlockPointer, iNodeBuffer + (index-1)*iNodeSize + 96, 4);
//    memcpy(&inode->generationNumber, iNodeBuffer + (index-1)*iNodeSize + 100, 4);
//    memcpy(&inode->reservedField, iNodeBuffer + (index-1)*iNodeSize + 104, 4);
//    memcpy(&inode->reservedField2, iNodeBuffer + (index-1)*iNodeSize + 108, 4);
//    memcpy(&inode->fragmentBlockAddress, iNodeBuffer + (index-1)*iNodeSize + 112, 4);
//    memcpy(inode->opSysValue2, iNodeBuffer + (index-1)*iNodeSize + 116, 12);

    free(inode);
}
