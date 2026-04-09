#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <stdbool.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "editing_point.h"
#include "editor.h"
#include "editor/defs.h"
#include "editor/commands.h"
#include "editor/utils.h"
#include "terminal.h"
#include "status_bar.h"
#include "aeolus/vec/vec.h"
#include "aeolus/result.h"
#include "aeolus/utils.h"


struct Editor editor;


void _abortHandler() {
    WRITE_SEQ(LEAVE_ALTERNATE_SCREEN);
    write(STDOUT_FILENO, "\r\n", 2);
}

static void editor_die_hook() {
    WRITE_SEQ(LEAVE_ALTERNATE_SCREEN);
}

static void errdbg_pre_hook() {
    WRITE_SEQ(LEAVE_ALTERNATE_SCREEN);
}

static void errdbg_post_hook() {
    WRITE_SEQ(ENTER_ALTERNATE_SCREEN);
}

void editorInit(Editor* ed) {
    // We immediately switch to alternate screen
    // because terminalInit function can mess the terminal by moving the cursor
    WRITE_SEQ(ENTER_ALTERNATE_SCREEN);

    set_die_hook(editor_die_hook);
    set_errdbg_hooks(errdbg_pre_hook, errdbg_post_hook);
    errdie(void, terminalInit(), "init: terminal init failed");

    atexit(terminalDeinit);

    ed->editing_point = (EditingPoint) 0;
    ed->rx = 0;
    vec_init(EditorRow, &ed->rows);
    ed->rowoff = 0;
    ed->coloff = 0;
    ed->filename = NULL;
    ed->message_bar_buf[0] = '\0';
    ed->message_bar_time = 0;
    ed->dirty = false;

    ed->searching = false;
    ed->search_query = NULL;

    ed->selecting = false;
    ed->selection_start = 0;
    vec_init(char, &ed->copy_buf);

    vec_init(Command, &ed->command_history);
    ed->curr_history_cmd = NULL;

    int height = terminal.screenrows;
    if (height < 0) {
        ed->view_rows = 0;
    } else if (height <= 2) {
        ed->view_rows = height;
    }
    ed->view_rows = height - 2;
}

Res(void) editorOpen(char *filename) {
    free(editor.filename);
    char *new = strdup(filename);
    if (new == NULL)
        return err(void, "strdup");
    editor.filename = new;

    FILE *fp = fopen(filename, "r");
    if (!fp)
        return err(void, strerror(errno));

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    Err open_err = NULL;
    while((linelen = getline(&line, &linecap, fp)) != -1) {
        // strip off newline or carriage retur at the end of line
        while (linelen > 0 && (line[linelen - 1] == '\n' ||
                               line[linelen - 1] == '\r')) {
            line[--linelen] = '\0';
        }

        if (editorInsertRow(editor.rows.len, line) != 0) {
            open_err = "insert row";
            goto cleanup;
        }
    }

cleanup:
    free(line);
    fclose(fp);
    if (open_err)
        return err(void, open_err);
    else
        return ok(void);
}

void editorRun() {
    messageBarSet("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");
    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
}
