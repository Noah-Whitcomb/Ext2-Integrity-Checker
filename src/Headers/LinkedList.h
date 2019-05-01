#ifndef EXT2_CHECKER_LINKEDLIST_H
#define EXT2_CHECKER_LINKEDLIST_H

#include <stdint.h>
#include <stdlib.h>


struct List
{
    struct node* head;
    struct node* tail;
    struct List* (*initializeList)();
    void (*add)(struct List* list, uint32_t nextNode);
};

struct node
{
    uint32_t value;
    struct node* nextNode;
    void (*add)(struct node* head, uint32_t nextNode);
    struct node* (*initialize)(uint32_t initialValue);
};

struct List* initializeList();
struct node* createNode(uint32_t initialValue);
void add(struct List* list, uint32_t nextNode);
void freeList(struct List* list);
void freeNodes(struct node* head);
//TODO add list free function

#endif //EXT2_CHECKER_LINKEDLIST_H
