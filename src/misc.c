#include "misc.h"
#include "uv.h"
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
