#include "../Headers/IntegrityCheckers.h"

int integrityCheck(VDIFile *vdi, Bitmaps *bitmaps, struct List *inodesNotReachable, struct List *inodesReachable,
                   struct List *blocksNotReachable, struct List *blocksReachable, struct List *badBGDescriptors,
                   struct List *badSuperblocks, struct List *duplicateBlocks)
{
    int isGoodFileSystem = 1;
    uint32_t numDirectories = 0;
    uint32_t numNonDirectories = 0;

    Directory* dir = openDirectory(vdi, 2);

    // take care of reserved inodes
    for(size_t i = 1; i<=10; i++)
    {
        addInode(vdi, bitmaps, i);
        addBlocksFromInode(vdi, bitmaps, i, duplicateBlocks);
    }

    traverseAndMark(vdi, dir, "root", 2, bitmaps, &numDirectories, &numNonDirectories, duplicateBlocks);

    // mark off blocks from block group descriptors
    for(size_t i = 0; i < vdi->superBlock->numBlockGroups; i++)
    {
        addBlock(vdi, bitmaps, vdi->blockGroupDescriptorTable[i]->blockUsageBitmap, duplicateBlocks);
        addBlock(vdi, bitmaps, vdi->blockGroupDescriptorTable[i]->inodeUsageBitmap, duplicateBlocks);
        for(size_t j = 0; j < (128*vdi->superBlock->inodesPerGroup)/vdi->superBlock->blockSize; j++)
        {
            addBlock(vdi, bitmaps, vdi->blockGroupDescriptorTable[i]->inodeTableAddress+j, duplicateBlocks);
        }
    }

    // mark off blocks 0, 1, 2 from block group 0
    for(size_t j = 0; j<2; j++)
    {
        addBlock(vdi, bitmaps, j + vdi->superBlock->superBlockNumber, duplicateBlocks);
    }

    //check copies of superblock and block group descriptor tables
    for(uint32_t i = 1; i<vdi->superBlock->numBlockGroups; i++)
    {
        if(checkPowerOf(3, i) || checkPowerOf(5, i) || checkPowerOf(7, i))
        {
            for(size_t j = 0; j<2; j++)
            {
                addBlock(vdi, bitmaps, i*vdi->superBlock->blocksPerGroup + j + vdi->superBlock->superBlockNumber, duplicateBlocks);
            }
            uint8_t superBlockCopy[SUPERBLOCK_SIZE];
            uint8_t blockGroupDescriptorCopy[vdi->superBlock->blockSize];
            fetchBlock(vdi, superBlockCopy, i*vdi->superBlock->blocksPerGroup+1);
            fetchBlock(vdi, blockGroupDescriptorCopy, i*vdi->superBlock->blocksPerGroup + 2);
            if(memcmp(vdi->superBlock->fullArray, superBlockCopy, SUPERBLOCK_SIZE) != 0)
            {
                badSuperblocks->add(badSuperblocks, i);
                isGoodFileSystem = 0;
            }
            if(memcmp(vdi->BlockGroupDescriptorFullContents, blockGroupDescriptorCopy, vdi->superBlock->blockSize) != 0)
            {
                badBGDescriptors->add(badBGDescriptors, i);
                isGoodFileSystem = 0;
            }

        }
    }

    // START INTEGRITY CHECKING
    // check bitmaps
    if(bitmapsCmp(vdi, bitmaps, inodesNotReachable, inodesReachable, blocksNotReachable, blocksReachable) == 0)
    {
        isGoodFileSystem = 0;
    }

