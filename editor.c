#include "editor.h"
#include "init.h"
#include "gui.h"
#include "syntaxHL.h"
void vlist_init(vlist *l)
{
    l->head = l->tail = NULL;
}
void appendRow(vlist *l, char *line, int lineLength)
{
    E.numOfRows++;
    vnode *new_node = malloc(sizeof(vnode));
    new_node->next = NULL;
    new_node->row.hl = NULL;
    new_node->row.size = lineLength;
    new_node->row.chars = (char *)malloc(lineLength + 1);
    memcpy(new_node->row.chars, line, lineLength);
    new_node->row.chars[lineLength] = '\0';
    // editorRowUpdateHighlight(&new_node->row);

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
}
void deleteRow(vnode *p)
{
    if (p == E.l.head)
        return;
    vnode *temp = p->prev;
    temp->next = p->next;
    if (p == E.l.tail)
        E.l.tail = temp;
    else
        p->next->prev = temp;
    free(p->row.chars);
    free(p);
    E.numOfRows--;
}
void insertRowAbove(vnode *p, char *line, int lineLength)
{
    E.numOfRows++;
    vnode *new_node = malloc(sizeof(vnode));
    vnode *beforeP = p->prev;
    new_node->next = p;
    new_node->prev = beforeP;
    p->prev = new_node;
    if (p == E.l.head)
        E.l.head = new_node;
    else
        beforeP->next = new_node;
    new_node->row.size = lineLength;
    new_node->row.chars = (char *)malloc(lineLength + 1);
    memcpy(new_node->row.chars, line, lineLength);
    new_node->row.chars[lineLength] = '\0';
    E.currentRow = new_node;
}
void insertRowBelow(vnode *p, char *line, int lineLength)
{
    insertRowAbove(p->next, line, lineLength);
    E.currentRow = E.currentRow->prev;
}
void editorRowAppendString(editorRow *row, char *s, int len)
{
    row->chars = realloc(row->chars, row->size + len + 1);
    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    E.dirtyFlag = 1;
}
void editor_init()
{
    E.Cx = DEFPOS_X;
    E.Cy = DEFPOS_Y;
    E.numOfRows = 0;
    E.x_offset = 0;
    E.y_offset = 0;
    E.dirtyFlag = 0;
    E.syntaxHighlightFlag = 0;
    vlist_init(&E.l);
    noecho();
    // LINES, COLS gives current rows and cols of the terminal and are defined in Ncurses library
    if (LINES < 30 || COLS < 98)
    {
        endwin();
        fprintf(stderr, "Terminal window is too small.\n Min: 30x98, your: %dx%d\n", LINES, COLS);
        exit(EXIT_FAILURE);
    }
    init_colors();
    init_windows();
    refresh();
    init_gui();
    // wrefresh(win[INFO_WINDOW]);
    // init
    E.query = NULL;
    E.syntax = NULL;
    raw();
    wmove(win[EDIT_WINDOW], E.Cy, E.Cx);
}
void destroyDataStructure()
{
    vnode *p = E.l.head, *temp;
    while (p)
    {
        temp = p;
        free(temp->row.chars);
        if (temp->row.hl)
            free(temp->row.hl);
        free(temp);
        p = p->next;
    }
    E.Cx = DEFPOS_X;
    E.Cy = DEFPOS_Y;
    E.numOfRows = 0;
    E.x_offset = 0;
    E.y_offset = 0;
    E.dirtyFlag = 0;
    vlist_init(&E.l);
}
void createBlankFile()
{
    appendRow(&E.l, "", 0);
    E.currentRow = E.l.head;
}
void allocateMoreRows(FILE *fp, int totalCount)
{
    if (fp == NULL)
    {
        fp = fopen(E.fname, "r");
    }
    char *line = NULL;
    int count = 0;
    size_t lineCapacity = 0;
    // return type of getline
    ssize_t lineLength;

    fsetpos(fp, &E.filePosition);
    // printf("filepositon:%lld\n", t);
    // if (lineLength != -1)
    // I HAVE USED GETLINE INSTEAD OF FGETS, SINCE WE DONT KNOW THE SIZE OF LINE BEFOREHAND
    while (count < totalCount && (lineLength = getline(&line, &lineCapacity, fp)) != -1)
    {
        while (lineLength > 0 && (line[lineLength - 1] == '\n' ||
                                  line[lineLength - 1] == '\r'))
            lineLength--;
        // Convert tabs to spaces, since
        //\t moves cursor by 8 spaces forward
        for (int i = 0; i < lineLength; i++)
        {
            if (line[i] == '\t')
            {
                line = realloc(line, sizeof(char) *
                                         (lineLength + KITE_TABSIZE + 1));
                memmove(&line[i + KITE_TABSIZE], &line[i + 1],
                        sizeof(char) * (lineLength - i + 1));

                for (int j = 0; j < KITE_TABSIZE; j++)
                    line[i + j] = ' ';

                i += KITE_TABSIZE - 1;
                lineLength += KITE_TABSIZE - 1;
            }
        }
        appendRow(&E.l, line, lineLength);
        count++;
    }
    fgetpos(fp, &E.filePosition);
    // printf("filepositon:%lld\n", t);
    free(line);
    fclose(fp);
}
void openFile()
{
    FILE *fp = fopen(E.fname, "r");
    // E.fname = (char *)malloc(strlen(filename) + 1);
    if (!fp)
    {
        setEditorStatus(1, "Unable to open the file, created a blank file instead");
        createBlankFile();
        // selectSyntaxHighlighting();
        return;
    }
    E.newFileflag = 0;
    allocateMoreRows(fp, CHUNK_SIZE);
    E.currentRow = E.l.head;
    // selectSyntaxHighlighting();
    setEditorStatus(0, "File opened Successfully");
}
char *dataStructureToString(int *totalLength)
{
    int len = 0;
    vnode *p = E.l.head;

    for (int i = 0; i < E.numOfRows; i++)
    {
        len += p->row.size + 1;

        p = p->next;
    }
    *totalLength = len;
    char *result = (char *)malloc(sizeof(char) * len);
    char *pointerToResult = result;
    p = E.l.head;
    for (int i = 0; i < E.numOfRows; i++)
    {
        memcpy(pointerToResult, p->row.chars, p->row.size);
        pointerToResult += p->row.size;
        // replacing '\0' with '\n' character
        *pointerToResult = '\n';
        pointerToResult++;
        p = p->next;
    }
    return result;
}
void saveFile()
{
    int buflen = 0;
    char *buf = dataStructureToString(&buflen);
    if (E.newFileflag)
    {
        save_file_popup();
        E.newFileflag = 0;
    }
    FILE *fp = fopen(E.fname, "w");
    if (fp == NULL)
    {
        setEditorStatus(1, "Unable to open the file");
        return;
    }
    // selectSyntaxHighlighting();
    // mvwprintw(win[MENU_WINDOW], 1, 25, "%s %d", buf,buflen);
    wrefresh(win[MENU_WINDOW]);
    int wsize = fwrite(buf, sizeof(char), buflen, fp);
    if (wsize != buflen)
    {
        free(buf);
        printf("error\n");
        fclose(fp);
    }
    free(buf);
    fclose(fp);
    E.dirtyFlag = 0;
    setEditorStatus(0, "File Saved Successfully");
}
void saveFileReadInChunk()
{
    // A short Optimization
    if (E.dirtyFlag == 0)
    {
        setEditorStatus(0, "File Saved Successfully");
        return;
    }
    FILE *fPtr;
    FILE *fTemp;
    char *buffer = NULL;
    int buflen = 0;
    if (E.newFileflag)
    {
        save_file_popup();
        E.newFileflag = 0;
    }
    //  Open all required files
    fPtr = fopen(E.fname, "r");
    // creating an hidden file ,since it starts with a '.'
    fTemp = fopen(".replace.tmp", "w");
    if (fPtr == NULL || fTemp == NULL)
    {
        // Unable to open file hence exit
        setEditorStatus(1, "Unable to open the file");
        printf("\nUnable to open file.\n");
        printf("Please check whether file exists and you have read/write privilege.\n");
        return;
    }
    char *buf = dataStructureToString(&buflen);
    mvwprintw(win[MENU_WINDOW], 1, 25, "%d", buflen);
    wrefresh(win[MENU_WINDOW]);
    int wsize = fwrite(buf, sizeof(char), buflen, fTemp);
    if (wsize != buflen)
    {
        free(buf);
        printf("error\n");
        fclose(fTemp);
    }
    free(buf);
    //     Read line from source file and write to destination
    // file after writing the datastructure in the file
    size_t lineCapacity2 = 0;
    fsetpos(fPtr, &E.filePosition);
    while ((getline(&buffer, &lineCapacity2, fPtr)) != -1)
    {
        fputs(buffer, fTemp);
    }
    free(buffer);
    // Close all files to release resource
    fclose(fPtr);
    fclose(fTemp);
    //  Delete original source file
    remove(E.fname);
    //  Rename temporary file as original file
    rename(".replace.tmp", E.fname);
    E.dirtyFlag = 0;
    setEditorStatus(0, "File Saved Successfully");
}
void editorRowInsertChar(editorRow *row, int at, int ch)
{
    if (at < 0 || at > row->size)
        at = row->size;
    row->chars = (char *)realloc(row->chars, row->size + 2);
    // +2 since we need space for \0 byte too
    memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
    row->size++;
    row->chars[at] = ch;
    if (E.syntaxHighlightFlag)
        editorRowUpdateHighlight(row);
}
void editorInsertChar(int c)
{
    if (iscntrl(c))
    {
        // flushinp says dont print that character
        flushinp();
        return;
    }
    if (E.Cy + E.y_offset == E.numOfRows)
        appendRow(&E.l, "", 0);
    editorRowInsertChar(&E.currentRow->row, E.Cx + E.x_offset - DEFPOS_X, c);
    editorMoveCursor(KEY_RIGHT);
    E.dirtyFlag = 1;
}

