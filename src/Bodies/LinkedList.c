#include "../Headers/LinkedList.h"


struct List* initializeList()
{
    struct List* list = (struct List*)malloc(sizeof(struct List));
    list->head = NULL;
    list->tail = NULL;
    list->add = add;
    return list;
}

struct node* createNode(uint32_t initialValue)
{
    struct node* head = malloc(sizeof(struct node));
    head->value = initialValue;
    head->nextNode = NULL;
    return head;
}

void add(struct List* list, uint32_t nextNode)
{
    if(list->tail != NULL)
    {
        list->tail->nextNode = createNode(nextNode);
        list->tail = list->tail->nextNode;
        return;
    }
    list->head = createNode(nextNode);
    list->tail = list->head;
}

void freeNodes(struct node* head)
{
    if (head->nextNode != NULL) {
        freeNodes(head->nextNode);
    }
    free(head);
}

void freeList(struct List* list)
{
    if(list->head != NULL) freeNodes(list->head);
    free(list);
}