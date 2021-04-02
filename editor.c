#include "editor.h"
#include "init.h"
#include "gui.h"
void vlist_init(vlist *l)
{
    l->head = l->tail = NULL;
}
void appendRow(vlist *l, char *line, int lineLength)
{
    E.numOfRows++;
    vnode *new_node = malloc(sizeof(vnode));
    new_node->next = NULL;
    new_node->row.size = lineLength;
    new_node->row.chars = (char *)malloc(lineLength + 1);
    new_node->row.gsize = lineLength;
    new_node->row.gap_size = 0;
    new_node->row.gap_left = -1;
    new_node->row.gap_right = -1;
    new_node->row.gapBuffer = NULL;
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
}
void editor_init()
{
    E.Cx = DEFPOS_X;
    E.Cy = DEFPOS_Y;
    E.numOfRows = 0;
    E.x_offset = 0;
    E.y_offset = 0;
    E.dirtyFlag = 0;
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
    raw();
    wmove(win[EDIT_WINDOW], E.Cy, E.Cx);
}
void display()
{
    vnode *p = E.l.head;
    do
    {
        if (p->row.gapBuffer != NULL)
        {

            for (int i = 0; i < p->row.gsize; i++)
            {
                printf("%c", p->row.gapBuffer[i]);
            }
            printf("\ngap->size=%d gap->left=%d gap->right=%d gsize=%d row->size=%d", p->row.gap_size, p->row.gap_left, p->row.gap_right, p->row.gsize, p->row.size);
        }
        else
        {
            for (int i = 0; i < p->row.size; i++)
            {
                printf("%c", p->row.chars[i]);
            }
        }
        printf("\n");
        p = p->next;
    } while (p != E.l.tail);

    printf("\n");
}

// Function that is used to move the gap
// left in the array
void left(int position, editorRow *row)
{
    // Move the gap left character by character
    // and the buffers
    while (position < row->gap_left)
    {
        row->gap_left--;
        row->gap_right--;
        row->gapBuffer[row->gap_right + 1] = row->gapBuffer[row->gap_left];
        row->gapBuffer[row->gap_left] = '\0';
    }
}

// Function that is used to move the gap
// right in the array
void right(int position, editorRow *row)
{
    // Move the gap right character by character
    // and the row->gapBuffers
    while (position > row->gap_left)
    {
        row->gap_left++;
        row->gap_right++;
        row->gapBuffer[row->gap_left - 1] = row->gapBuffer[row->gap_right];
        row->gapBuffer[row->gap_right] = '\0';
    }
}

