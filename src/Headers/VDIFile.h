//
// Created by crisc on 1/27/2019.
//

#ifndef EXT2_CHECKER_LOAD_FILE_H
#define EXT2_CHECKER_LOAD_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include "VDIFile.h"
#include "../Headers/Structs.h"
#include "Util.h"

VDIFile* vdiOpen(char* filename);
void vdiClose(VDIFile* vdi);

#endif //EXT2_CHECKER_LOAD_FILE_H
