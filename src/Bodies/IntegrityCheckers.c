#include "../Headers/IntegrityCheckers.h"

void makeBitmaps(VDIFile* vdi)
{
    Bitmaps* bitmaps = initializeBitmaps(vdi);

    Directory* dir = openDirectory(vdi, 2);

    // take care of reserved inodes
    for(size_t i = 1; i<=10; i++)
    {
        addInode(vdi, bitmaps, i);
        addBlocksFromInode(vdi, bitmaps, i);
    }

    traverseAndMark(vdi, dir, "root", 2, bitmaps);

    // mark off blocks from block group descriptors
    for(size_t i = 0; i < vdi->superBlock->numBlockGroups; i++)
    {
        addBlock(vdi, bitmaps, vdi->blockGroupDescriptorTable[i]->blockUsageBitmap);
        addBlock(vdi, bitmaps, vdi->blockGroupDescriptorTable[i]->inodeUsageBitmap);
        for(size_t j = 0; j < (128*vdi->superBlock->inodesPerGroup)/vdi->superBlock->blockSize; j++)
        {
            addBlock(vdi, bitmaps, vdi->blockGroupDescriptorTable[i]->inodeTableAddress+j);
        }
        //addBlock(vdi, bitmaps, vdi->blockGroupDescriptorTable[i]->inodeTableAddress);
    }

    // mark off blocks 0, 1, 2 from block group 0
    for(size_t j = 0; j<3; j++)
    {
        addBlock(vdi, bitmaps, j + vdi->superBlock->superBlockNumber);
    }

    //check copies of superblock and block group descriptor tables
    struct List* badSuperblocks = initializeList();
    struct List* badBlockGroupDescriptors = initializeList();

    for(uint32_t i = 1; i<vdi->superBlock->numBlockGroups; i++)
    {
        if(checkPowerOf(3, i) || checkPowerOf(5, i) || checkPowerOf(7, i))
        {
            for(size_t j = 1; j<4; j++)
            {
                addBlock(vdi, bitmaps, i*vdi->superBlock->blocksPerGroup + j);
            }
            uint8_t superBlockCopy[SUPERBLOCK_SIZE];
            uint8_t blockGroupDescriptorCopy[vdi->superBlock->blockSize];
            fetchBlock(vdi, superBlockCopy, i*vdi->superBlock->blocksPerGroup+1);
            fetchBlock(vdi, blockGroupDescriptorCopy, i*vdi->superBlock->blocksPerGroup + 2);
            if(memcmp(vdi->superBlock->fullArray, superBlockCopy, SUPERBLOCK_SIZE) != 0)
            {
                badSuperblocks->add(badSuperblocks, i);
            }
            if(memcmp(vdi->BlockGroupDescriptorFullContents, blockGroupDescriptorCopy, vdi->superBlock->blockSize) != 0)
            {
                badBlockGroupDescriptors->add(badSuperblocks, i);
            }

        }
    }

    struct List* inodesReachable = initializeList();
    struct List* inodesNotReachable = initializeList();
    struct List* blocksReachable = initializeList();
    struct List* blocksNotReachable = initializeList();


    int result = bitmapsCmp(vdi, bitmaps, inodesNotReachable, inodesReachable, blocksNotReachable, blocksReachable);
    if(result)
    {
        printf("All bitmaps are the same!\n");
    }
    else
    {
        printf("You suck!\n");
    }

    struct List* temp = blocksNotReachable;
    printf("Not reachable: \n");
    int count = 0;
    while(temp->head != NULL)
    {
        printf("Block group: %d\t", temp->head->value/vdi->superBlock->blocksPerGroup);
        printf("Block number: %d\n", temp->head->value);
//        uint8_t bl[1024];
//        fetchBlock(vdi, bl, temp->head->value);
//        printBytes(bl, 1024, "block");
        temp->head = temp->head->nextNode;
        count++;
    }
    printf("not reachable count: %d\n", count);
    printf("\n#########\n");
    struct List* temp2 = blocksReachable;
    printf("Reachable:\n");
    int count2 = 0;
    while(temp2->head != NULL)
    {
        printf("Block group: %d\t", temp2->head->value/vdi->superBlock->blocksPerGroup);
        printf("Block number: %d\n", temp2->head->value);
        temp2->head = temp2->head->nextNode;
        count2++;
    }
    printf("reachable count: %d\n", count2);

    closeDirectory(dir);
    freeBitmaps(bitmaps, vdi);
    freeList(badBlockGroupDescriptors);
    freeList(badSuperblocks);
    freeList(inodesReachable);
    freeList(inodesNotReachable);
}

void traverseAndMark(VDIFile *vdi, Directory *dir, char *name, uint32_t iNodeNumber, Bitmaps *bitmaps)
{
    //printf("##############\nTraversing %s at inode %d\n", name, value);
    while(getNextEntry(vdi, dir))
    {
        addInode(vdi, bitmaps, dir->inodeNumber);
        addBlocksFromInode(vdi, bitmaps, dir->inodeNumber);
        Directory* new_dir = openDirectory(vdi, dir->inodeNumber);
        if(new_dir != NULL)
        {
            traverseAndMark(vdi, new_dir, dir->name, dir->inodeNumber, bitmaps);
        }
    }
   // printf("finished traversing [%s]\n", name);
    closeDirectory(dir);
}

