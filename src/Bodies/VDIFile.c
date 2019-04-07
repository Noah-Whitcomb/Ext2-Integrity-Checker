#include "../Headers/VDIFile.h"

void vdiClose(VDIFile *vdi);

VDIFile* vdiOpen(char* filename){
    VDIFile * vdi = (VDIFile*)malloc(sizeof (VDIFile));
    vdi->header = (Header*)malloc(sizeof (Header));
    vdi->header->diskGeometry = (DiskGeometry*)malloc( sizeof(DiskGeometry));
    vdi->superPage = (SuperPage*)malloc(sizeof(SuperPage));
    vdi->blockGroupDescriptorTable = NULL;

    printf("%s\n",filename);
    vdi->f = fopen(filename, "rb");
    if (vdi->f == NULL)
    {
        printf("fuck");
        vdiClose(vdi);
        return NULL;
    }

    //error check fread??
    fread(vdi->header->preHeader, 1, 72, vdi->f);
    fread(&vdi->header->headerSize, 4, 1, vdi->f);
    fread(&vdi->header->imageType, 4, 1, vdi->f);
    fread(&vdi->header->imageFlags, 4, 1, vdi->f);
    fread(vdi->header->imageDescription, 1, 32, vdi->f);
    fseek(vdi->f, 0x154, SEEK_SET);
    fread(&vdi->header->offsetPages, 4, 1, vdi->f);
    fread(&vdi->header->offsetData, 4, 1, vdi->f);

    //TODO make sure this is good (ask kramer sometime)
    // make sure offset data accounts for header boundary (1 MB or 0x100000)
    vdi->header->offsetData += 0x100000;

    fread(&vdi->header->diskGeometry->cylinders, 4, 1, vdi->f);
    fread(&vdi->header->diskGeometry->heads, 4, 1, vdi->f);
    fread(&vdi->header->diskGeometry->sectors, 4, 1, vdi->f);
    fread(&vdi->header->diskGeometry->sectorSize, 4, 1, vdi->f);
    fread(&vdi->header->unused, 4, 1, vdi->f);
    fread(&vdi->header->diskSize, 8, 1, vdi->f);
    fread(&vdi->header->pageSize, 4, 1, vdi->f);
    fread(&vdi->header->pageExtraData, 4, 1, vdi->f);
    fread(&vdi->header->pagesInHDD, 4, 1, vdi->f);
    fread(&vdi->header->pagesAllocated, 4, 1, vdi->f);
    fread(vdi->header->UUID, 1, 16, vdi->f);
    fread(vdi->header->UUIDLastSnap, 1, 16, vdi->f);
    fread(vdi->header->UUIDLink, 1, 16, vdi->f);
    fread(vdi->header->UUIDParent, 1, 16, vdi->f);
    fread(vdi->header->shit, 1, 56, vdi->f);

    return vdi;
}

void vdiSeek(VDIFile* vdi, long long offset, int anchor)
{
    if(anchor == VDI_SET)
    {
        vdi->cursor = vdi->header->offsetData + offset;
    }
    if(anchor == VDI_CUR)
    {
        vdi->cursor += offset;
    }
    if(anchor == VDI_END)
    {
        // cursor should be negative
        vdi->cursor = offset + vdi->header->diskSize;
    }
}

// call vdiSeek before vdiRead at all times
void vdiRead(VDIFile* vdi, uint8_t* buffer, size_t nbytes)
{
    // TODO: change this to dynamic if we get to it
    long long page = vdi->cursor/vdi->header->pageSize;
    long long offset = vdi->cursor%vdi->header->pageSize;

    long long position = page*vdi->header->pageSize+offset;
    fseek(vdi->f, position, SEEK_SET);
    fread(buffer, 1, nbytes, vdi->f);
}

void vdiClose(VDIFile* vdi)
{
    if(vdi->blockGroupDescriptorTable != NULL)
    {
        for (size_t i = 0; i < vdi->superPage->numpagegroups; i++)
        {
            free(vdi->blockGroupDescriptorTable[i]);
        }
        free(vdi->blockGroupDescriptorTable);
    }

    free(vdi->header->diskGeometry);
    free(vdi->header);
    free(vdi->superPage);
    fclose(vdi->f);
    free(vdi);
}


