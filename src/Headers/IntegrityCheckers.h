#ifndef EXT2_CHECKER_INTEGRITYCHECKERS_H
#define EXT2_CHECKER_INTEGRITYCHECKERS_H

#include "VDIFile.h"

void readSuperBlock(VDIFile* vdi, uint8_t* superblock);

#endif //EXT2_CHECKER_INTEGRITYCHECKERS_H
