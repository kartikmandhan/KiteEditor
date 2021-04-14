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
#define GAP_LEN 20
#define KEY_BS '\b'
#define KEY_DEL 127
#define KEY_CR '\r'
#define KEY_HT '\t'
#define KEY_NL '\n'
#define HIGHLIGHT_NUMBERS (1 << 0) //0000 0001
#define HIGHLIGHT_STRINGS (1 << 1) //0000 0010
#define syntaxDB_SIZE (sizeof(syntaxDB) / sizeof(syntaxDB[0]))
#define KITE_QUIT_TIMES 3
#define CHUNK_SIZE 100 //number of rows to be loaded
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
    char *gapBuffer;
    int gap_size;
    int gap_left;
    int gap_right;
    int gsize;
    unsigned char *hl;
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
/*** data ***/
typedef struct editorSyntax
{
    char *filetype;
    char **filematch;
    char *singlelineCommentStart;
    char **keywords;
    // flags is a bit field that will contain flags for whether to highlight numbers and whether to highlight strings for that filetype.
    int flags;
} editorSyntax;
typedef struct clipboard
{
    short fullLineCopy;
    short fullLineCut;
    int len;
    char *chars;
} clipboard;
void vlist_init(vlist *l);
void appendRow(vlist *l, char *line, int lineLength);
struct editorConfig
{
    int screenRows;
    int screenCols;
    int Cx; // x position of edit window(screen)
    int Cy; // y position of edit window(screen)
    char current_theme;
    int numOfRows; // number of Rows loaded in Data Structure
    int x_offset;  // displacement of scrollposition of x-axis
    int y_offset;  // displacement of scrollposition of y-axis
    vlist l;
    vnode *currentRow; //to keep track of current row while insertion,deletion,scrolling
    char fname[FILENAME_MAX];
    int status;
    char statusMessage[100]; //status message displaye in INFO WINDOW
    short dirtyFlag;         // to give a warning to user before closing modified file
    short newFileflag;       // to know whether to display saveFile dialogBox
    // newFileFlag=2 : file has no name yet, i.e. ./editor
    // newFileFlag=1 : file has a name but is new, ./editor file.txt ,, file.txt doesnt exist, empty buffer created but now on saving it will be saved as file.txt
    // newFileflag=0: file already exists
    short syntaxHighlightFlag; //to toggle ON and OFF syntax highlighting
    char *query;               // search string
    struct editorSyntax *syntax;
    fpos_t filePosition; // keeps track of how mush file is read in Data structure
    clipboard clip;
    // Cy+y_offset= position of cursor in the screen
} E;
enum editorHighlight
{
    HL_NORMAL = 0,
    HL_COMMENT,
    HL_KEYWORDS,
    HL_STRING,
    HL_NUMBER,
    HL_SEARCH
};
void setEditorStatus(int status, char *format, ...);
void editorMoveCursor(int key);
void editorRowUpdateHighlight(editorRow *row);
int editorSyntaxToColor(int hl);
void selectSyntaxHighlighting();
#endif // !EDITOR_H