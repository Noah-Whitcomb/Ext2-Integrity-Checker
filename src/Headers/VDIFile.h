#ifndef EXT2_CHECKER_VDIFILE_H
#define EXT2_CHECKER_VDIFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

#include "../Headers/Structs.h"
#include "Util.h"

#define VDI_SET 0
#define VDI_CUR 1
#define VDI_END 2

VDIFile* vdiOpen(char* filename);
void vdiClose(VDIFile* vdi);
void vdiSeek(VDIFile* vdi, long long offset, int anchor);
void vdiRead(VDIFile* vdi, uint8_t* buffer, size_t nbytes);

void fetchBlock(VDIFile* vdi, uint8_t* buffer, uint32_t blockNumber);

#endif //EXT2_CHECKER_VDIFILE_H
