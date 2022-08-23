//
// server.c
//
// Copyright (c) 2022 Joshua Moore
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License Version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#include "json.h"
#include "server.h"
#include "mongoose.h"

#include <stdio.h>
#include <signal.h>
#include <stdbool.h>

char  SERVER_HOST[256];
long  SERVER_PORT = 8000;
char  BIND_STRING[300];


// Handle interrupts, like CTRL+C
static int s_signo;
static void signal_handler(int signo)
{
    s_signo = signo;
}

static void fn(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
    if (ev == MG_EV_HTTP_MSG)
    {
        struct mg_http_message *hm = (struct mg_http_message*) ev_data;

        // match URI to route
        //\TODO routing
        if (mg_http_match_uri(hm, "/"))
        {
            //char* resp = json_encode(test);
            //mg_http_reply(
            //    c,
            //    200,
            //    "Content-Type: application/json\r\n",
            //    "%s",
            //    resp
            //);
            //free(resp);
        }
        else
        {
            // no matching route found
            mg_http_reply(c, 404, "", "");
        }
    }
}

PyObject* ludicrous_server_run(PyObject* self, PyObject* args, PyObject* kwargs)
{
    printf("Preparing ship for ludicrous speed. . .\n");

    // parse arguments
    static char* keywords[] = {"host", "port", NULL};
    char* arg_host = NULL;
    long arg_port = 0;

    PyArg_ParseTupleAndKeywords(
        args,
        kwargs,
        "|sI",
        keywords,
        &arg_host,
        &arg_port
    );

    if (arg_host)
    {
        snprintf(SERVER_HOST, 256, "%s", arg_host);
    }
    else
    {
        sprintf(SERVER_HOST, "localhost");
    }

    if (arg_port)
    {
        SERVER_PORT = arg_port;
    }

    // join host and port into one string to bind to
    snprintf(BIND_STRING, 300, "%s:%ld", SERVER_HOST, SERVER_PORT);

    // initialize mongoose
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    struct mg_connection *c = mg_http_listen(&mgr, BIND_STRING, fn, NULL);

    if (c == NULL)
    {
        //\TODO set error message
        printf("Failed to bind on %s\n", BIND_STRING);
        return NULL;
    }

    printf("Server started on %s (CTRL+C to stop)\n", BIND_STRING);
    fflush(stdout);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // loop until interrupted, handle requests
    while(s_signo == 0) mg_mgr_poll(&mgr, 1000);

    Py_RETURN_NONE;
}
