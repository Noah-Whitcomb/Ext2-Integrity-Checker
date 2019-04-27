#include "../Headers/IntegrityCheckers.h"

void makeBitmaps(VDIFile* vdi)
{
    Directory* dir = openDirectory(vdi, 2);

    traverseAndMark(vdi, dir, "root", 2);

    closeDirectory(dir);
}

void traverseAndMark(VDIFile* vdi, Directory* dir, char* name, uint32_t iNodeNumber)
{
    printf("##############\nTraversing %s at inode %d\n", name, iNodeNumber);
    while(getNextEntry(vdi, dir))
    {
        Directory* new_dir = openDirectory(vdi, dir->inodeNumber);
        if(new_dir != NULL)
        {
            traverseAndMark(vdi, new_dir, dir->name, dir->inodeNumber);
        }
    }
    printf("finished traversing [%s]\n", name);
    closeDirectory(dir);
}