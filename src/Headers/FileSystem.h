#ifndef EXT2_CHECKER_INTEGRITYCHECKERS_H
#define EXT2_CHECKER_INTEGRITYCHECKERS_H

#include "VDIFile.h"

#define BLOCK_DESCRIPTOR_SIZE 32
#define SUPERBLOCK_SIZE 84
#define REWIND_NO_DOTS 24
#define REWIND_DOTS 0

void readSuperBlock(VDIFile* vdi, uint8_t* superblock);
void readBlockDescTable(VDIFile* vdi, uint8_t* blockDescTable);
Inode* fetchInode(VDIFile* vdi, uint32_t iNodeNumber);
void fetchBlockFromInode(VDIFile *vdi, Inode *inode, int blockNum, uint8_t *blockBuf);
void fetchSingle(VDIFile* vdi, Inode* inode, int blockNum, uint8_t* blockBuf, size_t ipb, int start);
void fetchDouble(VDIFile* vdi, Inode* inode, int blockNum, uint8_t* blockBuf, size_t ipb, int start);
void fetchTriple(VDIFile* vdi, Inode* inode, int blockNum, uint8_t* blockBuf, size_t ipb);
void openDirectory(VDIFile *vdi, Inode *inode);
uint32_t getNextEntry(VDIFile *vdi, Directory *dir);
void rewindDirectory(Directory* dir, uint32_t location);

#endif //EXT2_CHECKER_INTEGRITYCHECKERS_H
