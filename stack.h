#ifndef STACK_H
#define STACK_H
#include "editor.h"
typedef struct event
{
    int ch;
    vnode *r;
    short type;
    int x_pos;
    int y_pos;
} event;
/*
type 1:insertion


*/

typedef struct snode
{
    event *data;
    struct snode *next;
} snode;
typedef snode *stack;
void push(stack *s, event *data);
void s_init(stack *s);
int s_isempty(stack s);
event *peek(stack s);
event *pop(stack *s);
#endif // !STACK_H
