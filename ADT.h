#include <stdio.h>
typedef struct hnode
{
    char ch;
    struct hnode *prev;
    struct hnode *next;

} hnode;
typedef struct hlist
{
    hnode *head;
    hnode *tail;
} hlist;
typedef struct vnode
{
    hlist *line;
    int lineNumber;
    struct vnode *up;
    struct vnode *down;
} vnode;
typedef struct vlist
{
    vnode *head;
    vnode *tail;
} vlist;