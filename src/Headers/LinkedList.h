#ifndef EXT2_CHECKER_LINKEDLIST_H
#define EXT2_CHECKER_LINKEDLIST_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

struct List
{
    struct node* head;
    struct node* tail;
    void (*add)(struct List* list, uint32_t nextNode);
    void (*printList)(struct List* list);
    uint32_t size;
};

struct node
{
    uint32_t value;
    struct node* nextNode;
};

struct List* initializeList();
struct node* createNode(uint32_t initialValue);
void add(struct List* list, uint32_t nextNode);
void freeList(struct List* list);
void printList(struct List* list);
void freeNodes(struct node* head);

#endif //EXT2_CHECKER_LINKEDLIST_H
