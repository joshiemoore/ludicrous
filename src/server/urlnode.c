//
// urlnode.c
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

#include "urlnode.h"

#include <stdlib.h>


URLNode* url_create_node(char* endpoint, PyObject* callback)
{
    URLNode* node = malloc(sizeof(URLNode));

    if (!node)
    {
        return NULL;
    }

    node->endpoint = endpoint;
    node->callback = callback;
    node->children = NULL;
    node->children_count = 0;
    node->children_size = 0;

    return node;
}

int url_add_child(URLNode* node, URLNode* new_child)
{
    if (!node->children)
    {
        // initialize children list with a size of 2
        node->children = (URLNode**)malloc(2 * sizeof(URLNode*));
        if (!node->children)
        {
            return 0;
        }
        node->children_size = 2;
    }

    // add new child to children list
    node->children[node->children_count] = new_child;
    node->children_count++;

    // resize children list as necessary
    if (node->children_count >= node->children_size)
    {
        node->children = (URLNode**)realloc(
            node->children,
            node->children_count * 2
        );

        if (!node->children)
        {
            return 0;
        }
    }

    return 1;
}

URLNode* url_find_child(URLNode* node, char* endpoint)
{
    for (int i = 0; i < node->children_count; i++)
    {
        URLNode* child = node->children[i];
        if (child && child->endpoint && !strcmp(endpoint, child->endpoint))
        {
            return child;
        }
    }

    return NULL;
}

int url_insert(URLNode* root, const char* url, PyObject* callback)
{
    // check whether we are just inserting a callback for the root node
    if (!strcmp(url, "/"))
    {
        root->callback = callback;
        return 1;
    }

    const int url_len = strlen(url);
    char* url_copy = (char*)malloc(url_len + 1);
    snprintf(url_copy, url_len + 1, "%s", url);

    // iterate through elements in the URL, adding children
    // to the URL tree as necessary
    char* tok = strtok(url_copy, "/");
    while (tok)
    {
        URLNode* child = url_find_child(root, tok);
        if (!child)
        {
            // this URL element was not found, so create it
            int tok_len = strlen(tok);
            char* tok_copy = (char*)malloc(tok_len + 1);
            snprintf(tok_copy, tok_len + 1, "%s", tok);

            child = url_create_node(tok_copy, NULL);
            if (!child)
            {
                return 0;
            }
            // add the new child node to the current root
            if (!url_add_child(root, child))
            {
                return 0;
            }
        }
        root = child;
        tok = strtok(0, "/");
    }

    root->callback = callback;
    free(url_copy);
    return 1;
}

URLNode* url_find(URLNode* root, struct mg_str url)
{
    // check whether we are just accessing the root node
    if (!strcmp(url.ptr, "/"))
    {
        return root;
    }

    char* url_copy = (char*)malloc(url.len + 1);
    snprintf(url_copy, url.len + 1, "%s", url.ptr);

    // iterate through elements in the URL, searching
    // for children
    char* tok = strtok(url_copy, "/");
    while (tok)
    {
        root = url_find_child(root, tok);
        if (!root)
        {
            return NULL;
        }

        tok = strtok(0, "/");
    }

    return root;
}