    //find total directories from block group descriptor table
    uint32_t totalDirectories = 0;
    for(uint32_t i = 0; i<vdi->superBlock->numBlockGroups; i++)
    {
        totalDirectories += vdi->blockGroupDescriptorTable[i]->numDirectories;
    }
    // print summary
    printf("##############\nFields reported by superblock/block group descriptor table\n##############\n");
    printf("Total file system size: %d bytes\n", vdi->superBlock->totalBlocks*vdi->superBlock->blockSize);
    printf("Total space unused: %d bytes\n", vdi->superBlock->unallocatedBlocks*vdi->superBlock->blockSize);
    printf("Total space used: %d bytes\n", (vdi->superBlock->totalBlocks-vdi->superBlock->unallocatedBlocks)*vdi->superBlock->blockSize);
    printf("Total inodes: %d\n", vdi->superBlock->totalInodes-vdi->superBlock->unallocatedInodes);
    printf("Number of existing Directories: %d\n", totalDirectories);
    printf("Number of block groups: %d\n", vdi->superBlock->numBlockGroups);
    printf("------------------------\n");
    for(size_t i = 0; i<vdi->superBlock->numBlockGroups; i++)
    {
        printf("   Block group %d:\n", i);
        printf("    Block address of block usage bitmap: %d\n", vdi->blockGroupDescriptorTable[i]->blockUsageBitmap);
        printf("    Block address of inode usage bitmap: %d\n", vdi->blockGroupDescriptorTable[i]->inodeUsageBitmap);
        printf("    Block address of inode table: %d\n", vdi->blockGroupDescriptorTable[i]->inodeTableAddress);
        printf("    Number of unallocated blocks in group: %d\n", vdi->blockGroupDescriptorTable[i]->numUnallocatedBlocks);
        printf("    Number of unallocated inodes in group: %d\n", vdi->blockGroupDescriptorTable[i]->numUnallocatediNodes);
        printf("    Number of directories in group: %d\n", vdi->blockGroupDescriptorTable[i]->numDirectories);
    }
    printf("------------------------\n");
    printf("##############\nIntegrity check summary\n##############\n");
    if(vdi->superBlock->magic == 0xef53)
    {
        printf("Superblock magic value is correct\n");
    }
    else
    {
        isGoodFileSystem = 0;
        printf("Superblock magic value (0xEF53) is incorrect\n");
    }
    if(badSuperblocks->size == 0)
    {
        printf("All superblock copies are consistent\n");
    }
    else
    {
        isGoodFileSystem = 0;
        printf("Bad superblock copies in block groups: ");
        printList(badSuperblocks);
    }
    if(badBGDescriptors->size == 0)
    {
        printf("All block group descriptor table copies are consistent\n");
    }
    else
    {
        isGoodFileSystem = 0;
        printf("Bad block group descriptor table copies in block groups: ");
        printList(badBGDescriptors);
    }
    if(inodesNotReachable->size == 0)
    {
        printf("All inodes reachable from root\n");
    }
    else
    {
        isGoodFileSystem = 0;
        printf("Inodes marked as used but not reachable: ");
        printList(inodesNotReachable);
    }
    if(inodesReachable->size == 0)
    {
        printf("No inodes marked as unused but reachable from root\n");
    }
    else
    {
        isGoodFileSystem = 0;
        printf("Inodes marked as unused but reachable from root: ");
        printList(inodesReachable);
    }
    if(duplicateBlocks->size == 0)
    {
        printf("No blocks referenced by more than one inode\n");
    }
    else
    {
        isGoodFileSystem = 0;
        printf("Blocks referenced more than once: ");
        printList(duplicateBlocks);
    }
    if(blocksReachable->size == 0)
    {
        printf("No unused blocks referenced by inodes\n");
    }
    else
    {
        isGoodFileSystem = 0;
        printf("Unused blocks referenced by inodes:");
        printList(blocksReachable);
    }
    if((vdi->superBlock->totalInodes-vdi->superBlock->unallocatedInodes) == numNonDirectories+numDirectories+9)
    {
        printf("Total inodes from superblock is correct\n");
    }
    else
    {
        isGoodFileSystem = 0;
        printf("Total inodes from superblock is WRONG - corect value is %d\n", numNonDirectories+numDirectories+9);
    }
    if(totalDirectories == numDirectories)
    {
        printf("Number of directories from block group descriptor table is correct\n");
    }
    else
    {
        printf("Number of directories from block group descriptor table is WRONG - correct value is %d\n", numDirectories);
        isGoodFileSystem = 0;
    }
    if(isGoodFileSystem)
    {
        printf("State of file system is GOOD\n");
    }
    else
    {
        printf("State of file system is BAD\n");
    }
    
