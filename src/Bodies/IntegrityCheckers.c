#include "../Headers/IntegrityCheckers.h"

void makeBitmaps(VDIFile* vdi)
{
    // initialize new free inode and block bitmaps, calloc sets all values to zero
    Bitmaps* bitmaps = initializeBitmaps(vdi);

    Directory* dir = openDirectory(vdi, 2);

    for(size_t i = 1; i<=10; i++)
    {
        addInode(vdi, bitmaps, i);
    }

    traverseAndMark(vdi, dir, "root", 2, bitmaps);

    int result = bitmapsCmp(vdi, bitmaps);
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

int bitmapsCmp(VDIFile* vdi, Bitmaps* bitmaps)
{
    for(size_t i = 0; i<vdi->superBlock->numBlockGroups; i++)
    {
        uint8_t freeInodeBitmap[1024];
        fetchBlock(vdi, freeInodeBitmap, vdi->blockGroupDescriptorTable[i]->inodeUsageBitmap);
       // printBytes(freeInodeBitmap, 254, "original");
        //printBytes(bitmaps->iNodeBitmaps[i], 254, "new");
        if(memcmp(freeInodeBitmap, bitmaps->iNodeBitmaps[i], vdi->superBlock->inodesPerGroup/8) != 0)
        {
            return 0;
        }
    }
    return 1;
}

int bitsCmp(uint8_t original, uint8_t new)
{

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