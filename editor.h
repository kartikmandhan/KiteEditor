#ifndef EDITOR_H
#define EDITOR_H
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>
#include <curses.h>
// for variable arguements having functions
#include <stdarg.h>
#define CTRL_KEY(k) ((k)&0x1f)
#define KITE_QUIT_TIMES 3
/*
 * This implementation has 3 windows: menu, edit, info.
 *
 * +------------------+
 * | (1) Menu window  |
 * +------------------+
 * |                  |
 * | (2) Edit window  |
 * |                  |
 * +------------------+
 * | (3) Info window  |
 * +------------------+
 *
 * Menu window contains the set of F(k) keys that you can use and
 * get the editor functionality. For example, if you press the 'Help'
 * key, you`ll get the full list of control and function keys that
 * you can use in the program. In the edit field you can read the
 * text and change it if you want. Info window displays the current
 * state of the file (e.g.: name, mode, cursor offset, errors).
 */

#define WINDOWS_COUNT 3

/*
 * Each window framed, then default relative position of text will be
 * (y:1,x:1). X-axis limit: number of columns - 2 (left border and the
 * right one). Y-axis limit: number of lines - 10 (3 of menu window,
 * 5 of info, plus top & bottom borders).
 */

#define DEFPOS_X 1
#define DEFPOS_Y 1
#define LIMIT_X (COLS - 2)
#define LIMIT_Y (LINES - 10)
#define KITE_TABSIZE 4
#define KEY_BS '\b'
#define KEY_DEL 127
#define KEY_CR '\r'
#define KEY_HT '\t'
#define KEY_NL '\n'
/* Editor windows */
WINDOW *win[WINDOWS_COUNT];

/* Window types */
enum win_type
{
    MENU_WINDOW = 0,
    EDIT_WINDOW,
    INFO_WINDOW
};
typedef struct editorRow
{
    int size;
    char *chars;
} editorRow;
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
void vlist_init(vlist *l);
void appendRow(vlist *l, char *line, int lineLength);
struct editorConfig
{
    int Cx; // x position of edit window(screen)
    int Cy; // y position of edit window(screen)
    char current_theme;
    int numOfRows;
    int x_offset;
    int y_offset;
    vlist l;
    vnode *currentRow;
    char fname[FILENAME_MAX];
    int status;
    char statusMessage[100];
    int dirtyFlag;
    int newFileflag;
    // Cy+y_offset= position of cursor in the screen
} E;
#endif // !EDITOR_H
void setEditorStatus(int status, char *format, ...);
void editorMoveCursor(int key);