    closeDirectory(dir);
    return isGoodFileSystem;
}

void traverseAndMark(VDIFile *vdi, Directory *dir, char *name, uint32_t iNodeNumber, Bitmaps *bitmaps,
                     uint32_t *numDirectories, uint32_t *numNonDirectories, struct List *duplicateBlocks)
{
    (*numDirectories)++;
    //printf("##############\nTraversing %s at inode %d\n", name, value);
    while(getNextEntry(vdi, dir))
    {
        addInode(vdi, bitmaps, dir->inodeNumber);
        addBlocksFromInode(vdi, bitmaps, dir->inodeNumber, duplicateBlocks);
        Directory* new_dir = openDirectory(vdi, dir->inodeNumber);
        if(new_dir != NULL)
        {
            traverseAndMark(vdi, new_dir, dir->name, dir->inodeNumber, bitmaps, numDirectories, numNonDirectories, duplicateBlocks);
            closeDirectory(new_dir);
        }
        else
        {
            (*numNonDirectories)++;
        }
    }
   // printf("finished traversing [%s]\n", name);
}

void addInode(VDIFile* vdi, Bitmaps* bitmaps, uint32_t iNodeNumber)
{
    uint32_t blockGroup = (iNodeNumber-1)/vdi->superBlock->inodesPerGroup;
    uint32_t blockGroupIndex = (iNodeNumber-1)%vdi->superBlock->inodesPerGroup;
    uint32_t byteIndex = blockGroupIndex/8;
    uint32_t bitIndex = blockGroupIndex%8;
    uint8_t temp = 1u << bitIndex;
    bitmaps->iNodeBitmaps[blockGroup][byteIndex] |= temp;

}

void addBlocksFromInode(VDIFile *vdi, Bitmaps *bitmaps, uint32_t iNodeNumber, struct List *duplicateBlocks)
{
    Inode* inode = fetchInode(vdi, iNodeNumber);

    if(inode->fragmentBlockAddress != 0)
    {
        addBlock(vdi, bitmaps, inode->fragmentBlockAddress, duplicateBlocks);
    }

    for (size_t i = 0; i < 15; i++)
    {
        if(inode->pointers[i] != 0)
        {
            addBlock(vdi, bitmaps, inode->pointers[i], duplicateBlocks);
        }
    }
    if(inode->pointers[12] != 0)
    {
        markSingles(vdi, bitmaps, inode->pointers[12], duplicateBlocks);
    }
    if(inode->pointers[13] != 0)
    {
        markDouble(vdi, bitmaps, inode->pointers[13], duplicateBlocks);
    }
    if(inode->pointers[14] != 0)
    {
        markTriple(vdi, bitmaps, inode->pointers[14], duplicateBlocks);
    }
}

void markSingles(VDIFile *vdi, Bitmaps *bitmaps, uint32_t blockNumber, struct List *duplicateBlocks)
{
    uint8_t data[vdi->superBlock->blockSize];
    fetchBlock(vdi, data, blockNumber);

    for(size_t i = 0; i<vdi->superBlock->blockSize/4; i++)
    {
        uint32_t temp;
        memcpy(&temp, data +i*4, 4);
        if(temp != 0)
        {
            addBlock(vdi, bitmaps, temp, duplicateBlocks);
        }
    }
}

