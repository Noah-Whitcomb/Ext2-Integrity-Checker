//#include <stdio.h>
#include "Headers/VDIFile.h"
#include "Headers/Structs.h"
#include "Headers/Util.h"
#include "Headers/FileSystem.h"
#include "Headers/IntegrityCheckers.h"
#include "Headers/LinkedList.h"

#define PATH "C:\\vdifiles\\VDITestFiles\\Good\\Test-fixed-1k.vdi"
#define PATH_U "C:\\Users\\crisc\\VirtualBox VMs\\Ubuntu\\Ubuntu.vdi"

int main(int argc, char** argv) {

    //TODO: configure command line arguments for file path

    VDIFile* vdi = vdiOpen(PATH);
    if (vdi == NULL)
    {
        printf("Could not read file, fix it you fucking idiot");
        return 1;
    }

    //get superBlock, 1024 is superblock size always
    uint8_t superBlock [SUPERBLOCK_SIZE];
    vdiSeek(vdi, 0x400, VDI_SET);
    vdiRead(vdi, superBlock, SUPERBLOCK_SIZE);
    readSuperBlock(vdi, superBlock);

    // check magic of ext2 filesystem
    if (vdi->superBlock->magic != 0xef53)
    {
        printBytes((uint8_t*)&vdi->superBlock->magic, 2, "Magic");
        printf("fuck, you suck at programming and life. The magic isn't right. \n holy fuck you should have gotten this already");
        vdiClose(vdi);
        return 1;
    }


    //get block descriptor table
    uint8_t blockDescTable [vdi->superBlock->blockSize];
    fetchBlock(vdi, blockDescTable, 2);
    readBlockDescTable(vdi, blockDescTable);

//    for(size_t i = 0;i<vdi->superBlock->numBlockGroups;i++)
//    {
//        printf("inode table address of block group %d: %d\n", i, vdi->blockGroupDescriptorTable[i]->inodeTableAddress);
//    }

//    uint8_t iNodeBuffer[128];
//    for(size_t i = 1;i<999; i++)
//    {
//        printf("#######\nInode %d\n", i);
//        fetchInode(vdi, iNodeBuffer, i);
//    }

    //Inode* lost = fetchInode(vdi, 11);

    makeBitmaps(vdi);

    vdiClose(vdi);
//    struct List* list = initializeList();
//    add(list, 6969);
//    add(list, 696969);
//    add(list, 42069);
//    add(list, 5);
//    struct node* temp = list->head;
//    while(temp != NULL)
//    {
//        printf("value: %d\n", temp->value);
//        temp = temp->nextNode;
//    }
//    freeList(list);
    return 0;
}