#ifndef MISC_H_
#define MISC_H_

char *ds_basename (char *path);
void ds_fork ();
void ds_end_loop ();
char *ds_format_new (char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
char *ds_get_conf_dir ();
char *ds_get_state_dir ();

#endif // MISC_H_
