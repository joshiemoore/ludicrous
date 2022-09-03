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
#include "urlnode.h"
#include "mongoose.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

char  SERVER_HOST[256];
long  SERVER_PORT = 8000;
char  BIND_ADDR[300];


// the base of the URL tree (the root endpoint)
// this gets returned when a request is made to
// "/", and is also the parent of all other URL nodes
static URLNode* ROOT_NODE;

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
        struct mg_str url = hm->uri;
        PyObject* endpoint_callback = NULL;

        URLNode* endpoint = url_find(ROOT_NODE, url);
        if (endpoint)
        {
            endpoint_callback = endpoint->callback;
        }

        if (endpoint_callback)
        {
            // prepare request metadata dict
            PyObject* request_dict = PyDict_New();
            if (!request_dict)
            {
                mg_http_reply(c, 500, "", "");
                return;
            }

            // request method
            PyObject* method_val = NULL;
            if (hm->method.len > 0)
            {
                method_val = PyUnicode_FromStringAndSize(
                    hm->method.ptr,
                    hm->method.len
                );
                PyDict_SetItemString(request_dict, "METHOD", method_val);
            }

            // get POST data from request
            PyObject* post_obj = NULL;
            if (hm->body.len > 0)
            {
                post_obj = json_decode(hm->body.ptr, hm->body.len);
            }

            if (post_obj)
            {
                PyDict_SetItemString(request_dict, "POST", post_obj);
            }
            else
            {
                Py_INCREF(Py_None);
                PyDict_SetItemString(request_dict, "POST", Py_None);
            }

            // get headers from request
            PyObject* header_dict = PyDict_New();
            if (!header_dict)
            {
                mg_http_reply(c, 500, "", "");
                return;
            }
            for (int i = 0; i < MG_MAX_HTTP_HEADERS; i++)
            {
                struct mg_http_header header = hm->headers[i];
                if (!header.name.ptr || !header.value.ptr) break;
                PyObject* header_key = PyUnicode_FromStringAndSize(header.name.ptr, header.name.len);
                PyObject* header_val = PyUnicode_FromStringAndSize(header.value.ptr, header.value.len);
                PyDict_SetItem(header_dict, header_key, header_val);
                Py_DECREF(header_val);
                Py_DECREF(header_key);
            }
            PyDict_SetItemString(request_dict, "HEADERS", header_dict);

            // call route handler, then send encoded JSON back to client
            PyObject* response = PyObject_CallOneArg(
                endpoint_callback,
                request_dict
            );
            if (response && response != Py_None)
            {
                char* response_text = json_encode(response);
                mg_http_reply(
                    c,
                    200,
                    "Content-Type: application/json\r\n",
                    "%s",
                    response_text
                );
                free(response_text);
            }
            else
            {
                // TODO what to do if the handler does not return a response?
                mg_http_reply(c, 501, "", "");
            }

            Py_DECREF(response);
            Py_DECREF(header_dict);
            Py_XDECREF(method_val);
            Py_DECREF(request_dict);
        }
        else
        {
            // endpoint not found
            mg_http_reply(c, 404, "", "");
        }
    }
}

PyObject* ludicrous_server_run(PyObject* self, PyObject* args, PyObject* kwargs)
{
    printf("Preparing ship for ludicrous speed. . .\n\n");

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
        snprintf(SERVER_HOST, 10, "localhost");
    }

    if (arg_port)
    {
        SERVER_PORT = arg_port;
    }

    // join host and port into one string to bind to
    snprintf(BIND_ADDR, 300, "%s:%ld", SERVER_HOST, SERVER_PORT);

    // intialize URL root node
    ROOT_NODE = url_create_node(NULL, NULL);

    // set up routing based on functions in global namespace
    PyObject* globals = PyEval_GetGlobals();
    PyObject* global_key;
    PyObject* global_value;
    Py_ssize_t pos = 0;

    while (PyDict_Next(globals, &pos, &global_key, &global_value))
    {
        if (PyCallable_Check(global_value))
        {
            PyObject* url = PyObject_GetAttr(
                global_value,
                PyUnicode_FromString("_LUDICROUS")
            );
            if (url && PyUnicode_Check(url))
            {
                // add the endpoint and its request handler to the tree
                const char* url_str = PyUnicode_AsUTF8(url);
                printf("Adding route %s...", url_str);
                if (!url_insert(ROOT_NODE, url_str, global_value))
                {
                    printf(" FAILED!");
                }
                printf("\n");
            }
        }
    }

    // initialize mongoose
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);

    // bind server
    struct mg_connection *c = mg_http_listen(&mgr, BIND_ADDR, fn, NULL);
    if (c == NULL)
    {
        //\TODO set python error
        printf("\nFailed to bind on %s\n", BIND_ADDR);
        return NULL;
    }

    printf("\nServer started on %s (CTRL+C to stop)\n", BIND_ADDR);
    fflush(stdout);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // loop until interrupted, handle requests
    while(s_signo == 0) mg_mgr_poll(&mgr, 1000);

    // clean up mongoose
    mg_mgr_free(&mgr);

    // clean up the URL tree
    url_delete_node(ROOT_NODE);

    printf("\n");
    Py_RETURN_NONE;
}
