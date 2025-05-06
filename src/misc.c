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

/* XDG basedir lookup */

enum {
    XDG_CONFIG_HOME,
    XDG_STATE_HOME,
};

struct xdg_dir {
    char *env;
    char *fallback;
    char *cache;
};

static struct xdg_dir xdg_dir[] = {
    [XDG_CONFIG_HOME] = {"XDG_CONFIG_HOME", ".config"},
    [XDG_STATE_HOME] = {"XDG_STATE_HOME", ".local/state"},
};

static char *
ds_get_xdg_dir (int i)
{
    char *path, *home;
    size_t len;

    if ((path = getenv (xdg_dir[i].env))) {
        return strdup (path);
    }

    if ((home = getenv ("HOME")) == NULL) {
        home = getpwuid (getuid ())->pw_dir;
    }

    return ds_format_new ("%s/%s/%s/", home, xdg_dir[i].fallback, "dsnet");
}

int
ds_mkpath (char *filepath)
{
    char *p = filepath;
    int ret;

    if (!filepath) {
        return -1;
    }

    while ((p = strchr (p + 1, '/'))) {
        *p = 0;
        ret = mkdir (filepath, 0755);
        *p = '/';

        if (ret && errno != EEXIST) {
            return -1;
        }
    }

    return 0;
}

char *
ds_get_conf_dir ()
{
    char *path = ds_get_xdg_dir (XDG_CONFIG_HOME);

    if (ds_mkpath (path)) {
        return NULL;
    }

    return path;
}

char *
ds_get_state_dir ()
{
    char *path = ds_get_xdg_dir (XDG_STATE_HOME);

    if (ds_mkpath (path)) {
        return NULL;
    }

    return path;
}
