#include "../Headers/IntegrityCheckers.h"

void makeBitmaps(VDIFile* vdi)
{

}

void traverseAndMark(VDIFile* vdi, Directory* dir)
{
    while(getNextEntry(vdi, dir))
    {

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