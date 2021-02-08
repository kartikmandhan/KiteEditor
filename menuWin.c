#include <ncurses.h>
int main()
{
    initscr();
    noecho();
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    WINDOW *menuwin = newwin(6, xMax - 12, yMax - 8, 5);
    box(menuwin, 0, 0);
    refresh();
    wrefresh(menuwin);
    // so that we can use special macros like KEY_UP
    keypad(menuwin, true);
    char *choices[3] = {"File", "Edit", "View"};
    int choice;
    int highlight = 0;
    while (1)
    {
        for (int i = 0; i < 3; i++)
        {
            if (i == highlight)
                wattron(menuwin, A_REVERSE);
            mvwprintw(menuwin, i + 1, 1, choices[i]);
            wattroff(menuwin, A_REVERSE);
        }
        choice = wgetch(menuwin);
        switch (choice)
        {
        case KEY_UP:
            highlight = (highlight + 2) % 3;

            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % 3;

            break;

        default:
            break;
        }
        // 10 is for enter key
        if (choice == 10)
        {
            break;
        }
    }
    printw("Your choice was %s", choices[highlight]);

    getch();
    endwin();
}