void print_text()
{
    // initialising cursor to start of the editor
    // E.Cx = DEFPOS_X;
    // E.Cy = DEFPOS_Y;
    // wmove(win[EDIT_WINDOW], E.Cy, E.Cx);
    // these x and y are used for the position where text will be printed
    int x = 0, y = 0;

    int file_rowOffset = E.y_offset;
    vnode *p = E.l.head;
    while (y < E.numOfRows && y < LIMIT_Y)
    {
        wmove(win[EDIT_WINDOW], y + 1, 1);
        x = E.x_offset;

        while (file_rowOffset-- > 0)
        {
            p = p->next;
        }
        unsigned char *hl = p->row.hl;
        // default color indicated by -1
        // this logic is implemented in order to prevent so many wattron and wattroff
        while (x < p->row.size && x < LIMIT_X + E.x_offset)
        {
            // if (E.l.head->row.size > LIMIT_X)
            if (E.syntaxHighlightFlag && hl)
            {
                int color = editorSyntaxToColor(hl[x]);
                if (hl[x] == HL_NORMAL)
                {
                    waddch(win[EDIT_WINDOW], p->row.chars[x]);
                }
                else
                {
                    wattron(win[EDIT_WINDOW], COLOR_PAIR(color));
                    waddch(win[EDIT_WINDOW], p->row.chars[x]);
                    wattroff(win[EDIT_WINDOW], COLOR_PAIR(color));
                }
            }
            else
            {
                waddch(win[EDIT_WINDOW], p->row.chars[x]);
            }
            x++;
        }
        p = p->next;
        y++;
    }
}
void editorRefresh()
{
    // wmove(win[EDIT_WINDOW], E.Cy, E.Cx);
    // int rowsToPrint = E.numOfRows;

    // wprintw(win[EDIT_WINDOW], "key %ld", rowsToPrint);
    // static int flag = 0;
    // if (!flag)
    // {

    print_text();
    // flag = 1;
    // }
    wmove(win[EDIT_WINDOW], E.Cy, E.Cx);
    werase(win[INFO_WINDOW]);
    draw_window(INFO_WINDOW);
    wrefresh(win[INFO_WINDOW]);
    if (E.numOfRows - (E.Cy + E.y_offset) < 10)
    {
        allocateMoreRows(NULL, 10);
        setEditorStatus(0, "here %d %d", E.numOfRows, E.numOfRows - (E.Cy + E.y_offset));
    }
}
void editorMoveCursor(int key)
{
    // wmove(win[EDIT_WINDOW], E.Cy, E.Cx);
    // static int flag = 0;
    // static vnode *p = NULL;
    // if (!flag)
    // {
    //     p = E.l.head;
    //     flag = 1;
    // }
    // wprintw(win[EDIT_WINDOW], "%d %u", p->row.size, p);
    switch (key)
    {
    case KEY_LEFT:
        if (E.Cx > DEFPOS_X)
            E.Cx--;

        else if (E.x_offset > 0)
        {
            E.x_offset--;
            // these two functions clear the window and redraw the border which is necessary to prevent overwriting of text
            werase(win[EDIT_WINDOW]);
            draw_window(EDIT_WINDOW);
        }
        else if (E.Cy + E.y_offset > DEFPOS_Y)
        {
            // move on end of other line on press of <-
            E.currentRow = E.currentRow->prev;
            if (E.y_offset > 0 && E.Cy == DEFPOS_Y)
            {
                E.y_offset--;
            }
            else
                E.Cy--;
            if (E.currentRow->row.size + 1 - LIMIT_X > 0)
                E.x_offset = E.currentRow->row.size + 1 - LIMIT_X;
            E.Cx = LIMIT_X;
            // these two functions clear the window and redraw the border which is necessary to prevent overwriting of text
            werase(win[EDIT_WINDOW]);
            draw_window(EDIT_WINDOW);
        }
        break;
    case KEY_RIGHT:
        if (E.Cx <= E.currentRow->row.size && E.Cx < LIMIT_X)
            E.Cx++;
        else if (E.Cx + E.x_offset <= E.currentRow->row.size)
        {
            E.x_offset++;
            // these two functions clear the window and redraw the border which is necessary to prevent overwriting of text
            werase(win[EDIT_WINDOW]);
            draw_window(EDIT_WINDOW);
        }
        else if (E.Cy + E.y_offset < E.numOfRows)
        {
            // move on end of other line on press of ->
            E.currentRow = E.currentRow->next;
            if (E.Cy == LIMIT_Y)
            {
                E.y_offset++;
                // these two functions clear the window and redraw the border which is necessary to prevent overwriting of text
                werase(win[EDIT_WINDOW]);
                draw_window(EDIT_WINDOW);
            }
            else
                E.Cy++;

            E.Cx = DEFPOS_X;
            E.x_offset = 0;
        }
        break;
    case KEY_UP:

        if (E.Cy > DEFPOS_Y)
        {
            E.currentRow = E.currentRow->prev;

            E.Cy--;
        }
        else if (E.y_offset > 0)
        {
            E.currentRow = E.currentRow->prev;

            E.y_offset--;
            // these two functions clear the window and redraw the border which is necessary to prevent overwriting of text
            werase(win[EDIT_WINDOW]);
            draw_window(EDIT_WINDOW);
        }
        break;
    case KEY_DOWN:

        if (E.Cy < LIMIT_Y && E.Cy < E.numOfRows)
        {
            E.currentRow = E.currentRow->next;

            E.Cy++;
        }
        else if (E.Cy + E.y_offset < E.numOfRows)
        {
            E.currentRow = E.currentRow->next;

            E.y_offset++;
            // these two functions clear the window and redraw the border which is necessary to prevent overwriting of text
            werase(win[EDIT_WINDOW]);
            draw_window(EDIT_WINDOW);
        }
        break;
    }
    //makes cursor move up and down only upto bounds of line,when up or down arrow is pressed
    if (E.Cx > E.currentRow->row.size)
    {
        E.Cx = E.currentRow->row.size + 1;
        E.x_offset = 0;
    }
}
void editorRowDelChar(editorRow *row, int at)
{
    if (at < 0 || at >= row->size)
        return;
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
    row->size--;
    E.dirtyFlag = 1;
}
void editorDelChar()
{
    werase(win[EDIT_WINDOW]);
    draw_window(EDIT_WINDOW);
    if (E.Cx + E.x_offset == DEFPOS_X)
    {
        if (E.currentRow->row.size != 0)
        {
            editorRowAppendString(&E.currentRow->prev->row, E.currentRow->row.chars, E.currentRow->row.size);
        }
        deleteRow(E.currentRow);
    }
    else if (E.Cx + E.x_offset > DEFPOS_X)
    {
        editorRowDelChar(&E.currentRow->row, E.Cx + E.x_offset - DEFPOS_X - 1);
    }
    editorMoveCursor(KEY_LEFT);
    if (E.syntaxHighlightFlag)
        editorRowUpdateHighlight(&E.currentRow->row);
}
void editorInsertNewline()
{
    if (E.Cx + E.x_offset == DEFPOS_X)
    {
        insertRowAbove(E.currentRow, "", 0);
    }
    else if (E.Cx + E.x_offset - DEFPOS_X == E.currentRow->row.size)
    {
        insertRowBelow(E.currentRow, "", 0);
        // since we are passing a 0 length row we need to adjust the curentRow pointer
    }
    else
    {
        int previousSize = E.currentRow->row.size;
        int newSize = E.Cx + E.x_offset - DEFPOS_X;
        insertRowBelow(E.currentRow, &E.currentRow->row.chars[newSize], previousSize - newSize);
        E.currentRow->row.size = newSize;
        E.currentRow->row.chars[E.currentRow->row.size] = '\0';
        if (E.syntaxHighlightFlag)
        {
            editorRowUpdateHighlight(&E.currentRow->row);
            editorRowUpdateHighlight(&E.currentRow->next->row);
        }
    }
    editorMoveCursor(KEY_DOWN);
    E.Cx = DEFPOS_X;
    E.x_offset = 0;
    E.dirtyFlag = 1;
    werase(win[EDIT_WINDOW]);
    draw_window(EDIT_WINDOW);
}
void search(int reset)
{
    WINDOW *findwin = NULL;
    static vnode *p = NULL;
    static int i = 0, ismatchFound = 0;
    // searched highlighting part 1
    static vnode *saved_hl_vnode;
    static char *saved_hl = NULL;
    if (saved_hl)
    {
        memcpy(saved_hl_vnode->row.hl, saved_hl, saved_hl_vnode->row.size);
        free(saved_hl);
        saved_hl = NULL;
    }
    // searched highlighting part 1 ends
    if (reset == 1)
    {
        p = E.l.head;
        if (E.query != NULL)
        {
            free(E.query);
            E.query = NULL;
        }
        i = 0;
        E.query = (char *)malloc(sizeof(char) * 100);
        E.query = search_file_popup(findwin);
        ismatchFound = 0;
        // empty searchbox
        if (E.query == NULL)
            return;
    }
    int x_off, y_off, queryLen;
    queryLen = strlen(E.query);
    for (; i < E.numOfRows; i++, p = p->next)
    {
        // case insensitive search
        char *match = strcasestr(p->row.chars, E.query);
        if (match)
        {
            setEditorStatus(0, "%d %s", i, E.query);
            y_off = i + DEFPOS_Y;
            x_off = match - p->row.chars + DEFPOS_X;
            // searched highlighting part 2
            saved_hl_vnode = p;
            saved_hl = (char *)malloc(sizeof(char) * p->row.size);
            memcpy(saved_hl, p->row.hl, p->row.size);
            memset(&p->row.hl[match - p->row.chars], HL_SEARCH, strlen(E.query));
            // searched highlighting part 2 ends
            if (y_off < LIMIT_Y)
            {
                E.Cy = y_off;
                E.y_offset = 0;
            }
            else
            {
                E.Cy = LIMIT_Y;
                E.y_offset = y_off - LIMIT_Y;
            }
            if (x_off < LIMIT_X)
            {
                E.Cx = x_off;
                E.x_offset = 0;
            }
            else
            {
                E.Cx = LIMIT_X - queryLen;
                E.x_offset = queryLen + x_off - LIMIT_X;
            }
            werase(win[EDIT_WINDOW]);
            draw_window(EDIT_WINDOW);
            E.currentRow = p;
            p = p->next;
            i++;
            ismatchFound = 1;
            break;
        }
    }
    if (i == E.numOfRows)
        setEditorStatus(0, "Search Completed");
    if (!ismatchFound)
        setEditorStatus(0, "Not Found");
}
void read_key()
{
    int c = wgetch(win[EDIT_WINDOW]);
    // wmove(win[EDIT_WINDOW], 10, 0);
    static int quit_times = KITE_QUIT_TIMES;

    // printw("%c", c);
    switch (c)
    {
    case CTRL_KEY('q'):
        if (E.dirtyFlag && quit_times > 0)
        {
            setEditorStatus(0, "WARNING!!! File has unsaved changes.Press Ctrl-Q %d more times to quit.", quit_times);
            flash();
            quit_times--;
            return;
        }
        endwin();
        exit(EXIT_SUCCESS);
        break;
    case CTRL_KEY('f'):
        search(1);
        break;
    case CTRL_KEY('g'):
        if (E.query != NULL)
            search(0);
        else
            setEditorStatus(0, "Use ctrl+f to find, before using find next");
        break;
    case KEY_F(3):
        if (E.syntaxHighlightFlag)
            E.syntaxHighlightFlag = 0;
        else
        {
            E.syntaxHighlightFlag = 1;
            selectSyntaxHighlighting();
        }
    case KEY_DOWN:
    case KEY_UP:
    case KEY_LEFT:
    case KEY_RIGHT:
        editorMoveCursor(c);
        break;
    case KEY_HOME:
        for (int i = 0; i < 1 * (E.Cy + E.y_offset); i++)
        {
            editorMoveCursor(KEY_UP);
            editorMoveCursor(KEY_UP);
        }

        break;
    case KEY_END:
        // wprintw(win[EDIT_WINDOW], "key end");
        for (int i = 0; i < 1 * (E.Cy + E.y_offset); i++)
        {
            editorMoveCursor(KEY_DOWN);
            editorMoveCursor(KEY_DOWN);
        }
        break;
    case KEY_ENTER:
    case KEY_NL:
        editorInsertNewline();
        break;
    case CTRL_KEY('s'):
        // saveFile();
        saveFileReadInChunk();
        break;
    case KEY_DC:
    case KEY_BS:
    case KEY_DEL:
    case KEY_BACKSPACE:
        editorDelChar();
        break;
    case KEY_F(1):
        if (E.l.head != NULL)
            destroyDataStructure();
        open_file_popup();
        openFile();
        break;
    default:
        if (!iscntrl(c) && c < 127 && c >= 32)
            editorInsertChar(c);
        else
            beep();
        break;
    }
    quit_times = KITE_QUIT_TIMES;
}
void setEditorStatus(int status, char *format, ...)
{
    E.status = status;
    va_list args;
    va_start(args, format);
    // This function will take a format string and a variable number of arguments, like the printf().
    vsprintf(E.statusMessage, format, args);
    werase(win[INFO_WINDOW]);
    draw_window(INFO_WINDOW);
    va_end(args);
}
int editorSyntaxToColor(int hl)
{
    switch (hl)
    {
    case HL_NUMBER:
        return 5;
    case HL_SEARCH:
        return 7;
    case HL_STRING:
        return 6;
    case HL_COMMENT:
        return 8;
    case HL_KEYWORDS:
        return 7;
    default:
        return 2;
    }
}
int is_separator(int c)
{
    //The strchr() function locates the first occurrence of c (converted to a char) in the string pointed to by s
    // so we created a string of all the seperators and if c belongs to any of them, it will return a non NULL value.
    return isspace(c) || strchr(",.()+-/*=~%<>[];", c) != NULL;
}

