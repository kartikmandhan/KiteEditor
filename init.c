#include "init.h"

int init_colors()
{
    if (start_color() == ERR || !has_colors())
    {
        fprintf(stderr, "This terminal doesn't support colouring.\n");
        return ERR;
    }

    // Pair id, Foreground, Background
    init_pair(1, COLOR_BLACK, COLOR_YELLOW); // menu components
    init_pair(2, COLOR_CYAN, COLOR_BLUE);    // edit field
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); // pop-up windows
    init_pair(4, COLOR_WHITE, COLOR_BLACK);  // borders
    // syntax Highlighting colors
    init_pair(5, COLOR_RED, COLOR_BLUE);
    init_pair(6, COLOR_GREEN, COLOR_BLUE);
    init_pair(7, COLOR_YELLOW, COLOR_BLUE);
    init_pair(8, COLOR_WHITE, COLOR_BLUE);
    if (can_change_color())
    {
        // color green is changed to rgb value as described

        // init_color(macro of color to be changed, 0-999, 0-999, 0-999);
        init_color(COLOR_BLUE, 121, 168, 215);
        init_color(COLOR_GREEN, 47, 955, 0);
        init_color(COLOR_CYAN, 458, 984, 890);
        init_color(COLOR_BLACK, 126, 127, 131);
        init_color(COLOR_YELLOW, 639, 866, 796);
    }
    return OK;
}
void init_gui()
{
    for (int i = 0; i < WINDOWS_COUNT; i++)
    {
        draw_window(i);
        // wrefresh(win[i]);
    }
}

void init_windows(void)
{ //height, width, start_y, start_x
    win[MENU_WINDOW] = newwin(3, COLS, 0, 0);

    win[EDIT_WINDOW] = newwin(LINES - 8, COLS, 3, 0);

    win[INFO_WINDOW] = newwin(5, COLS, LINES - 5, 0);

    // Enable scrolling, func keys, arrows etc
    keypad(win[EDIT_WINDOW], TRUE);
}
