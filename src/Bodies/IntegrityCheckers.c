#include "../Headers/IntegrityCheckers.h"

void readSuperBlock(VDIFile* vdi, uint8_t* superblock)
{
    memcpy(&vdi->superBlock->totalInodes, superblock, 4);
    memcpy(&vdi->superBlock->totalBlocks, superblock+4, 4);
    memcpy(&vdi->superBlock->superUserBlocks, superblock+8, 4);
    memcpy(&vdi->superBlock->unallocatedBlocks, superblock+12, 4);
    memcpy(&vdi->superBlock->unallocatedInodes, superblock + 16, 4);
    memcpy(&vdi->superBlock->superBlockNumber, superblock + 20, 4);
    memcpy(&vdi->superBlock->log2BlockSize, superblock + 24, 4);
    memcpy(&vdi->superBlock->log2FragmentSize, superblock + 28, 4);
    memcpy(&vdi->superBlock->blocksPerGroup, superblock + 32, 4);
    memcpy(&vdi->superBlock->fragmentsPerGroup, superblock + 36, 4);
    memcpy(&vdi->superBlock->inodesPerGroup, superblock + 40, 4);
    memcpy(&vdi->superBlock->lastMountTime, superblock + 44, 4);
    memcpy(&vdi->superBlock->lastWriteTime, superblock + 48, 4);
    memcpy(&vdi->superBlock->timesMounted, superblock + 52, 2);
    memcpy(&vdi->superBlock->mountsAllowed, superblock + 54, 2);
    memcpy(&vdi->superBlock->magic, superblock + 56, 2);
    memcpy(&vdi->superBlock->systemState, superblock + 58, 2);
    memcpy(&vdi->superBlock->errorHandler, superblock + 60, 2);
    memcpy(&vdi->superBlock->minorVersion, superblock + 62, 2);
    memcpy(&vdi->superBlock->lastCheck, superblock + 66, 4);
    memcpy(&vdi->superBlock->interval, superblock + 70, 4);
    memcpy(&vdi->superBlock->opSysId, superblock + 74, 4);
    memcpy(&vdi->superBlock->majorPortion, superblock + 78, 4);
    memcpy(&vdi->superBlock->userId, superblock + 80, 2);
    memcpy(&vdi->superBlock->groupId, superblock + 82, 2);

    vdi->superBlock->numBlockGroups = getNumBlockGroups(vdi);
    if(vdi->superBlock->numBlockGroups == 0)
    {
        printf("did not get page group number because your mom is dumb");
        assert(1);
    }

    vdi->superBlock->blockSize = 1024u << vdi->superBlock->log2BlockSize;

    //TODO: if majorPortion >= 1, read in extended vdi shit (ask kramer)
//    fread((void*)&vdi->superBlock->totalInodes, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->totalBlocks, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->superUserBlocks, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->unallocatedBlocks, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->unallocatedInodes, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->superBlockNumber, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->log2BlockSize, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->log2FragmentSize, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->blocksPerGroup, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->fragmentsPerGroup, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->inodesPerGroup, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->lastMountTime, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->lastWriteTime, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->timesMounted, 2, 1, vdi->f);
//    fread((void*)&vdi->superBlock->mountsAllowed, 2, 1, vdi->f);
//    fread((void*)&vdi->superBlock->magic, 2, 1, vdi->f);
//    fread((void*)&vdi->superBlock->systemState, 2, 1, vdi->f);
//    fread((void*)&vdi->superBlock->errorHandler, 2, 1, vdi->f);
//    fread((void*)&vdi->superBlock->minorVersion, 2, 1, vdi->f);
//    fread((void*)&vdi->superBlock->lastCheck, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->interval, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->opSysId, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->majorPortion, 4, 1, vdi->f);
//    fread((void*)&vdi->superBlock->userId, 2, 1, vdi->f);
//    fread((void*)&vdi->superBlock->groupId, 2, 1, vdi->f);
}

void readBlockDescTable(VDIFile* vdi, uint8_t* blockDescTable)
{
    vdi->blockGroupDescriptorTable = (BlockGroupDescriptor**)calloc(vdi->superBlock->numBlockGroups, sizeof(BlockGroupDescriptor*));
    for(size_t i = 0; i < vdi->superBlock->numBlockGroups; i++)
    {
        vdi->blockGroupDescriptorTable[i] = (BlockGroupDescriptor*)malloc(sizeof(BlockGroupDescriptor));
    }

    for(size_t i = 0;i<vdi->superBlock->numBlockGroups;i++)
    {
        memcpy(&vdi->blockGroupDescriptorTable[i]->blockUsageBitmap, blockDescTable + i*BLOCK_DESCRIPTOR_SIZE, 4);
        memcpy(&vdi->blockGroupDescriptorTable[i]->inodeUsageBitmap, blockDescTable + i*BLOCK_DESCRIPTOR_SIZE + 4, 4);
        memcpy(&vdi->blockGroupDescriptorTable[i]->inodeTableAddress, blockDescTable + i*BLOCK_DESCRIPTOR_SIZE + 8, 4);
        memcpy(&vdi->blockGroupDescriptorTable[i]->numUnallocatedBlocks, blockDescTable + i*BLOCK_DESCRIPTOR_SIZE + 12, 2);
        memcpy(&vdi->blockGroupDescriptorTable[i]->numUnallocatediNodes, blockDescTable + i*BLOCK_DESCRIPTOR_SIZE + 14, 2);
        memcpy(&vdi->blockGroupDescriptorTable[i]->numDirectories, blockDescTable + i*BLOCK_DESCRIPTOR_SIZE + 16, 2);
    }
}