void addInode(VDIFile* vdi, Bitmaps* bitmaps, uint32_t iNodeNumber)
{
    uint32_t blockGroup = (iNodeNumber-1)/vdi->superBlock->inodesPerGroup;
    uint32_t blockGroupIndex = (iNodeNumber-1)%vdi->superBlock->inodesPerGroup;
    uint32_t byteIndex = blockGroupIndex/8;
    uint32_t bitIndex = blockGroupIndex%8;

    uint8_t temp = 1u << bitIndex;
    //if(bitIndex == 0) byteIndex--;
    bitmaps->iNodeBitmaps[blockGroup][byteIndex] |= temp;

}

void addBlocksFromInode(VDIFile *vdi, Bitmaps *bitmaps, uint32_t iNodeNumber)
{
    Inode* inode = fetchInode(vdi, iNodeNumber);

    if(inode->fragmentBlockAddress != 0)
    {
        addBlock(vdi, bitmaps, inode->fragmentBlockAddress);
    }

    for (size_t i = 0; i < 15; i++)
    {
        if(inode->pointers[i] != 0)
        {
            addBlock(vdi, bitmaps, inode->pointers[i]);
        }
    }
    if(inode->pointers[12] != 0)
    {
        markSingles(vdi, bitmaps, inode->pointers[12]);
    }
    if(inode->pointers[13] != 0)
    {
        markDouble(vdi, bitmaps, inode->pointers[13]);
    }
    if(inode->pointers[14] != 0)
    {
        markTriple(vdi, bitmaps, inode->pointers[14]);
    }
}

void markSingles(VDIFile* vdi, Bitmaps* bitmaps, uint32_t blockNumber)
{
    uint8_t data[vdi->superBlock->blockSize];
    fetchBlock(vdi, data, blockNumber);

    if(blockNumber == 2766)
    {
        printBytes(data, 1024, "block 2766");
    }
    for(size_t i = 0; i<vdi->superBlock->blockSize/4; i++)
    {
        uint32_t temp;
        memcpy(&temp, data +i*4, 4);
        if(temp != 0)
        {
            addBlock(vdi, bitmaps, temp);
        }
    }
}

void markDouble(VDIFile* vdi, Bitmaps* bitmaps, uint32_t blockNumber)
{
    uint8_t data[vdi->superBlock->blockSize];
    fetchBlock(vdi, data, blockNumber);

    if(blockNumber == 530)
    {
        printBytes(data, 1024, "block 530");
    }

    for(size_t i = 0; i<vdi->superBlock->blockSize/4; i++)
    {
        uint32_t temp;
        memcpy(&temp, data + i*4, 4);
        if(temp != 0)
        {
            addBlock(vdi, bitmaps, temp);
            markSingles(vdi, bitmaps, temp);
        }
    }
}

void markTriple(VDIFile* vdi, Bitmaps* bitmaps, uint32_t blockNumber)
{
    uint8_t data[vdi->superBlock->blockSize];
    fetchBlock(vdi, data, blockNumber);

    for(size_t i = 0; i<vdi->superBlock->blockSize/4; i++)
    {
        uint32_t temp;
        memcpy(&temp, data +i*4, 4);
        if(temp != 0)
        {
            addBlock(vdi, bitmaps, temp);
            markDouble(vdi, bitmaps, temp);
        }
    }
}

void addBlock(VDIFile* vdi, Bitmaps* bitmaps, uint32_t blockNumber)
{
    if(blockNumber == 130048)
    {
        int n = 5;
        printf("%d", n);
    }
    // WHY DOES THIS WORK????
    blockNumber -= vdi->superBlock->superBlockNumber;
    // this takes away almost all of out problems but I hate it
    uint32_t blockGroup = blockNumber/vdi->superBlock->blocksPerGroup;
    uint32_t blockIndex = blockNumber%vdi->superBlock->blocksPerGroup;
    uint32_t byteIndex = blockIndex/8;
    uint32_t bitIndex = blockIndex%8;

    uint8_t temp = 1u << bitIndex;
    uint8_t t = bitmaps->blockBitmaps[blockGroup][byteIndex];
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
        printf("##################################\n");
        printBytes(freeBlockBitmap, vdi->superBlock->blocksPerGroup/8, "original");
        printBytes(bitmaps->blockBitmaps[blockGroup], vdi->superBlock->blocksPerGroup/8, "new");

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
                printf("bad inodes!\n");
            }
        }
    }
    // Handle last block group
    uint8_t freeInodeBitmap[1024];
    fetchBlock(vdi, freeInodeBitmap, vdi->blockGroupDescriptorTable[vdi->superBlock->numBlockGroups-1]->inodeUsageBitmap);

    uint8_t freeBlockBitmap[1024];
    fetchBlock(vdi, freeBlockBitmap, vdi->blockGroupDescriptorTable[vdi->superBlock->numBlockGroups-1]->blockUsageBitmap);

    uint32_t numBlocks = vdi->superBlock->totalBlocks%vdi->superBlock->blocksPerGroup-vdi->superBlock->superBlockNumber;
    uint32_t numBytesBlocks = numBlocks/8;
    uint32_t bitsInLastBlockByte = numBlocks%8;

    printf("##################################\n");
    printBytes(freeBlockBitmap, numBytesBlocks+1, "original");
    printBytes(bitmaps->blockBitmaps[vdi->superBlock->numBlockGroups-1], numBytesBlocks+1, "new");

   // if(bitsInLastBlockByte == 0) bitsInLastBlockByte = 8;
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
    uint32_t numBytesInodes = numInodes/8;
    uint32_t bitsInLastInodeByte = numInodes%8;
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
                printf("bad inodes!\n");
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
            if(byteNum == 135)
            {
                //printf("##############\n");
                printf("byte 135 original bit: %d \t new bit: %d\n", originalCurrentBit, newCurrentBit);
            }
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