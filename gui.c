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
    mvwprintw(win[wt], 1, offset, "  F4 - Open  ");
    offset += 15;
    mvwprintw(win[wt], 1, offset, "  F5 - Save  ");
    offset += 15;
    mvwprintw(win[wt], 1, offset, "  F6 - Extra  ");
    offset += 16;
    mvwprintw(win[wt], 1, offset, "  F7 - Help  ");
    offset += 15;
    mvwprintw(win[wt], 1, offset, "  F8 - Exit  ");
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
}
/*void change_theme(int popup)
{
    int fg_field, bg_field,
        fg_menu, bg_menu,
        fg_popup, bg_popup;

    if (popup)
        E.current_theme = (char)change_theme_popup();

    switch (E.current_theme)
    {
    case '0': // default
        fg_menu = COLOR_BLACK, bg_menu = COLOR_YELLOW;
        fg_field = COLOR_CYAN, bg_field = COLOR_BLUE;
        fg_popup = COLOR_YELLOW, bg_popup = COLOR_BLACK;
        break;
    case '1': // leet
        fg_menu = COLOR_BLACK, bg_menu = COLOR_GREEN;
        fg_field = COLOR_GREEN, bg_field = COLOR_BLACK;
        fg_popup = COLOR_GREEN, bg_popup = COLOR_BLACK;
        break;
    case '2': // icy
        fg_menu = COLOR_CYAN, bg_menu = COLOR_BLACK;
        fg_field = COLOR_MAGENTA, bg_field = COLOR_WHITE;
        fg_popup = COLOR_CYAN, bg_popup = COLOR_BLACK;
        break;
    case '3': // hell
        fg_menu = COLOR_BLACK, bg_menu = COLOR_RED;
        fg_field = COLOR_RED, bg_field = COLOR_BLACK;
        fg_popup = COLOR_RED, bg_popup = COLOR_BLACK;
        break;
    default:
        // set_status(0, "...");
        return;
    }

    init_pair(1, fg_menu, bg_menu);
    wattrset(win[MENU_WINDOW], MENU_CLR);

    init_pair(2, fg_field, bg_field);
    wattrset(win[EDIT_WINDOW], EDIT_CLR);

    init_pair(3, fg_popup, bg_popup);
    wattrset(win[INFO_WINDOW], POPUP_CLR);

    if (popup)
    {
        // set_status(0, "theme successfully changed to '%c'",E.current_theme);
    }
}
*/