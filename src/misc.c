#include "misc.h"
#include "uv.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *
ds_basename (char *path)
{
    char *b = strrchr (path, '/');
    return b ? b + 1 : b;
}

void
ds_fork ()
{
    if (fork ()) {
        exit (0);
    }

    setsid ();
}

void
ds_end_loop ()
{
    uv_stop (uv_default_loop ());
}

char *
ds_vformat_new (char *fmt, va_list va)
{
    char *str = NULL;
    va_list copy;
    int n;

    va_copy (copy, va);
    n = vsnprintf (NULL, 0, fmt, va);
    if (n < 0) {
        goto end;
    }

    str = malloc (n + 1);
    vsnprintf (str, n + 1, fmt, copy);

end:
    va_end (copy);
    return str;
}

__attribute__ ((format (printf, 1, 2))) char *
ds_format_new (char *fmt, ...)
{
    char *str;
    va_list va;

    va_start (va, fmt);
    str = ds_vformat_new (fmt, va);
    va_end (va);

    return str;
}

char *
ds_get_conf_dir ()
{
    char *home = getenv ("HOME");
    char *xdg_config = getenv ("XDG_CONFIG_HOME");

    return NULL;
}

char *
ds_get_state_dir ()
{
    char *home = getenv ("HOME");
    char *xdg_config = getenv ("XDG_STATE_HOME");

    if (xdg_config) {
    }

    return NULL;
}
