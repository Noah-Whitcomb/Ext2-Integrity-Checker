#include "../Headers/IntegrityCheckers.h"

void makeBitmaps(VDIFile* vdi)
{
    Inode* inode = fetchInode(vdi, 2);
    Directory* dir = openDirectory(vdi, inode);

    traverseAndMark(vdi, dir);

    closeDirectory(dir);
}

void traverseAndMark(VDIFile* vdi, Directory* dir)
{
    uint32_t currentInode;
    while(getNextEntry(vdi, dir))
    {
        Inode* newInode = fetchInode(vdi, dir->inodeNumber);
        Directory* new_dir = openDirectory(vdi, newInode);
        if(new_dir != NULL)
        {
            printf("directory name: %s at inode: %d\n recursing...\n", new_dir->name, new_dir->inodeNumber);
            //traverseAndMark(vdi, new_dir);
            printf("################\nfinished traversing [%s]\n", new_dir->name);
            closeDirectory(new_dir);
        }
        //printf("Current inode: %d\n", dir->inodeNumber);
    }
    //    if(type == 2)
//    {
//        printf("directory name: %s at inode: %d\n recursing...\n", name, inode);
//        Inode* newInode = fetchInode(vdi, inode);
//        openDirectory(vdi, newInode);
//        free(newInode);
//        printf("################\nfinished traversing [%s]\n", name);
//        free(name);
//    }
//    return dir->cursor < dir->inode->lower32BitsSize;
}