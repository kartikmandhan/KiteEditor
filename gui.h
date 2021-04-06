#ifndef __GUI_H__
#define __GUI_H__

#include "editor.h"
#define MENU_CLR COLOR_PAIR(1)
#define EDIT_CLR COLOR_PAIR(2)
#define POPUP_CLR COLOR_PAIR(3)
#define BORDER_CLR COLOR_PAIR(4)

void draw_window(enum win_type wt);
void draw_menu(enum win_type wt);
void draw_edit(enum win_type wt);
void draw_info(enum win_type wt);
void save_file_popup(void);
#endif // !__GUI_H__