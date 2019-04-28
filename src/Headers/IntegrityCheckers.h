#ifndef EXT2_CHECKER_INTEGRITYCHECKERS_H
#define EXT2_CHECKER_INTEGRITYCHECKERS_H

#include "FileSystem.h"
#include "VDIFile.h"

void makeBitmaps(VDIFile* vdi);
void traverseAndMark(VDIFile *vdi, Directory *dir, char *name, uint32_t iNodeNumber, Bitmaps *bitmaps);

void freeBitmaps(Bitmaps* bitmaps, VDIFile* vdi);
Bitmaps* initializeBitmaps(VDIFile* vdi);
void addInode(VDIFile* vdi, Bitmaps* bitmaps, uint32_t iNodeNumber);
int bitmapsCmp(VDIFile* vdi, Bitmaps* bitmaps);

#endif //EXT2_CHECKER_INTEGRITYCHECKERS_H
