#include "ADT.h"
void init(vlist *l)
{
    l->head = l->tail = NULL;
}
void appendRow(vlist *l, char *line, int lineLength)
{
    vnode *new_node = malloc(sizeof(vnode));
    new_node->next = NULL;
    new_node->row.size = lineLength;
    new_node->row.chars = (char *)malloc(lineLength + 1);
    memcpy(new_node->row.chars, line, lineLength);
    new_node->row.chars[lineLength] = '\0';
    if (l->head == NULL)
    {
        new_node->prev = NULL;
        l->head = l->tail = new_node;
        return;
    }
    vnode *p = l->head;
    while (p != l->tail)
        p = p->next;
    new_node->prev = p;
    p->next = new_node;
    l->tail = new_node;
    E.numOfRows++;
}