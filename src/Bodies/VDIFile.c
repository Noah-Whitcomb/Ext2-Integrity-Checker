#include "../Headers/VDIFile.h"

void vdiClean(VDIFile* vdi);

VDIFile* vdiOpen(char* filename){
    VDIFile * vdi = (VDIFile*)malloc(sizeof (VDIFile));
    vdi->header = (Header*)malloc(sizeof (Header));

    printf("%s\n",filename);
    vdi->f = fopen(filename, "rb");
    if (vdi->f == NULL)
    {
        printf("fuck");
        vdiClean(vdi);
        return NULL;
    }

    //error check fread??
    fread((void *)vdi->header->preHeader, 1, 72, vdi->f);
    fread((void *)&vdi->header->headerSize, 4, 1, vdi->f);
    fread((void *)&vdi->header->imageType, 4, 1, vdi->f);
    fread((void *)&vdi->header->imageFlags, 4, 1, vdi->f);
    fread((void *)vdi->header->imageDescription, 1, 32, vdi->f);
    fread((void *)&vdi->header->offsetPages, 4, 1, vdi->f);


    return vdi;
}

void vdiClean(VDIFile* vdi)
{
    free(vdi->header);
    fclose(vdi->f);
    free(vdi);
}