void markDouble(VDIFile *vdi, Bitmaps *bitmaps, uint32_t blockNumber, struct List *duplicateBlocks)
{
    uint8_t data[vdi->superBlock->blockSize];
    fetchBlock(vdi, data, blockNumber);

    for(size_t i = 0; i<vdi->superBlock->blockSize/4; i++)
    {
        uint32_t temp;
        memcpy(&temp, data + i*4, 4);
        if(temp != 0)
        {
            addBlock(vdi, bitmaps, temp, duplicateBlocks);
            markSingles(vdi, bitmaps, temp, duplicateBlocks);
        }
    }
}

void markTriple(VDIFile *vdi, Bitmaps *bitmaps, uint32_t blockNumber, struct List *duplicateBlocks)
{
    uint8_t data[vdi->superBlock->blockSize];
    fetchBlock(vdi, data, blockNumber);

    for(size_t i = 0; i<vdi->superBlock->blockSize/4; i++)
    {
        uint32_t temp;
        memcpy(&temp, data +i*4, 4);
        if(temp != 0)
        {
            addBlock(vdi, bitmaps, temp, duplicateBlocks);
            markDouble(vdi, bitmaps, temp, duplicateBlocks);
        }
    }
}

void addBlock(VDIFile *vdi, Bitmaps *bitmaps, uint32_t blockNumber, struct List *duplicateBlocks)
{
    blockNumber -= vdi->superBlock->superBlockNumber;
    uint32_t blockGroup = blockNumber/vdi->superBlock->blocksPerGroup;
    uint32_t blockIndex = blockNumber%vdi->superBlock->blocksPerGroup;
    uint32_t byteIndex = blockIndex/8;
    uint32_t bitIndex = blockIndex%8;

    uint8_t temp = 1u << bitIndex;
    uint8_t t = ((bitmaps->blockBitmaps[blockGroup][byteIndex] >> bitIndex) & 1u) << bitIndex;
    if(temp == t)
    {
        duplicateBlocks->add(duplicateBlocks, blockNumber);
    }
    bitmaps->blockBitmaps[blockGroup][byteIndex] |= temp;
}

int bitmapsCmp(VDIFile *vdi, Bitmaps *bitmaps, struct List *inodesNotReachable, struct List *inodesReachable,
               struct List *blocksNotReachable, struct List *blocksReachable)
{
    int good = 1;

    // Handle all block groups except last one
    for(size_t blockGroup = 0; blockGroup<vdi->superBlock->numBlockGroups-1; blockGroup++)
    {
        uint8_t freeInodeBitmap[1024];
        fetchBlock(vdi, freeInodeBitmap, vdi->blockGroupDescriptorTable[blockGroup]->inodeUsageBitmap);

        uint8_t freeBlockBitmap[1024];
        fetchBlock(vdi, freeBlockBitmap, vdi->blockGroupDescriptorTable[blockGroup]->blockUsageBitmap);
//        printf("##################################\n");
//        printBytes(freeBlockBitmap, vdi->superBlock->blocksPerGroup/8, "original");
//        printBytes(bitmaps->blockBitmaps[blockGroup], vdi->superBlock->blocksPerGroup/8, "new");

        for(uint32_t byteIndex = 0; byteIndex<vdi->superBlock->blocksPerGroup/8; byteIndex++)
        {
            if(bitsCmpBlocks(vdi, blocksNotReachable, blocksReachable, freeBlockBitmap[byteIndex],
                       bitmaps->blockBitmaps[blockGroup][byteIndex], byteIndex, blockGroup, 8) == 0)
            {
                good = 0;
            }
        }
        for(uint32_t byteIndex = 0; byteIndex<vdi->superBlock->inodesPerGroup/8; byteIndex++)
        {
            if(bitsCmpInodes(vdi, inodesNotReachable, inodesReachable, freeInodeBitmap[byteIndex],
                       bitmaps->iNodeBitmaps[blockGroup][byteIndex], byteIndex, blockGroup, 8) == 0)
            {
                good = 0;
            }
        }
    }
    // Handle last block group
    uint8_t freeInodeBitmap[1024];
    fetchBlock(vdi, freeInodeBitmap, vdi->blockGroupDescriptorTable[vdi->superBlock->numBlockGroups-1]->inodeUsageBitmap);

