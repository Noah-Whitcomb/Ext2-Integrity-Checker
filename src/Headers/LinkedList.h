#ifndef EXT2_CHECKER_LINKEDLIST_H
#define EXT2_CHECKER_LINKEDLIST_H

#include <stdint.h>
#include <stdlib.h>


struct node
{
    uint32_t iNodeNumber;
    struct node* nextInode;
    void (*add)(struct node* head, uint32_t nextNode);
    struct node* (*initialize)(uint32_t initialValue);
};

struct node* createNode(uint32_t initialValue);
void add(struct node* head, uint32_t nextNode);
void traverse(struct node* head);

#endif //EXT2_CHECKER_LINKEDLIST_H