Inode* fetchInode(VDIFile* vdi, uint32_t iNodeNumber)
{
    uint8_t iNodeBuffer[128];
    uint32_t iNodeSize = 128;
    uint32_t blockGroup = (iNodeNumber-1)/vdi->superBlock->inodesPerGroup;
    uint32_t index = (iNodeNumber-1)%vdi->superBlock->inodesPerGroup;
    uint32_t containingBlock = (index*iNodeSize)/vdi->superBlock->blockSize;

    Inode* inode = (Inode*)malloc(sizeof(Inode));

    // get the block that contains our inode
    uint8_t buf[1024];
    fetchBlock(vdi, buf, containingBlock+vdi->blockGroupDescriptorTable[blockGroup]->inodeTableAddress);

    index = index % 8;

    memcpy(iNodeBuffer, buf+(index)*iNodeSize,iNodeSize);

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
    memcpy(inode->pointers, iNodeBuffer + 40, 60);
    memcpy(&inode->generationNumber, iNodeBuffer + 100, 4);
    memcpy(&inode->reservedField, iNodeBuffer + 104, 4);
    memcpy(&inode->reservedField2, iNodeBuffer + 108, 4);
    memcpy(&inode->fragmentBlockAddress, iNodeBuffer + 112, 4);
    memcpy(inode->opSysValue2, iNodeBuffer + 116, 12);

    return inode;
}

void fetchBlockFromFile(VDIFile* vdi, Inode* inode, int blockNum, uint8_t* blockBuf)
{
    size_t ipb = vdi->superBlock->blockSize/4;

    if(blockNum < 12)
    {
        fetchBlock(vdi, blockBuf, inode->pointers[blockNum]);
        return;
    }

    blockNum -= 12;
    if(blockNum < ipb)
    {
        fetchSingle(vdi, inode, blockNum, blockBuf, ipb, 1);
        return;
    }
    blockNum -= ipb;
    if(blockNum < ipb*ipb)
    {
        fetchDouble(vdi, inode, blockNum, blockBuf, ipb, 1);
        return;
    }
    blockNum -= ipb*ipb;
    if(blockNum < ipb*ipb*ipb)
    {
        fetchTriple(vdi, inode, blockNum, blockBuf, ipb);
    }
}

void fetchSingle(VDIFile* vdi, Inode* inode, int blockNum, uint8_t* blockBuf, size_t ipb, int start)
{
    uint8_t tempBuf[1024];
    if(start)
    {
        fetchBlock(vdi, tempBuf, inode->pointers[12]);
    }
    else
    {
        fetchBlock(vdi, tempBuf, blockNum);
    }

    blockNum %= ipb;
    uint32_t realBlock;
    uint32_t tempval = (blockNum/ipb)*sizeof(uint32_t);
    memcpy(&realBlock, tempBuf + tempval, 4);

    fetchBlock(vdi, blockBuf, realBlock);
}
void fetchDouble(VDIFile* vdi, Inode* inode, int blockNum, uint8_t* blockBuf, size_t ipb, int start)
{
    uint8_t tempBuf[1024];
    if(start)
    {
        fetchBlock(vdi, tempBuf, inode->pointers[13]);
    }
    else
    {
        fetchBlock(vdi, tempBuf, blockNum);
    }

    blockNum %= ipb*ipb;
    uint32_t realBlock;
    memcpy(&realBlock, tempBuf + (blockNum/(ipb*ipb))*sizeof(uint32_t), 4);
    fetchSingle(vdi, inode, realBlock, blockBuf, ipb, 0);
}
void fetchTriple(VDIFile* vdi, Inode* inode, int blockNum, uint8_t* blockBuf, size_t ipb)
{
    uint8_t tempBuf[1024];
    fetchBlock(vdi, tempBuf, inode->pointers[14]);

    uint32_t realBlock;
    memcpy(&realBlock, tempBuf + (blockNum/(ipb*ipb*ipb))*sizeof(uint32_t), 4);
    fetchDouble(vdi, inode, realBlock, blockBuf, ipb, 0);
}