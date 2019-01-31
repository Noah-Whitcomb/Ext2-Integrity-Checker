#include "../Headers/load_file.h"
#define PATH "C:\\Users\\crisc\\VirtualBox VMs\\Ubuntu\\Ubuntu.vdi"


void openFile(Header *head){
    printf("%s\n",PATH);
    FILE *f;
    f = fopen(PATH, "r");
    if (f == NULL) {
        printf("fuck");
        return;
    }

    fread((void *)head->preHeader, 1, 72, f);
    fread((void *)&head->headerSize, 4, 1, f);
    fread((void *)&head->imageType, 4, 1, f);
    fread((void *)&head->imageFlags, 4, 1, f);
    fread((void *)head->imageDescription, 1, 32, f);
    fread((void *)&head->offsetBlocks, 4, 1, f);

    //printf("%d", read);
}
