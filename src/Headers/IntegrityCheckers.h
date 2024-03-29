#ifndef EXT2_CHECKER_INTEGRITYCHECKERS_H
#define EXT2_CHECKER_INTEGRITYCHECKERS_H

#include "FileSystem.h"
#include "VDIFile.h"
#include "LinkedList.h"

int integrityCheck(VDIFile *vdi, Bitmaps *bitmaps, struct List *inodesNotReachable, struct List *inodesReachable,
                   struct List *blocksNotReachable, struct List *blocksReachable, struct List *badBGDescriptors,
                   struct List *badSuperblocks, struct List *duplicateBlocks);
void traverseAndMark(VDIFile *vdi, Directory *dir, char *name, uint32_t iNodeNumber, Bitmaps *bitmaps,
                     uint32_t *numDirectories, uint32_t *numNonDirectories, struct List *duplicateBlocks);

void freeBitmaps(Bitmaps* bitmaps, VDIFile* vdi);
Bitmaps* initializeBitmaps(VDIFile* vdi);
void addInode(VDIFile* vdi, Bitmaps* bitmaps, uint32_t iNodeNumber);
int bitmapsCmp(VDIFile *vdi, Bitmaps *bitmaps, struct List *inodesNotReachable, struct List *inodesReachable,
               struct List *blocksNotReachable, struct List *blocksReachable);
int bitsCmpInodes(VDIFile *vdi, struct List *notReachable, struct List *reachable, uint8_t original, uint8_t new,
            uint32_t byteNum, uint32_t blockGroup, uint32_t maxBits);
int bitsCmpBlocks(VDIFile *vdi, struct List *notReachable, struct List *reachable, uint8_t original, uint8_t new,
            uint32_t byteNum, uint32_t blockGroup, uint32_t maxBits);

void addBlock(VDIFile *vdi, Bitmaps *bitmaps, uint32_t blockNumber, struct List *duplicateBlocks);
void addBlocksFromInode(VDIFile *vdi, Bitmaps *bitmaps, uint32_t iNodeNumber, struct List *duplicateBlocks);

void markSingles(VDIFile *vdi, Bitmaps *bitmaps, uint32_t blockNumber, struct List *duplicateBlocks);
void markDouble(VDIFile *vdi, Bitmaps *bitmaps, uint32_t blockNumber, struct List *duplicateBlocks);
void markTriple(VDIFile *vdi, Bitmaps *bitmaps, uint32_t blockNumber, struct List *duplicateBlocks);

void fixBadCopies(VDIFile* vdi, struct List* badSuperblocks, struct List* badBGDescriptors);
void fixBitmaps(VDIFile* vdi, Bitmaps* bitmaps);

#endif //EXT2_CHECKER_INTEGRITYCHECKERS_H