void editorRowUpdateHighlight(editorRow *row)
{
    row->hl = realloc(row->hl, row->size);
    memset(row->hl, HL_NORMAL, row->size);
    if (E.syntax == NULL)
        return;
    int i = 0;
    int isprevCharSeperator = 1;
    int insideString = 0;
    char *slcs = E.syntax->singlelineCommentStart;
    int slcLen = slcs ? strlen(slcs) : 0;
    char **keywords = E.syntax->keywords;
    // we set it to 1 because the starting of line is also considered as seperator
    while (i < row->size)
    {
        char c = row->chars[i];
        unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : HL_NORMAL;
        if (slcLen && !insideString)
        {
            if (!strncmp(&row->chars[i], slcs, slcLen))
            {
                memset(&row->hl[i], HL_COMMENT, row->size - i);
                break;
            }
        }

        if (E.syntax->flags & HIGHLIGHT_STRINGS)
        {
            if (insideString)
            {
                row->hl[i] = HL_STRING;
                // If the sequence \' or \" occurs in a string, then the escaped quote doesn’t close the string,and is the part of the string
                if (c == '\\' && i + 1 < row->size)
                {
                    row->hl[i + 1] = HL_STRING;
                    i += 2;
                    continue;
                }
                // when you get closing `"` or `'`
                if (c == insideString)
                    insideString = 0;
                i++;
                // closing quote is a seperator
                isprevCharSeperator = 1;
                continue;
            }
            else
            {
                if (c == '"' || c == '\'')
                {
                    insideString = c;
                    row->hl[i] = HL_STRING;
                    i++;
                    continue;
                }
            }
        }

        if (E.syntax->flags & HIGHLIGHT_NUMBERS)
        {
            //  we now require the previous character to either be a separator, or to also be highlighted with HL_NUMBER.
            if ((isdigit(c) && (isprevCharSeperator || prev_hl == HL_NUMBER)) || (c == '.' && prev_hl == HL_NUMBER))
            {
                row->hl[i] = HL_NUMBER;
                i++;
                isprevCharSeperator = 0;
                continue;
            }
        }
        if (isprevCharSeperator)
        {
            int j = 0;
            while (keywords[j])
            {
                int klen = strlen(keywords[j]);
                if (!strncmp(&row->chars[i], keywords[j], klen) && is_separator(row->chars[i + klen]))
                {
                    memset(&row->hl[i], HL_KEYWORDS, klen);
                    i += klen;
                    break;
                }
                j++;
            }
            if (keywords[j] != NULL)
            {
                isprevCharSeperator = 0;
                continue;
            }
        }
        isprevCharSeperator = is_separator(c);
        i++;
    }
}
// this null will help us stop iteration in selectSyntaxHighlighting