// Function to control the movement of gap
// by checking its position to the point of
// insertion
void move_cursor_gapBuffer(editorRow *row, int position)
{
    if (position < row->gap_left)
    {
        // printf("left\n");
        left(position, row);
    }
    else
    {
        // printf("right\n");

        right(position, row);
    }
}
void grow(editorRow *row, int position)
{
    row->gapBuffer = (char *)realloc(row->gapBuffer, row->size + GAP_LEN + 1);
    row->gsize += GAP_LEN;
    char a[row->gsize];
    // Copy characters of buffer to a[]
    // after position
    for (int i = position; i < row->gsize; i++)
    {
        a[i - position] = row->gapBuffer[i];
    }
    // printf("pos:%d %d\n", strlen(a), row->gsize - position - GAP_LEN);

    // Insert a gap of k from index position
    // gap is being represented by '-'
    for (int i = 0; i < GAP_LEN; i++)
    {
        row->gapBuffer[i + position] = '\0';
    }
    // printf("grow%s\n", row->gapBuffer);
    // Reinsert the remaining array
    for (int i = 0; i < row->gsize - position - GAP_LEN; i++)
    {
        row->gapBuffer[position + GAP_LEN + i] = a[i];
    }

    row->gap_size += GAP_LEN;
    row->gap_right += GAP_LEN;
    // printf("grow:gap->size=%d gap->left=%d gap->right=%d gsize=%d row->size=%d\n", row->gap_size, row->gap_left, row->gap_right, row->gsize, row->size);
    // display();
}
void editorRowInsertChar(editorRow *row, int at, int ch)
{
    if (at < 0 || at > row->size)
        at = row->size;
    if (row->gapBuffer == NULL)
    {
        row->gapBuffer = (char *)malloc(sizeof(char) * (row->size + GAP_LEN + 1));
        strncpy(row->gapBuffer, row->chars, row->size);
        row->gsize = row->size + GAP_LEN;
        row->gap_left = at;
        row->gap_right = at + GAP_LEN - 1;
        row->gap_size += GAP_LEN;
        for (int i = 0; i < GAP_LEN; i++)
        {
            row->gapBuffer[at + i] = '\0';
        }
        strncpy(&row->gapBuffer[at + GAP_LEN], &row->chars[at], row->size - at + 1);
        // printf("herre:%s\n", row->gapBuffer);
    }
    if (at != row->gap_left)
    {
        move_cursor_gapBuffer(row, at);
    }
    if (row->gap_left == row->gap_right)
    {
        grow(row, at);
    }
    row->gapBuffer[at] = ch;
    row->size++;
    row->gap_size--;
    row->gap_left++;
    // printf("edit:gap->size=%d gap->left=%d gap->right=%d gsize=%d row->size=%d\n", row->gap_size, row->gap_left, row->gap_right, row->gsize, row->size);
}
void deletion(editorRow *row, int position)
{
    // If the point is not the gap check
    // and move the cursor to that point
    if (row->gapBuffer == NULL)
    {
        row->gapBuffer = (char *)malloc(sizeof(char) * (row->size + GAP_LEN + 1));
        strncpy(row->gapBuffer, row->chars, row->size);
        row->gsize = row->size + GAP_LEN;
        row->gap_left = position;
        row->gap_right = position + GAP_LEN - 1;
        row->gap_size += GAP_LEN;
        for (int i = 0; i < GAP_LEN; i++)
        {
            row->gapBuffer[position + i] = '\0';
        }
        strncpy(&row->gapBuffer[position + GAP_LEN], &row->chars[position], row->size - position + 1);
        // printf("herre:%s\n", row->gapBuffer);
    }
    if (position + 1 != row->gap_left)
    {
        move_cursor_gapBuffer(row, position + 1);
    }

    // Reduce the gap_left
    row->gap_left--;
    row->size--;
    row->gap_size++;
    row->gapBuffer[row->gap_left] = '\0';
}
void openFile(char *filename)
{
    FILE *fp = fopen(filename, "r");
    strcpy(E.fname, filename);
    if (!fp)
    {
        setEditorStatus(1, "Unable to open the file");
        return;
    }
    char *line = NULL;
    // parameter type of lineCapacity
    size_t lineCapacity = 0;
    // return type of getline
    ssize_t lineLength;

    // if (lineLength != -1)
    while ((lineLength = getline(&line, &lineCapacity, fp)) != -1)
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
        // E.row.size = lineLength;
        // E.row.chars = (char *)malloc(lineLength + 1);
        // memcpy(E.row.chars, line, lineLength);
        // E.row.chars[lineLength] = '\0';
        // E.numOfRows = 1;
    }
    free(line);
    E.currentRow = E.l.head;
    setEditorStatus(0, "File opened Successfully");
    fclose(fp);
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
void gapBuffertoRows()
{
    vnode *p = E.l.head;
    do
    {
        if (p->row.gapBuffer != NULL)
        {
            memmove(&p->row.gapBuffer[p->row.gap_left], &p->row.gapBuffer[p->row.gap_right + 1], p->row.gsize - p->row.gap_right);
            // exchanging pointers
            char *tmp = p->row.chars;
            // printf("%s", p->row.gapBuffer);
            p->row.chars = p->row.gapBuffer;
            free(tmp);
            p->row.gapBuffer = NULL;
        }
        p = p->next;
    } while (p != E.l.tail);
}

char *dataStructureToString(int *totalLength)
{
    int len = 0;
    vnode *p = E.l.head;
    gapBuffertoRows();
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
    mvwprintw(win[MENU_WINDOW], 1, 25, "%d", len);
    return result;
}
void saveFile()
{
    int buflen = 0;
    char *buf = dataStructureToString(&buflen);
    FILE *fp = fopen(E.fname, "w");
    if (fp == NULL)
    {
        return;
    }
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
}

void editorInsertChar(int c)
{
    if (iscntrl(c))
    {
        flushinp();
        return;
    }
    if (E.Cy + E.y_offset == E.numOfRows)
        appendRow(&E.l, "", 0);
    editorRowInsertChar(&E.currentRow->row, E.Cx + E.x_offset - DEFPOS_X, c);
    E.Cx++;
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
        if (p->row.gapBuffer != NULL)
        {
            while (x < p->row.gsize && x < LIMIT_X + E.x_offset)
            {
                // if (E.l.head->row.size > LIMIT_X)
                if (p->row.gapBuffer[x] != '\0')
                    waddch(win[EDIT_WINDOW], p->row.gapBuffer[x]);
                x++;
            }
        }
        else
        {

            while (x < p->row.size && x < LIMIT_X + E.x_offset)
            {
                // if (E.l.head->row.size > LIMIT_X)
                waddch(win[EDIT_WINDOW], p->row.chars[x]);
                x++;
            }
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
    setEditorStatus(0, "gap->size=%d gap->left=%d gap->right=%d gsize=%d row->size=%d", E.currentRow->row.gap_size, E.currentRow->row.gap_left, E.currentRow->row.gap_right, E.currentRow->row.gsize, E.currentRow->row.size);
}
void read_key()
{
    int c = wgetch(win[EDIT_WINDOW]);
    // wmove(win[EDIT_WINDOW], 10, 0);

    // printw("%c", c);
    wprintw(win[INFO_WINDOW], "%d %d", E.screenCols, E.screenRows);
    switch (c)
    {
    case CTRL_KEY('q'):
        endwin();
        exit(EXIT_SUCCESS);
        break;
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
        break;
    case CTRL_KEY('s'):
        saveFile();
    default:
        editorInsertChar(c);
        break;
    }
}
int main(int argc, char *argv[])
{
    initscr();
    editor_init();
    if (argc == 2)
        openFile(argv[1]);
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