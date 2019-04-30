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

    //check copies of superblock and block group descriptor tables
    struct node* badSuperblocks = NULL;
    struct node* badBlockGroupDescriptors = NULL;

    for(uint32_t i = 1; i<vdi->superBlock->numBlockGroups; i++)
    {
        if(checkPowerOf(3, i) || checkPowerOf(5, i) || checkPowerOf(7, i))
        {
            for(size_t j = 0; j<3; j++)
            {
                addBlock(vdi, bitmaps, i*vdi->superBlock->blocksPerGroup + j);
            }
            uint8_t superBlockCopy[vdi->superBlock->blockSize];
            uint8_t blockGroupDescriptorCopy[vdi->superBlock->blockSize];
            fetchBlock(vdi, superBlockCopy, i*vdi->superBlock->blocksPerGroup+1);
            fetchBlock(vdi, blockGroupDescriptorCopy, i*vdi->superBlock->blocksPerGroup + 2);
            printBytes(vdi->superBlock->fullArray, 1024, "original bgdt");
            printBytes(superBlockCopy, 1024, "copy");
            if(memcmp(vdi->superBlock->fullArray, superBlockCopy, SUPERBLOCK_SIZE) != 0)
            {
                add(badSuperblocks, i);
            }
            if(memcmp(vdi->BlockGroupDescriptorFullContents, blockGroupDescriptorCopy, vdi->superBlock->blockSize) != 0)
            {
                add(badSuperblocks, i);
            }

        }
    }

    struct node* reachable = NULL;
    struct node* notReachable = NULL;

    int result = bitmapsCmp(vdi, bitmaps, notReachable, reachable);
    if(result)
    {
        printf("All bitmaps are the same!\n");
    }
    else
    {
        printf("You fuckin suck!\n");
    }

    closeDirectory(dir);
    freeBitmaps(bitmaps, vdi);
}

void traverseAndMark(VDIFile *vdi, Directory *dir, char *name, uint32_t iNodeNumber, Bitmaps *bitmaps)
{
    //printf("##############\nTraversing %s at inode %d\n", name, iNodeNumber);
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
    uint32_t blockGroup = blockNumber/vdi->superBlock->blocksPerGroup;
    uint32_t blockGroupIndex = blockNumber%vdi->superBlock->blocksPerGroup;
    uint32_t byteIndex = blockGroupIndex/8;
    uint32_t bitIndex = blockGroupIndex%8;

    uint8_t temp = 1u << bitIndex;
    bitmaps->blockBitmaps[blockGroup][byteIndex] |= temp;
}

int bitmapsCmp(VDIFile *vdi, Bitmaps *bitmaps, struct node *notReachable, struct node *reachable)
{
    int good = 1;
    for(size_t blockGroup = 0; blockGroup<vdi->superBlock->numBlockGroups; blockGroup++)
    {
        uint8_t freeInodeBitmap[1024];
        fetchBlock(vdi, freeInodeBitmap, vdi->blockGroupDescriptorTable[blockGroup]->inodeUsageBitmap);

        uint8_t freeBlockBitmap[1024];
        fetchBlock(vdi, freeBlockBitmap, vdi->blockGroupDescriptorTable[blockGroup]->blockUsageBitmap);
//        printf("##################################\n");
//        printBytes(freeBlockBitmap, vdi->superBlock->blocksPerGroup/8, "original");
//        printBytes(bitmaps->blockBitmaps[blockGroup], vdi->superBlock->blocksPerGroup/8, "new");
        if(memcmp(freeBlockBitmap, bitmaps->blockBitmaps[blockGroup], vdi->superBlock->blocksPerGroup/8) != 0)
        {
            good = 0;
        }
        for(uint32_t byteIndex = 0; byteIndex<vdi->superBlock->inodesPerGroup/8; byteIndex++)
        {
            if(bitsCmp(vdi, notReachable, reachable, freeInodeBitmap[byteIndex], bitmaps->iNodeBitmaps[blockGroup][byteIndex], byteIndex, blockGroup) == 0)
            {
                good = 0;
            }
        }
    }
    return good;
}

int bitsCmp(VDIFile* vdi, struct node* notReachable, struct node* reachable, uint8_t original, uint8_t new, uint32_t byteNum, uint32_t blockGroup)
{
    int count = 1;
    for(uint32_t i = 0; i<8; i++)
    {
        uint32_t originalCurrentBit = (original >> i) & 1u;
        uint32_t newCurrentBit = (new >> i) & 1u;
        if((originalCurrentBit) == 0 &&  (newCurrentBit == 1))
        {
            uint32_t blockGroupIndex = byteNum*8+i;
            uint32_t iNodeNumber = blockGroup*vdi->superBlock->inodesPerGroup + blockGroupIndex + 1;
            reachable->add(reachable, iNodeNumber);
            count = 0;
        }
        if((originalCurrentBit) == 1 &&  (newCurrentBit == 0))
        {
            uint32_t blockGroupIndex = byteNum*8+i;
            uint32_t iNodeNumber = blockGroup*vdi->superBlock->inodesPerGroup + blockGroupIndex + 1;
            notReachable->add(notReachable, iNodeNumber);
            count = 0;
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
        bitmaps->iNodeBitmaps[i] = (uint8_t*)calloc(vdi->superBlock->blockSize, sizeof(uint8_t));
        bitmaps->blockBitmaps[i] = (uint8_t*)calloc(vdi->superBlock->blockSize, sizeof(uint8_t));
    }
    return bitmaps;
}