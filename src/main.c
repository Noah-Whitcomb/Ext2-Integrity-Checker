//#include <stdio.h>
#include "Headers/load_file.h"
#include "Headers/Structs.h"
#include "Headers/Util.h"

int main() {
    Header *head = (Header*)malloc(sizeof (Header));
    openFile(head);
    printHeader(head);

    free(head);
    return 0;
}