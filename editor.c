
#include "editor.h"
#include "init.h"
#include "gui.h"
void editor_init()
{
    E.Cx = DEFPOS_X;
    E.Cy = DEFPOS_Y;
    noecho();
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

void editorMoveCursor(int key)
{
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
        wprintw(win[EDIT_WINDOW], "key home");
        E.Cx = DEFPOS_X;
        E.Cy = DEFPOS_Y;
        break;
    case KEY_END:
        wprintw(win[EDIT_WINDOW], "key end");
        E.Cx = DEFPOS_X;
        E.Cy = LIMIT_Y;
        break;

    default:
        mvprintw(10, 5, "else");
        break;
    }
}
int main()
{
    initscr();
    editor_init();
    // mvprintw(E.screenCols / 2, E.screenRows / 2 - 10, "welcome to my editor");
    while (1)
    {
        read_key();
        // refresh();
        wmove(win[EDIT_WINDOW], E.Cy, E.Cx);

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