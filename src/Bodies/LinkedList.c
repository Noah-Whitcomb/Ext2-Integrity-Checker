#include "../Headers/LinkedList.h"

struct node* createNode(uint32_t initialValue)
{
    struct node* head = malloc(sizeof(struct node));
    head->iNodeNumber = initialValue;
    head->nextInode = NULL;
    return head;
}

void add(struct node* head, uint32_t nextNode)
{
    if (head == NULL)
        head = createNode(nextNode);
    else if (head->nextInode != NULL)
        add(head->nextInode, nextNode);
    else
        head->nextInode = createNode(nextNode);
}