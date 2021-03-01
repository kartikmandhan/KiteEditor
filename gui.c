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
    wattron(win[wt], MENU_CLR);
    int offset = 2;
    mvwprintw(win[wt], 1, offset, "E.Cy=%d", E.Cy);
    offset += 10;
    mvwprintw(win[wt], 1, offset, "E.Cx=%d", E.Cx);
    offset += 20;
    mvwprintw(win[wt], 1, offset, "E.x_offset=%d", E.x_offset);
    offset += 20;
    mvwprintw(win[wt], 1, offset, "E.y_offset=%d", E.y_offset);
    offset += 20;
    mvwprintw(win[wt], 1, offset, "E.numOfRows=%d", E.numOfRows);

    wattroff(win[wt], MENU_CLR);
}