void selectSyntaxHighlighting()
{
    E.syntax = NULL;
    if (E.newFileflag)
        return;
    // find the last occurence of '.', extension points to it,if found
    // or else will be NULL
    char *extension = strrchr(E.fname, '.');
    for (int i = 0; i < syntaxDB_SIZE; i++)
    {
        editorSyntax *s = &syntaxDB[i];
        int j = 0;
        while (s->filematch[j])
        {
            int isExtension = (s->filematch[j][0] == '.');
            if (isExtension && extension && !strcmp(extension, s->filematch[j]))
            {
                E.syntax = s;
                // rehighlighting the text which is needed when:
                // user creates an empty file and then save it with a .c extension
                // setEditorStatus(0, "here");
                vnode *p = E.l.head;
                if (E.syntaxHighlightFlag)
                    for (int k = 0; p != NULL; p = p->next, k++)
                    {
                        editorRowUpdateHighlight(&p->row);
                    }
                return;
            }
            j++;
        }
    }
}
int main(int argc, char *argv[])
{
    initscr();
    editor_init();
    if (argc == 2)
    {
        strcpy(E.fname, argv[1]);
        openFile();
    }
    else if (argc == 1)
    {
        E.newFileflag = 1;
        createBlankFile();
    }
    else
    {
        exit(0);
    }
    // mvprintw(E.screenCols / 2, E.screenRows / 2 - 10, "welcome to my editor");
    while (1)
    {
        editorRefresh();

        read_key();

        // refresh();
        // move(10, 5 + x);
        // x += 5;
        // printw("left");
    }
    // if (!has_colors())
    // {
    //     printw("Colors are not supported for you");
    //     getch();
    //     return -1;
    // }
    // init_colors();
    // // 1 becomes identity for the color pair
    // //              text color, bg color
    // // init_pair(1, COLOR_GREEN, COLOR_CYAN);
    // // init_pair(2, COLOR_MAGENTA, COLOR_YELLOW);
    // WINDOW *win1 = newwin(LINES - 8, COLS, 3, 0);
    // box(win1, 0, 0);
    // wbkgd(win1, EDIT_CLR);

    // refresh();
    // wattron(win1, COLOR_PAIR(2));
    // wprintw(win1, "diffrent color");
    // wattroff(win1, COLOR_PAIR(2));
    // wrefresh(win1);
    getch();
    getch();
    endwin();
    exit(EXIT_SUCCESS);
}
