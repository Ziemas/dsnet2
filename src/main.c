#include "cmd.h"
#include "misc.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int dsnetm_main (int argc, char *argv[], int id);

struct program {
    char *name;
    int (*func) (int, char **, int);
    int id;
};

struct program prog_list[] = {
    {"dsnetm", dsnetm_main, 0},
    {NULL},
};

int
main (int argc, char *argv[])
{

    struct program *prog = NULL;
    int i;

    ds_program_name = ds_basename (argv[0]);

    for (i = 0; prog_list[i].name; i++) {
        if (!strcmp (ds_program_name, prog_list[i].name)) {
            prog = &prog_list[i];
            break;
        }
    }

    if (!prog) {
        printf ("No such program implemented: %s\n", ds_program_name);
        return -1;
    }

    return prog->func(argc, argv, prog->id);
}
