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
    int statusLength = strlen(E.statusMessage);
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
