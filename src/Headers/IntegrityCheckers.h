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
int bitmapsCmp(VDIFile *vdi, Bitmaps *bitmaps, struct node *notReachable, struct node *reachable);
int bitsCmp(VDIFile* vdi, struct node* notReachable, struct node* reachable, uint8_t original, uint8_t new, uint32_t byteNum, uint32_t blockGroup);
void addBlock(VDIFile* vdi, Bitmaps* bitmaps, uint32_t blockNumber);
void addBlocksFromInode(VDIFile *vdi, Bitmaps *bitmaps, uint32_t iNodeNumber);

void markSingles(VDIFile* vdi, Bitmaps* bitmaps, uint32_t blockNumber);
void markDouble(VDIFile* vdi, Bitmaps* bitmaps, uint32_t blockNumber);
void markTriple(VDIFile* vdi, Bitmaps* bitmaps, uint32_t blockNumber);

#endif //EXT2_CHECKER_INTEGRITYCHECKERS_H
