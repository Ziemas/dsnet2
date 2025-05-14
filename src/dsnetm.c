#include "cmd.h"
#include "deci2.h"
#include "list.h"
#include "misc.h"
#include "uv.h"

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>

static uv_tcp_t tcp_server;
static uv_handle_t *server;

LIST_HEAD (proto_list);

struct proto_handler {
    int protocol;
    int priority;
    int (*handler) (uv_stream_t *, const uv_buf_t *);
    struct list_head hnd_list;
};

struct proto {
    struct list_head proto_list;
    int proto_id;
    struct list_head handlers;
};

static void
after_read (uv_stream_t *handle, ssize_t nread,
            const uv_buf_t *buf)
{
    struct deci2_hdr *hdr = (struct deci2_hdr *) buf->base;
    struct proto_handler *hnd, *hnd_strongest;
    uv_shutdown_t *sreq;
    struct proto *proto;

    if (nread < 0) {
        free (buf->base);
        sreq = malloc (sizeof (*sreq));
        if (uv_is_writable (handle)) {
            uv_shutdown (sreq, handle, NULL);
        }

        return;
    }

    if (nread == 0) {
        free (buf->base);
    }

    printf ("recv %c:%c proto %d len %d\n", hdr->source, hdr->destination,
            hdr->protocol, hdr->length);

    list_for_each (proto, &proto_list, proto_list) {
        if (proto->proto_id == hdr->protocol) {
            break;
        }
    }

    if (!list_empty (&proto->handlers)) {
        hnd = list_first_entry (&proto->handlers, struct proto_handler, hnd_list);
        hnd_strongest = hnd;

        list_for_each (hnd, &hnd->hnd_list, hnd_list) {
            if (hnd->priority > hnd_strongest->priority) {
                hnd_strongest = hnd;
            }
        }
    }

    hnd_strongest->handler (handle, buf);
}

static void
alloc (uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = malloc (suggested_size);
    buf->len = suggested_size;
}

static void
on_connection (uv_stream_t *server, int status)
{
    uv_stream_t *stream;
    int r;

    if (status != 0) {
        fprintf (stderr, "Connect error %s\n", uv_err_name (status));
    }

    stream = malloc (sizeof (uv_tcp_t));
    r = uv_tcp_init (uv_default_loop (), (uv_tcp_t *) stream);

    r = uv_accept (server, stream);
    r = uv_read_start (stream, alloc, after_read);
}

int
net_init ()
{
    struct sockaddr_in addr;
    int r;

    uv_ip4_addr ("0.0.0.0", 8510, &addr);
    server = (struct uv_handle_s *) &tcp_server;

    r = uv_tcp_init (uv_default_loop (), &tcp_server);
    if (r) {
        return 1;
    }

    r = uv_tcp_bind (&tcp_server, (const struct sockaddr *) &addr, 0);
    if (r) {
        return 1;
    }

    r = uv_listen ((uv_stream_t *) &tcp_server, SOMAXCONN, on_connection);
    if (r) {
        return 1;
    }

    return 0;
}

static void
protohandler_add (int protocol, int priority,
                  int (*handler) (uv_stream_t *, const uv_buf_t *))
{
    struct proto_handler *hand;
    struct proto *proto, *found;

    found = NULL;

    list_for_each (proto, &proto_list, proto_list) {
        if (proto->proto_id == protocol) {
            found = proto;
            break;
        }
    }

    if (!found) {
        found = malloc (sizeof (*found));
        found->proto_id = protocol;
        list_init (&found->proto_list);
        list_init (&found->handlers);

        list_insert (&proto_list, &found->proto_list);
    }

    hand = malloc (sizeof (*hand));
    list_init (&hand->hnd_list);
    hand->protocol = protocol;
    hand->handler = handler;
    hand->priority = priority;

    list_insert (&found->handlers, &hand->hnd_list);
}

int
handle_netmp (uv_stream_t *stream, const uv_buf_t *buf)
{
    printf ("netmp!\n");
    return 0;
}

static char doc[] = "DSNET Network Multiplexer";
static char args_doc[] = "";
static struct argp_option options[] = {
    {"interactive", 'i', 0, 0, "Interactive mode"},
    {"port", 'p', "PORT", 0, "Set listen port"},
    {"help", 'h', 0, 0, "Give this help list", -1},
    {"usage", 0x123, 0, 0, "Give a short usage message", -1},
    {0}};

struct arguments {
    int interactive, port;
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;
    switch (key) {
    case 'i':
        arguments->interactive = 1;
        break;
    case 'p':
        arguments->port = atoi (arg);
        break;
    case 'h':
        argp_state_help (state, state->out_stream, ARGP_HELP_STD_HELP);
        break;
    case 0x123:
        argp_state_help (state, state->out_stream,
                         ARGP_HELP_USAGE | ARGP_HELP_EXIT_OK);
        break;
    case ARGP_KEY_ARG:
        if (state->arg_num >= 0)
            argp_usage (state);
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int
main (int argc, char *argv[])
{
    struct arguments arg;

    ds_program_name = ds_basename (argv[0]);

    arg.interactive = 0;
    arg.port = 8510;

    argp_parse (&argp, argc, argv, ARGP_NO_HELP, 0, &arg);

    protohandler_add (PROTO_NETMP, 128, handle_netmp);
    if (arg.interactive) {
        ds_cmd_init ();
    } else {
        ds_fork ();
    }

    net_init ();
    uv_run (uv_default_loop (), UV_RUN_DEFAULT);

    if (arg.interactive) {
        ds_cmd_deinit ();
    }

    return 0;
}
