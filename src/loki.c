#include <stdlib.h>
#include <unistd.h>

#include "aeolus/result.h"
#include "editor.h"
#include "editor/defs.h"

extern struct Editor editor;

int main(int argc, char *argv[]) {
    // Editor ed;
    editorInit(&editor);

    if (argc >= 2) {
        errdie(void, editorOpen(argv[1]), "open: open file failed");
    }

    editorRun();

    return EXIT_SUCCESS;
}
