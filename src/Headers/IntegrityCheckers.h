#ifndef EXT2_CHECKER_INTEGRITYCHECKERS_H
#define EXT2_CHECKER_INTEGRITYCHECKERS_H

#include "VDIFile.h"

#define BLOCK_DESCRIPTOR_SIZE 32

void readSuperBlock(VDIFile* vdi, uint8_t* superblock);
void readBlockDescTable(VDIFile* vdi, uint8_t* blockDescTable);
void fetchInode(VDIFile* vdi, uint8_t* iNodeBuffer, uint32_t iNodeNumber);

#endif //EXT2_CHECKER_INTEGRITYCHECKERS_H