    uint8_t freeBlockBitmap[1024];
    fetchBlock(vdi, freeBlockBitmap, vdi->blockGroupDescriptorTable[vdi->superBlock->numBlockGroups-1]->blockUsageBitmap);

    uint32_t numBlocks = vdi->superBlock->totalBlocks%vdi->superBlock->blocksPerGroup-vdi->superBlock->superBlockNumber;
    if(numBlocks == 0) numBlocks = vdi->superBlock->blocksPerGroup;
    uint32_t numBytesBlocks = numBlocks/8;
    uint32_t bitsInLastBlockByte = numBlocks%8;

//    printf("##################################\n");
//    printBytes(freeBlockBitmap, numBytesBlocks+1, "original");
//    printBytes(bitmaps->blockBitmaps[vdi->superBlock->numBlockGroups-1], numBytesBlocks+1, "new");

    if(bitsInLastBlockByte == 0) bitsInLastBlockByte = 8;
    for(uint32_t byteIndex = 0; byteIndex<=numBytesBlocks; byteIndex++)
    {
        if(byteIndex == numBytesBlocks)
        {
            if(bitsCmpBlocks(vdi, blocksNotReachable, blocksReachable, freeBlockBitmap[byteIndex],
                       bitmaps->blockBitmaps[vdi->superBlock->numBlockGroups - 1][byteIndex], byteIndex,
                       vdi->superBlock->numBlockGroups - 1, bitsInLastBlockByte) == 0)
            {
                good = 0;
            }
        }
        else if(bitsCmpBlocks(vdi, blocksNotReachable, blocksReachable, freeBlockBitmap[byteIndex],
                   bitmaps->blockBitmaps[vdi->superBlock->numBlockGroups - 1][byteIndex], byteIndex,
                   vdi->superBlock->numBlockGroups - 1, 8) == 0)
        {
            good = 0;
        }
    }

    uint32_t numInodes = vdi->superBlock->totalInodes%vdi->superBlock->inodesPerGroup;
    if(numInodes == 0) numInodes = vdi->superBlock->inodesPerGroup;
    uint32_t numBytesInodes = numInodes/8;
    uint32_t bitsInLastInodeByte = numInodes%8;
    if(bitsInLastInodeByte == 0) bitsInLastInodeByte = 8;
    for(uint32_t byteIndex = 0; byteIndex<=numBytesInodes; byteIndex++)
    {
        if(byteIndex == numBytesInodes-1)
        {
            if(bitsCmpInodes(vdi, inodesNotReachable, inodesReachable, freeInodeBitmap[byteIndex],
                       bitmaps->iNodeBitmaps[vdi->superBlock->numBlockGroups - 1][byteIndex], byteIndex,
                       vdi->superBlock->numBlockGroups - 1, bitsInLastInodeByte) == 0)
            {
                good = 0;
            }
        }
        else if (bitsCmpInodes(vdi, inodesNotReachable, inodesReachable, freeInodeBitmap[byteIndex],
                        bitmaps->iNodeBitmaps[vdi->superBlock->numBlockGroups - 1][byteIndex], byteIndex,
                        vdi->superBlock->numBlockGroups - 1, 8) == 0)
            {
                good = 0;
            }
    }


    return good;
}

