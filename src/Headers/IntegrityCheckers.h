#ifndef EXT2_CHECKER_INTEGRITYCHECKERS_H
#define EXT2_CHECKER_INTEGRITYCHECKERS_H

#include "FileSystem.h"
#include "VDIFile.h"

void makeBitmaps(VDIFile* vdi);
void traverseAndMark(VDIFile* vdi, Directory* dir);

#endif //EXT2_CHECKER_INTEGRITYCHECKERS_H
