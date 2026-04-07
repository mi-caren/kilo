#ifndef EDITOR_H
#define EDITOR_H

#include "editor/defs.h"
#include <termios.h>
#include <stdbool.h>


void editorInit(Editor* ed);
Res(void) editorOpen(char *filename);
void editorRun();


#endif
