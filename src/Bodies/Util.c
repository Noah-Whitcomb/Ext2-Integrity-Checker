//
// Created by crisc on 1/28/2019.
//

#include "../Headers/Util.h"

void printBytes(const uint8_t* buffer, size_t size, char* description)
{
    printf("\n%s\n", description);
    for(size_t i = 0; i<size; i++)
    {
        printf("%X ", buffer[i]);
    }
    printf("\n");
}

int checkPowerOf(uint32_t power_of, uint32_t number)
{
    uint32_t count = 0;
    while(count < number)
    {
        uint32_t temp = power(power_of, count);
        if(number == temp) return 1;
        if(temp > number) return 0;
        count++;
    }
    return 0;
}

uint32_t power(uint32_t base, uint32_t power)
{
    uint32_t return_val = 1;
    for(uint32_t i = 0; i<power; i++)
    {
        return_val = return_val*base;
    }
    return return_val;
}

void printHeader(Header* head)
{
    printf("PRE HEADER - STRING\n%s\n", head->preHeader);
    printBytes(head->preHeader, 72, "PRE-HEADER - BYTES");

    printf("HEADER SIZE - INT\n%i\n", head->headerSize);
    printBytes((uint8_t *)&head->headerSize, 4, "HEADER SIZE - BYTES");

    printf("IMAGE TYPE - INT\n%i\n", head->imageType);
    printBytes((uint8_t *)&head->imageType, 4, "IMAGE TYPE - BYTES");

    printf("IMAGE FLAGS - INT\n%i\n", head->imageFlags);
    printBytes((uint8_t *)&head->imageFlags, 4, "IMAGE FLAGS - BYTES");

    printBytes(head->imageDescription, 32, "IMAGE DESCRIPTION - BYTES");

    printf("OFFSET BLOCKS - INT\n%i\n", head->offsetPages);
    printBytes((uint8_t *)&head->offsetPages, 4, "OFFSET BLOCKS - BYTES");
}

uint32_t getNumBlockGroups(const VDIFile* vdi)
{
    uint32_t numpagegroups1;
    if(vdi->superBlock->totalBlocks%vdi->superBlock->blocksPerGroup == 0)
    {
        numpagegroups1 = vdi->superBlock->totalBlocks/vdi->superBlock->blocksPerGroup;
    }
    else
    {
        numpagegroups1 =  vdi->superBlock->totalBlocks/vdi->superBlock->blocksPerGroup + 1;
    }

    int numpagegroups2;
    if(vdi->superBlock->totalInodes%vdi->superBlock->inodesPerGroup == 0)
    {
        numpagegroups2 = vdi->superBlock->totalInodes/vdi->superBlock->inodesPerGroup;
    }
    else
    {
        numpagegroups2 =  vdi->superBlock->totalInodes/vdi->superBlock->inodesPerGroup + 1;
    }

    if(numpagegroups1 == numpagegroups2)
    {
        return numpagegroups1;
    }
    else return 0;
}