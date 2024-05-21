#ifndef TERMINAL_H
#define TERMINAL_H


void disable_raw_mode();
void enable_raw_mode();
int get_cursor_position(int *rows, int *cols);
int get_window_size(int *rows, int *cols);


#endif
