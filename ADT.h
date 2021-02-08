#ifndef __ADT_H__
#define __ADT_H__

#include "editor.h"
typedef struct vnode
{
    editorRow row;
    struct vnode *next;
    struct vnode *prev;
} vnode;
typedef struct vlist
{
    vnode *head;
    vnode *tail;
} vlist;
void init(vlist *l);
void appendRow(vlist *l, char *line, int lineLength);
#endif // !__ADT_H__