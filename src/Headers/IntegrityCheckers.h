#ifndef EXT2_CHECKER_INTEGRITYCHECKERS_H
#define EXT2_CHECKER_INTEGRITYCHECKERS_H

#include "FileSystem.h"
#include "VDIFile.h"

void makeBitmaps(VDIFile* vdi);
void traverseAndMark(VDIFile* vdi, Directory* dir, char* name, uint32_t iNodeNumber);

#endif //EXT2_CHECKER_INTEGRITYCHECKERS_H
