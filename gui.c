#include "gui.h"
void draw_window(enum win_type wt)
{
    wattron(win[wt], BORDER_CLR);
    box(win[wt], ACS_VLINE, ACS_HLINE);
    wattroff(win[wt], BORDER_CLR);
    switch (wt)
    {
    case MENU_WINDOW:
        draw_menu(wt);
        break;
    case EDIT_WINDOW:
        draw_edit(wt);
        break;
    case INFO_WINDOW:
        draw_info(wt);
        break;
    default:
        break;
    }
    wrefresh(win[wt]);
}
void draw_menu(enum win_type wt)
{

    wbkgd(win[wt], BORDER_CLR);

    int offset = 2;

    wattron(win[wt], MENU_CLR);
    mvwprintw(win[wt], 1, offset, "  F1 - Open  ");
    offset += 15;
    mvwprintw(win[wt], 1, offset, "  F2 - Save  ");
    offset += 15;
    mvwprintw(win[wt], 1, offset, "  F3 - Extra  ");
    offset += 16;
    mvwprintw(win[wt], 1, offset, "  F4 - Help  ");
    offset += 15;
    mvwprintw(win[wt], 1, offset, "  F5 - Exit  ");
    mvwprintw(win[wt], 1, COLS - 20, " made by Kartik ");
    if (E.dirtyFlag)
        mvwprintw(win[wt], 3, COLS - 20, "(modified)");
    wattroff(win[wt], MENU_CLR);
}
void draw_edit(enum win_type wt)
{
    wbkgd(win[wt], EDIT_CLR);
}
void draw_info(enum win_type wt)
{
    wbkgd(win[wt], MENU_CLR);
    wattron(win[wt], COLOR_PAIR(2));
    // wattron(win[wt], A_REVERSE);
    mvwprintw(win[INFO_WINDOW], 0, COLS / 2 - 16,
              " y: %3d, x: %3d, yo: %4d, xo: %4d ",
              E.Cy, E.Cx,
              E.y_offset, E.x_offset);
    // wattroff(win[wt], A_REVERSE);
    wattroff(win[wt], COLOR_PAIR(2));
    wattron(win[wt], MENU_CLR);
    int offset = 4;
    char fname[20] = "[No Name]";
    int namelen = strlen(E.fname);
    if (namelen > 16)
    {
        strncpy(fname, E.fname + namelen - 16, sizeof(char) * 19);
        fname[0] = fname[1] = fname[2] = '.';
        fname[16] = '\0';
        mvwprintw(win[wt], 2, offset, "FILE: %s", E.fname);
        offset += 26;
    }
    else
    {
        strcpy(fname, E.fname);
        mvwprintw(win[wt], 2, offset, "FILE: %s", E.fname);
        offset += 10 + namelen;
    }
    // if (statusLength > offset-COLS-2)
    // {
    //     E.statusMessage[COLS - 1] = '\0';
    // }
    if (E.status == 1)
    {
        mvwprintw(win[wt], 2, offset, "Error: ");
        offset += 7;
    }
    mvwprintw(win[wt], 2, offset, "%s", E.statusMessage);
    mvwprintw(win[wt], 2, COLS - 10, "v1.0");
    if (E.dirtyFlag)
        mvwprintw(win[wt], 3, COLS - 20, "(modified)");
    wattroff(win[wt], MENU_CLR);
}
void save_file_popup(void)
{
    WINDOW *savewin;

    int win_height = 3;
    int win_width = 42;
    int offset_y = LINES / 2;
    int offset_x = COLS / 2;

    savewin = newwin(win_height, win_width,
                     offset_y - win_height / 2,
                     offset_x - win_width / 2);

    wattron(savewin, BORDER_CLR);
    box(savewin, ACS_VLINE, ACS_HLINE);
    wattroff(savewin, BORDER_CLR);
    wbkgd(savewin, POPUP_CLR);

    echo();

    mvwaddstr(savewin, 1, 1, " Enter file name: ");
    wrefresh(savewin);
    memset(E.fname, '\0', FILENAME_MAX);
    mvwgetstr(savewin, 1, 19, E.fname);

    flushinp();
    noecho();

    wclear(savewin);
    wrefresh(savewin);
    delwin(savewin);
    werase(win[EDIT_WINDOW]);
    draw_window(EDIT_WINDOW);
}
void open_file_popup(void)
{
    WINDOW *openwin;

    int win_height = 3;
    int win_width = 42;
    int offset_y = LINES / 2;
    int offset_x = COLS / 2;

    openwin = newwin(win_height, win_width,
                     offset_y - win_height / 2,
                     offset_x - win_width / 2);

    wattron(openwin, BORDER_CLR);
    box(openwin, ACS_VLINE, ACS_HLINE);
    wattroff(openwin, BORDER_CLR);
    wbkgd(openwin, POPUP_CLR);

    echo();

    mvwaddstr(openwin, 1, 1, " Enter file name: ");
    wrefresh(openwin);

    memset(E.fname, '\0', FILENAME_MAX);
    mvwgetstr(openwin, 1, 19, E.fname);

    flushinp();
    noecho();

    wclear(openwin);
    wrefresh(openwin);
    delwin(openwin);
    werase(win[EDIT_WINDOW]);
    draw_window(EDIT_WINDOW);
}
char *search_file_popup(WINDOW *findwin)
{

    int win_height = 2;
    int win_width = 42;
    int offset_y = LINES - 3;
    int offset_x = COLS / 2;

    findwin = newwin(win_height, win_width,
                     offset_y - win_height / 2,
                     offset_x - win_width / 2);

    wbkgd(findwin, POPUP_CLR);

    echo();
    flushinp();
    mvwaddstr(findwin, 1, 1, " Search :");
    wrefresh(findwin);
    char *query = (char *)malloc(sizeof(char) * 60);
    mvwgetstr(findwin, 1, 10, query);
    setEditorStatus(0, "Searching: %s", query);
    noecho();
    wclear(findwin);
    wrefresh(findwin);
    delwin(findwin);
    werase(win[INFO_WINDOW]);
    draw_window(INFO_WINDOW);
    if (strlen(query) == 0)
    {
        setEditorStatus(0, "Searching: aborted");
        return NULL;
    }
    return query;
}