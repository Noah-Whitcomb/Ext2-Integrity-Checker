#ifndef EXT2_CHECKER_FILESYSTEM_H
#define EXT2_CHECKER_FILESYSTEM_H

#include "VDIFile.h"

#define BLOCK_DESCRIPTOR_SIZE 32
#define SUPERBLOCK_SIZE 1024
#define REWIND_NO_DOTS 24
#define REWIND_DOTS 0

void readSuperBlock(VDIFile* vdi, uint8_t* superblock);
void readBlockDescTable(VDIFile* vdi, uint8_t* blockDescTable);
Inode* fetchInode(VDIFile* vdi, uint32_t iNodeNumber);
void fetchBlockFromInode(VDIFile *vdi, Inode *inode, int blockNum, uint8_t *blockBuf);
void fetchSingle(VDIFile* vdi, Inode* inode, int blockNum, uint8_t* blockBuf, size_t ipb, int start);
void fetchDouble(VDIFile* vdi, Inode* inode, int blockNum, uint8_t* blockBuf, size_t ipb, int start);
void fetchTriple(VDIFile* vdi, Inode* inode, int blockNum, uint8_t* blockBuf, size_t ipb);
Directory* openDirectory(VDIFile *vdi, uint32_t inodeNumber);
uint32_t getNextEntry(VDIFile *vdi, Directory *dir);
void rewindDirectory(Directory* dir, uint32_t location);
void closeDirectory(Directory* dir);

#endif //EXT2_CHECKER_FILESYSTEM_H
