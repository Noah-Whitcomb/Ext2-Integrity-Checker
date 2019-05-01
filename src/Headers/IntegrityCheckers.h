#ifndef EXT2_CHECKER_INTEGRITYCHECKERS_H
#define EXT2_CHECKER_INTEGRITYCHECKERS_H

#include "FileSystem.h"
#include "VDIFile.h"
#include "LinkedList.h"

void makeBitmaps(VDIFile* vdi);
void traverseAndMark(VDIFile *vdi, Directory *dir, char *name, uint32_t iNodeNumber, Bitmaps *bitmaps);

void freeBitmaps(Bitmaps* bitmaps, VDIFile* vdi);
Bitmaps* initializeBitmaps(VDIFile* vdi);
void addInode(VDIFile* vdi, Bitmaps* bitmaps, uint32_t iNodeNumber);
int bitmapsCmp(VDIFile *vdi, Bitmaps *bitmaps, struct List *inodesNotReachable, struct List *inodesReachable,
               struct List *blocksNotReachable, struct List *blocksReachable);
int bitsCmpInodes(VDIFile *vdi, struct List *notReachable, struct List *reachable, uint8_t original, uint8_t new,
            uint32_t byteNum, uint32_t blockGroup, uint32_t maxBits);
int bitsCmpBlocks(VDIFile *vdi, struct List *notReachable, struct List *reachable, uint8_t original, uint8_t new,
            uint32_t byteNum, uint32_t blockGroup, uint32_t maxBits);

void addBlock(VDIFile* vdi, Bitmaps* bitmaps, uint32_t blockNumber);
void addBlocksFromInode(VDIFile *vdi, Bitmaps *bitmaps, uint32_t iNodeNumber);

void markSingles(VDIFile* vdi, Bitmaps* bitmaps, uint32_t blockNumber);
void markDouble(VDIFile* vdi, Bitmaps* bitmaps, uint32_t blockNumber);
void markTriple(VDIFile* vdi, Bitmaps* bitmaps, uint32_t blockNumber);

//copies of superblock and block group descriptor table in block groups copies of 3, 5, 7

#endif //EXT2_CHECKER_INTEGRITYCHECKERS_H