int bitsCmpInodes(VDIFile *vdi, struct List *notReachable, struct List *reachable, uint8_t original, uint8_t new,
            uint32_t byteNum, uint32_t blockGroup, uint32_t maxBits)
{
    int count = 1;
    for(uint32_t i = 0; i<8; i++)
    {
        if(i < maxBits)
        {
            uint32_t originalCurrentBit = (original >> i) & 1u;
            uint32_t newCurrentBit = (new >> i) & 1u;
            if ((originalCurrentBit == 0) && (newCurrentBit == 1))
            {
                uint32_t blockGroupIndex = byteNum * 8 + i;
                uint32_t iNodeNumber = blockGroup * vdi->superBlock->inodesPerGroup + blockGroupIndex + 1;
                reachable->add(reachable, iNodeNumber);
                count = 0;
            }
            else if ((originalCurrentBit) == 1 && (newCurrentBit == 0))
            {
                uint32_t blockGroupIndex = byteNum * 8 + i;
                uint32_t iNodeNumber = blockGroup * vdi->superBlock->inodesPerGroup + blockGroupIndex + 1;
                notReachable->add(notReachable, iNodeNumber);
                count = 0;
            }
        }
    }
    return count;
}

int bitsCmpBlocks(VDIFile *vdi, struct List *notReachable, struct List *reachable, uint8_t original, uint8_t new,
                  uint32_t byteNum, uint32_t blockGroup, uint32_t maxBits)
{
    int count = 1;
    for(uint32_t i = 0; i<8; i++)
    {
        if(i < maxBits)
        {
            uint32_t originalCurrentBit = (original >> i) & 1u;
            uint32_t newCurrentBit = (new >> i) & 1u;
            if ((originalCurrentBit == 0) && (newCurrentBit == 1))
            {
                uint32_t blockGroupIndex = byteNum * 8 + i;
                uint32_t blockNum = blockGroup * vdi->superBlock->blocksPerGroup + blockGroupIndex;
                reachable->add(reachable, blockNum);
                count = 0;
            }
            else if ((originalCurrentBit) == 1 && (newCurrentBit == 0))
            {
                uint32_t blockGroupIndex = byteNum * 8 + i;
                uint32_t blockNum = blockGroup * vdi->superBlock->blocksPerGroup + blockGroupIndex;
                notReachable->add(notReachable, blockNum);
                count = 0;
            }
        }
    }
    return count;
}

void fixBadCopies(VDIFile* vdi, struct List* badSuperblocks, struct List* badBGDescriptors)
{
    struct node* temp = badSuperblocks->head;
    while(temp != NULL)
    {
        writeBlock(vdi, vdi->superBlock->fullArray, temp->value*vdi->superBlock->blocksPerGroup+1);
        temp = temp->nextNode;
    }

    struct node* temp2 = badBGDescriptors->head;
    while(temp2 != NULL)
    {
        writeBlock(vdi, vdi->BlockGroupDescriptorFullContents, temp2->value*vdi->superBlock->blocksPerGroup+2);
        temp2 = temp2->nextNode;
    }
}

