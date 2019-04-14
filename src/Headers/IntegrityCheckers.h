#ifndef EXT2_CHECKER_INTEGRITYCHECKERS_H
#define EXT2_CHECKER_INTEGRITYCHECKERS_H

#include "VDIFile.h"

#define BLOCK_DESCRIPTOR_SIZE 32

void readSuperBlock(VDIFile* vdi, uint8_t* superblock);
void readBlockDescTable(VDIFile* vdi, uint8_t* blockDescTable);
Inode* fetchInode(VDIFile* vdi, uint32_t iNodeNumber);
void fetchBlockFromFile(VDIFile* vdi, Inode* inode, int blockNum, uint8_t* blockBuf);
void fetchSingle(VDIFile* vdi, Inode* inode, int blockNum, uint8_t* blockBuf, size_t ipb, int start);
void fetchDouble(VDIFile* vdi, Inode* inode, int blockNum, uint8_t* blockBuf, size_t ipb, int start);
void fetchTriple(VDIFile* vdi, Inode* inode, int blockNum, uint8_t* blockBuf, size_t ipb);

#endif //EXT2_CHECKER_INTEGRITYCHECKERS_H
