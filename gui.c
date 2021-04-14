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
    mvwprintw(win[INFO_WINDOW], 0, COLS / 2 - 20,
              " y: %3d, x: %3d, yo: %4d, xo: %4d, LN: %4d ",
              E.Cy, E.Cx,
              E.y_offset, E.x_offset, (E.Cy + E.y_offset));
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
        mvwprintw(win[wt], 3, COLS - 15, "(modified)");
    if (E.syntax)
        mvwprintw(win[wt], 3, 4, "Filetype:%s", E.syntax->filetype);
    else
        mvwprintw(win[wt], 3, 4, "Filetype:txt");
    wattroff(win[wt], MENU_CLR);
    if (E.clip.chars)
    {
        int clipLen = strlen(E.clip.chars);
        int availableLen = LIMIT_X - 50;
        if (clipLen < availableLen)
            mvwprintw(win[wt], 3, 19, "| ClipBoard:%s", E.clip.chars);
        else
        {
            char clip[100];
            strncpy(clip, E.clip.chars, availableLen - 1);
            clip[availableLen - 3] = clip[availableLen - 2] = clip[availableLen - 1] = '.';
            mvwprintw(win[wt], 3, 19, "| ClipBoard:%s", clip);
        }
    }
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
int change_theme_popup(void)
{
    WINDOW *theme_win;

    int win_height = 7;
    int win_width = 15;
    int line = 1;
    int offset_y = LINES / 2;
    int offset_x = COLS / 2;

    theme_win = newpad(win_height, win_width);
    wattron(theme_win, BORDER_CLR);
    box(theme_win, ACS_VLINE, ACS_HLINE);
    wattroff(theme_win, BORDER_CLR);
    wbkgd(theme_win, POPUP_CLR);
    curs_set(0);

    wmove(theme_win, line++, 1);
    waddstr(theme_win, " Themes: ");
    wmove(theme_win, line++, 1);
    waddstr(theme_win, " [1] default ");
    wmove(theme_win, line++, 1);
    waddstr(theme_win, " [2] earth ");
    wmove(theme_win, line++, 1);
    waddstr(theme_win, " [3] icy  ");
    wmove(theme_win, line++, 1);
    waddstr(theme_win, " [4] hell ");

    prefresh(theme_win, 0, 0,
             offset_y - win_height / 2,
             offset_x - win_width / 2,
             offset_y + win_height / 2,
             offset_x + win_width / 2);

    int choice = wgetch(theme_win);

    flushinp();
    wclear(theme_win);
    wrefresh(theme_win);
    delwin(theme_win);
    curs_set(1);
    werase(win[EDIT_WINDOW]);
    draw_window(EDIT_WINDOW);
    return choice;
}
void change_theme(int popup)
{
    int fg_field, bg_field,
        fg_menu, bg_menu,
        fg_popup, bg_popup;

    if (popup)
        E.current_theme = change_theme_popup();

    switch (E.current_theme)
    {
    case 49: // default
        fg_menu = COLOR_BLACK, bg_menu = COLOR_YELLOW;
        fg_field = COLOR_CYAN, bg_field = COLOR_BLUE;
        fg_popup = COLOR_YELLOW, bg_popup = COLOR_BLACK;
        break;
    case 50: // leet
        fg_menu = COLOR_BLACK, bg_menu = COLOR_GREEN;
        fg_field = COLOR_GREEN, bg_field = COLOR_BLACK;
        fg_popup = COLOR_GREEN, bg_popup = COLOR_BLACK;
        break;
    case 51: // icy
        fg_menu = COLOR_CYAN, bg_menu = COLOR_BLACK;
        fg_field = COLOR_BLACK, bg_field = COLOR_WHITE;
        fg_popup = COLOR_CYAN, bg_popup = COLOR_BLACK;
        break;
    case 52: // hell
        fg_menu = COLOR_BLACK, bg_menu = COLOR_RED;
        fg_field = COLOR_RED, bg_field = COLOR_BLACK;
        fg_popup = COLOR_RED, bg_popup = COLOR_BLACK;
        break;
    default:
        setEditorStatus(0, "Invalid Theme Choice");
        return;
    }

    init_pair(1, fg_menu, bg_menu);
    wattrset(win[MENU_WINDOW], MENU_CLR);

    init_pair(2, fg_field, bg_field);
    wattrset(win[EDIT_WINDOW], EDIT_CLR);

    init_pair(3, fg_popup, bg_popup);
    wattrset(win[INFO_WINDOW], POPUP_CLR);

    if (popup)
        setEditorStatus(0, "theme successfully changed");
}