#include "../Headers/VDIFile.h"
#define VDI_SET 0
#define VDI_CUR 1
#define VDI_END 2

void vdiClose(VDIFile *vdi);

VDIFile* vdiOpen(char* filename){
    VDIFile * vdi = (VDIFile*)malloc(sizeof (VDIFile));
    vdi->header = (Header*)malloc(sizeof (Header));
    vdi->header->diskGeometry = (DiskGeometry*)malloc( sizeof(DiskGeometry));

    printf("%s\n",filename);
    vdi->f = fopen(filename, "rb");
    if (vdi->f == NULL)
    {
        printf("fuck");
        vdiClose(vdi);
        return NULL;
    }

    //error check fread??
    fread((void *)vdi->header->preHeader, 1, 72, vdi->f);
    fread((void *)&vdi->header->headerSize, 4, 1, vdi->f);
    fread((void *)&vdi->header->imageType, 4, 1, vdi->f);
    fread((void *)&vdi->header->imageFlags, 4, 1, vdi->f);
    fread((void *)vdi->header->imageDescription, 1, 32, vdi->f);
    fseek(vdi->f, 0x154, SEEK_SET);
    fread((void *)&vdi->header->offsetPages, 4, 1, vdi->f);
    fread((void *)&vdi->header->offsetData, 4, 1, vdi->f);
    fread((void *)&vdi->header->diskGeometry->cylinders, 4, 1, vdi->f);
    fread((void *)&vdi->header->diskGeometry->heads, 4, 1, vdi->f);
    fread((void *)&vdi->header->diskGeometry->sectors, 4, 1, vdi->f);
    fread((void *)&vdi->header->diskGeometry->sectorSize, 4, 1, vdi->f);
    fread((void *)&vdi->header->unused, 4, 1, vdi->f);
    fread((void *)&vdi->header->diskSize, 8, 1, vdi->f);
    fread((void *)&vdi->header->pageSize, 4, 1, vdi->f);
    fread((void *)&vdi->header->pageExtraData, 4, 1, vdi->f);
    fread((void *)&vdi->header->pagesInHDD, 4, 1, vdi->f);
    fread((void *)&vdi->header->pagesAllocated, 4, 1, vdi->f);
    fread((void *)vdi->header->UUID, 1, 16, vdi->f);
    fread((void *)vdi->header->UUIDLastSnap, 1, 16, vdi->f);
    fread((void *)vdi->header->UUIDLink, 1, 16, vdi->f);
    fread((void *)vdi->header->UUIDParent, 1, 16, vdi->f);
    fread((void *)vdi->header->shit, 1, 56, vdi->f);

    // set cursor to boundary of header (1 MB or 0x100000)
    vdi->cursor = 0x100000;

    return vdi;
}

void vdiSeek(VDIFile* vdi, long long offset, int anchor)
{
    if(anchor == VDI_SET)
    {
        vdi->cursor = 0x100000 + offset;
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

void vdiClose(VDIFile* vdi)
{
    free(vdi->header);
    free(vdi->header->diskGeometry);
    fclose(vdi->f);
    free(vdi);
}
