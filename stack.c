#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
void s_init(stack *s)
{
    *s = NULL;
}
void push(stack *s, event *data)
{
    snode *new_node = (snode *)malloc(sizeof(snode));
    new_node->data = data;
    new_node->next = *s;
    *s = new_node;
}
int s_isempty(stack s)
{
    return s == NULL;
}
event *peek(stack s)
{
    if (s_isempty(s))
    {
        return NULL;
    }
    return s->data;
}
event *pop(stack *s)
{
    if (s_isempty(*s))
    {
        return NULL;
    }
    snode *p = *s;
    event *e = p->data;
    *s = p->next;
    free(p);
    return e;
}