void fixBitmaps(VDIFile* vdi, Bitmaps* bitmaps)
{
    uint32_t totalUnallocatedInodes = 0;
    uint32_t totalUnallocatedBlocks = 0;

    for(uint32_t i = 0; i<vdi->superBlock->numBlockGroups; i++)
    {
        uint32_t inodesCount;
        uint32_t blocksCount;

        uint16_t newUnallocatedBlocks;
        uint16_t newUnallocatedInodes;

        if(i != vdi->superBlock->numBlockGroups-1)
        {
            inodesCount = numEntriesBitmap(bitmaps->iNodeBitmaps[i], vdi->superBlock->inodesPerGroup);
            blocksCount = numEntriesBitmap(bitmaps->blockBitmaps[i], vdi->superBlock->blocksPerGroup);
            newUnallocatedBlocks = vdi->superBlock->blocksPerGroup-blocksCount;
            newUnallocatedInodes = vdi->superBlock->inodesPerGroup-inodesCount;
        }
        else
        {
            uint32_t blocksLastGroup = vdi->superBlock->totalBlocks%vdi->superBlock->blocksPerGroup;
            uint32_t inodesLastGroup = vdi->superBlock->totalInodes%vdi->superBlock->inodesPerGroup;
            if(blocksLastGroup == 0) blocksLastGroup = vdi->superBlock->blocksPerGroup;
            if(inodesLastGroup == 0) inodesLastGroup = vdi->superBlock->inodesPerGroup;
            inodesCount = numEntriesBitmap(bitmaps->iNodeBitmaps[i], inodesLastGroup);
            blocksCount = numEntriesBitmap(bitmaps->blockBitmaps[i], blocksLastGroup);
            newUnallocatedBlocks = blocksLastGroup-blocksCount;
            newUnallocatedInodes = inodesLastGroup-inodesCount;
        }

        totalUnallocatedBlocks += newUnallocatedBlocks;
        totalUnallocatedInodes += newUnallocatedInodes;

        memcpy(vdi->BlockGroupDescriptorFullContents+BLOCK_DESCRIPTOR_SIZE*i+12, (uint8_t *)&newUnallocatedBlocks, 2);
        memcpy(vdi->BlockGroupDescriptorFullContents+BLOCK_DESCRIPTOR_SIZE*i+14, (uint8_t *)&newUnallocatedInodes, 2);

//        writeInt(vdi, newUnallocatedBlocks, vdi->superBlock->superBlockNumber+1, BLOCK_DESCRIPTOR_SIZE*i+12);
//        writeInt(vdi, newUnallocatedInodes, vdi->superBlock->superBlockNumber+1, BLOCK_DESCRIPTOR_SIZE*i+14);

        writeBlock(vdi, bitmaps->blockBitmaps[i], vdi->blockGroupDescriptorTable[i]->blockUsageBitmap);
        writeBlock(vdi, bitmaps->iNodeBitmaps[i], vdi->blockGroupDescriptorTable[i]->inodeUsageBitmap);
    }

    memcpy(vdi->superBlock->fullArray + 12, (uint8_t*)&totalUnallocatedBlocks, 4);
    memcpy(vdi->superBlock->fullArray + 16, (uint8_t*)&totalUnallocatedInodes, 4);

//    writeInt(vdi, totalUnallocatedBlocks, vdi->superBlock->superBlockNumber, 12);
//    writeInt(vdi, totalUnallocatedInodes, vdi->superBlock->superBlockNumber, 16);

    for(uint32_t i = 0; i<vdi->superBlock->numBlockGroups; i++)
    {
        if (checkPowerOf(3, i) || checkPowerOf(5, i) || checkPowerOf(7, i) || i == 0)
        {
            writeBlock(vdi, vdi->superBlock->fullArray, i*vdi->superBlock->blocksPerGroup+ vdi->superBlock->superBlockNumber);
            writeBlock(vdi, vdi->BlockGroupDescriptorFullContents, i*vdi->superBlock->blocksPerGroup + vdi->superBlock->superBlockNumber+1);
        }
    }

}

void freeBitmaps(Bitmaps* bitmaps, VDIFile* vdi)
{
    for(size_t i = 0; i<vdi->superBlock->numBlockGroups; i++)
    {
        free(bitmaps->iNodeBitmaps[i]);
        free(bitmaps->blockBitmaps[i]);
    }
    free(bitmaps->iNodeBitmaps);
    free(bitmaps->blockBitmaps);
    free(bitmaps);
}

Bitmaps* initializeBitmaps(VDIFile* vdi)
{
    Bitmaps* bitmaps = (Bitmaps*)malloc(sizeof(Bitmaps));
    bitmaps->iNodeBitmaps = (uint8_t**)calloc(vdi->superBlock->numBlockGroups, sizeof(uint8_t*));
    bitmaps->blockBitmaps = (uint8_t**)calloc(vdi->superBlock->numBlockGroups, sizeof(uint8_t*));

    for(size_t i = 0; i<vdi->superBlock->numBlockGroups; i++)
    {
        bitmaps->iNodeBitmaps[i] = (uint8_t*)calloc(vdi->superBlock->blockSize, 1);
        bitmaps->blockBitmaps[i] = (uint8_t*)calloc(vdi->superBlock->blockSize, 1);
    }
    return bitmaps;
}