#include "cmd.h"
#include "list.h"
#include "misc.h"
#include "uv.h"

#include <stdio.h>
#include <stdlib.h>

#include "readline/history.h"
#include "readline/readline.h"

char *ds_program_name;
char *prompt_status = "";

static char prompt[256];
static uv_poll_t rl_poll;
static uv_signal_t sigint, sigwinch;

LIST_HEAD (cmd_list);
struct cmd_entry {
    struct list_head cmd_list;
    char *name;
    char *arg;
    char *help;
    int (*func) (int ac, char *av[]);
    int f_enable;
};

void
ds_set_prompt_status (char *status)
{
    prompt_status = status;
    sprintf (prompt, "%s %s> ", ds_program_name, prompt_status);
}

void
ds_cmd_install (char *name, char *arg, char *help, int (*func) (int ac, char *av[]))
{
    struct cmd_entry *cmd;

    cmd = malloc (sizeof (*cmd));
    if (!cmd) {
        exit (-1);
    }

    cmd->name = name;
    cmd->arg = arg;
    cmd->help = help;
    cmd->func = func;

    list_insert (&cmd_list, &cmd->cmd_list);
}

int
ds_cmd_quit (int ac, char *av[])
{
    ds_end_loop ();
    return 0;
}

void
ds_cmd_standard_install ()
{
    ds_cmd_install ("quit", "", "quit", ds_cmd_quit);
}

static int
ds_cmd_execute (char *s)
{
    char **tokens = history_tokenize (s);
    int i;

    for (i = 0; tokens[i]; i++) {
        free (tokens[i]);
    }

    free (tokens);
    return 0;
}

static void
ds_linehandler_cb (char *line)
{
    char *hist_expand;
    int ret;

    if (!line) {
        ds_end_loop ();
        return;
    }

    if (!line[0]) {
        goto exit;
    }

    ret = history_expand (line, &hist_expand);
    if (ret >= 0 && ret != 2) {
        add_history (hist_expand);
        ds_cmd_execute (hist_expand);
    }

    free (hist_expand);

exit:
    free (line);
}

void
ds_stdin_cb (uv_poll_t *handle, int status, int events)
{
    rl_callback_read_char ();
}

void
ds_signal_cb (uv_signal_t *handle, int signum)
{
    if (signum == SIGINT) {
        rl_free_line_state ();
        rl_cleanup_after_signal ();
        RL_UNSETSTATE (RL_STATE_ISEARCH | RL_STATE_NSEARCH | RL_STATE_VIMOTION | RL_STATE_NUMERICARG | RL_STATE_MULTIKEY);
        rl_done = 1;
        rl_callback_handler_remove ();
        printf ("\n");

        rl_callback_handler_install (prompt, ds_linehandler_cb);
    }

    if (signum == SIGWINCH) {
        rl_resize_terminal ();
    }
}

void
ds_cmd_init ()
{
    ds_set_prompt_status (" ");
    rl_initialize ();
    rl_callback_handler_install (prompt, ds_linehandler_cb);

    uv_poll_init (uv_default_loop (), &rl_poll, fileno (rl_instream));
    uv_poll_start (&rl_poll, UV_READABLE, ds_stdin_cb);

    uv_signal_init (uv_default_loop (), &sigint);
    uv_signal_start (&sigint, ds_signal_cb, SIGINT);
    uv_signal_init (uv_default_loop (), &sigwinch);
    uv_signal_start (&sigwinch, ds_signal_cb, SIGWINCH);
}

void
ds_cmd_deinit ()
{
}
