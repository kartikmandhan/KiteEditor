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
    getmaxyx(win[EDIT_WINDOW], E.screenCols, E.screenRows);
    E.screenCols -= 2;
    E.screenRows -= 2;

    init_gui();
    // wrefresh(win[1]);
    // init
    raw();
    wmove(win[EDIT_WINDOW], E.Cy, E.Cx);
}
void openFile(char *filename)
{
    FILE *fp = fopen(filename, "r");

    if (!fp)
        handleError("fopen");
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
        appendRow(&E.l, line, lineLength);
        // E.row.size = lineLength;
        // E.row.chars = (char *)malloc(lineLength + 1);
        // memcpy(E.row.chars, line, lineLength);
        // E.row.chars[lineLength] = '\0';
        // E.numOfRows = 1;
    }
    free(line);
    fclose(fp);
}
void move_cursor_right()
{
}
void print_text()
{
    // initialising cursor to start of the editor
    // E.Cx = DEFPOS_X;
    // E.Cy = DEFPOS_Y;
    // wmove(win[EDIT_WINDOW], E.Cy, E.Cx);
    int x = 0, y = 0;
    vnode *p = E.l.head;
    while (y < E.numOfRows && y < LIMIT_Y)
    {
        wmove(win[EDIT_WINDOW], y + 1, 1);
        x = 0;

        while (x < p->row.size && x < LIMIT_X)
        {
            // if (E.l.head->row.size > LIMIT_X)
            waddch(win[EDIT_WINDOW], p->row.chars[x]);
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
}
void editorMoveCursor(int key)
{
    // wprintw(win[EDIT_WINDOW], "%d %d", E.Cx, E.Cy);
    // wmove(win[EDIT_WINDOW], E.Cy, E.Cx);
    switch (key)
    {
    case KEY_LEFT:
        if (E.Cx > DEFPOS_X)
            E.Cx--;

        break;
    case KEY_RIGHT:
        if (E.Cx < LIMIT_X)
            E.Cx++;

        break;
    case KEY_UP:
        if (E.Cy > DEFPOS_Y)
            E.Cy--;

        break;
    case KEY_DOWN:
        if (E.Cy < LIMIT_Y)
            E.Cy++;

        break;
    }
}
void read_key()
{
    // wrefresh(win[EDIT_WINDOW]);
    int c = wgetch(win[EDIT_WINDOW]);
    // wmove(win[EDIT_WINDOW], 10, 0);

    // printw("%c", c);
    // wprintw(win[EDIT_WINDOW], "%d %d", E.screenCols, E.screenRows);
    switch (c)
    {
    case CTRL_KEY('q'):
        printw("%c", c);

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
        // wprintw(win[EDIT_WINDOW], "key home");
        E.Cx = DEFPOS_X;
        E.Cy = DEFPOS_Y;
        break;
    case KEY_END:
        // wprintw(win[EDIT_WINDOW], "key end");
        E.Cx = DEFPOS_X;
        E.Cy = LIMIT_Y;
        break;

    default:
        mvprintw(10, 5, "else");
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