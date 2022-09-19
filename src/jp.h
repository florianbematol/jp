#ifndef JP_H
#define JP_H

#include "arguments.h"

typedef enum {
  // JP types
  NODE_NONE,
  NODE_ROOT_OBJECT,
  NODE_ROOT_ARRAY,

  // Handled types
  NODE_STRING,
  NODE_NUMBER,
  NODE_BOOLEAN,
  NODE_ARRAY,
  NODE_OBJECT
} jp_type;

typedef struct jp_ctx {
    jp_args *args;
    char *data;
} jp_ctx;

typedef struct jp_node {
    // node infos
    jp_type type;
    char *key;
    void *value;

    // node list management
    struct jp_node *head;
    struct jp_node *prev;
    struct jp_node *next;
} jp_node;

jp_ctx *init_ctx(jp_args *args);

jp_node *parse_nodes(jp_ctx *jp_ctx);
void store_string(char **data, char **dest);
void store_number(char **data, char **dest);
void store_boolean(char **data, char **dest);

void print_node(jp_ctx *ctx, jp_node *node, size_t lvl);

#endif