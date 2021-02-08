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
    if (can_change_color())
    {
        // color green is changed to rgb value as described

        // init_color(macro of color to be changed, 0-999, 0-999, 0-999);
        init_color(COLOR_BLUE, 121, 168, 215);
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

// void handleError(const char *s)
// {
//     perror(s);
//     exit(1);
// }
// void disableRawMode()
// {
//     printf("Thanks for using my editor");
//     if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
//         handleError("tcsetattr");
// }
// void enableRawMode()
// {
//     if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
//         handleError("tcgetattr");
//     atexit(disableRawMode);
//     // atexit() comes from <stdlib.h>. We use it to register our disableRawMode() function to be called automatically when the program exits, whether it exits by returning from main(), or by calling the exit() function. This way we can ensure we’ll leave the terminal attributes the way we found them when our program exits.
//     struct termios raw = E.orig_termios;
//     // There is an ICANON flag that allows us to turn off canonical mode. This means we will finally be reading input byte-by-byte, instead of line-by-line.
//     // raw.c_lflag &= ~(ECHO);
//     // By default, Ctrl-C sends a SIGINT signal to the current process which causes it to terminate, and Ctrl-Z sends a SIGTSTP signal to the current process which causes it to suspend. Let’s turn off the sending of both of these signals.
//     // By default, Ctrl-S and Ctrl-Q are used for software flow control. Ctrl-S stops data from being transmitted to the terminal until you press Ctrl-Q. This originates in the days when you might want to pause the transmission of data to let a device like a printer catch up. Let’s just turn off that feature.
//     // By default, Ctrl-S and Ctrl-Q are used for software flow control. Ctrl-S stops data from being transmitted to the terminal until you press Ctrl-Q. This originates in the days when you might want to pause the transmission of data to let a device like a printer catch up. Let’s just turn off that feature.
//     // On some systems, when you type Ctrl-V, the terminal waits for you to type another character and then sends that character literally. For example, before we disabled Ctrl-C, you might’ve been able to type Ctrl-V and then Ctrl-C to input a 3 byte. We can turn off this feature using the IEXTEN flag.
//     // It turns out that the terminal is helpfully translating any carriage returns (13, '\r') inputted by the user into newlines (10, '\n'). Let’s turn off this feature.
//     // ICRNL comes from <termios.h>. The I stands for “input flag”, CR stands for “carriage return”, and NL stands for “new line”.
//     // Now Ctrl-M is read as a 13 (carriage return), and the Enter key is also read as a 13.
//     // raw.c_iflag &= ~(ICRNL | IXON);
//     // We will turn off all output processing features by turning off the OPOST flag. In practice, the "\n" to "\r\n" translation is likely the only output processing feature turned on by default.
//     // From now on, we’ll have to write out the full "\r\n" whenever we want to start a new line.
//     raw.c_oflag &= ~(OPOST);
//     // miscelleneous flags:
//     raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
//     raw.c_cflag |= (CS8);
//     raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
//     if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
//         handleError("tcsetattr");
// }
// void editor_init()
// {
//     noecho();
//     keypad(stdscr, TRUE);
//     getmaxyx(stdscr, E.screenCols, E.screenRows);
//     if (LINES < 11 || COLS < 98)
//     {
//         endwin();
//         fprintf(stderr, "Terminal window is too small.\n"
//                         "Min: 11x98, your: %dx%d\n",
//                 LINES, COLS);
//         exit(EXIT_FAILURE);
//     }
//     enableRawMode();
// }