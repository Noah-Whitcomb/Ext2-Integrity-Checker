#include "../Headers/LinkedList.h"


struct List* initializeList()
{
    struct List* list = (struct List*)malloc(sizeof(struct List));
    list->head = NULL;
    list->tail = NULL;
    list->add = add;
    list->printList = printList;
    list->size = 0;
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
        list->size++;
        return;
    }
    list->size++;
    list->head = createNode(nextNode);
    list->tail = list->head;
}

void printList(struct List* list)
{
    struct node* temp = list->head;
    while(temp != NULL)
    {
        printf("%d, ", temp->value);
        temp = temp->nextNode;
    }
    printf("\n");
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