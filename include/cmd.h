#ifndef CMD_H_
#define CMD_H_

extern char *ds_program_name;

void ds_cmd_init ();
void ds_cmd_deinit ();
void ds_cmd_install (char *name, char *arg, char *help, int (*func) (int ac, char *av[]));
void ds_cmd_standard_install ();

#endif // CMD_H_
