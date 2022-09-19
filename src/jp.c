#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "jp.h"

jp_ctx *init_ctx(jp_args *args) {
    jp_ctx *ctx = calloc(1, sizeof(jp_ctx));
    ctx->args = args;

    FILE *fptr = fopen(ctx->args->filename, "r");
    if (fptr == NULL) {
        printf("Cannot open file %s", ctx->args->filename);
        exit(1);
    }

    fseek(fptr, 0, SEEK_END);
    long fsize = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    ctx->data = calloc(fsize + 1, sizeof(char));
    fread(ctx->data, fsize, 1, fptr);
    fclose(fptr);
    
    return ctx;
}

jp_node *parse_nodes(jp_ctx *jp_ctx) {
    // create first node
    jp_node *node = calloc(1, sizeof(jp_node));
    node->head = node;
    node->prev = NULL;
    node->next = NULL;
    node->type = NODE_NONE;
    node->key = NULL;
    node->value = NULL;

    for (;;) {

        // skip spaces
        if (*jp_ctx->data == ' ') {
            jp_ctx->data++;
            continue;
        }

        // skip new line
        if (*jp_ctx->data == '\n') {
            jp_ctx->data++;
            continue;
        }

        // next node
        if (*jp_ctx->data == ',') {
            jp_ctx->data++;

            jp_node *next_node = calloc(1, sizeof(jp_node));
            next_node->head = node->head;
            next_node->prev = node;
            next_node->next = NULL;
            next_node->type = NODE_NONE;
            next_node->key = NULL;
            next_node->value = NULL;

            node->next = next_node;

            node = next_node;
            continue;
        }

        // key of node
        if (*jp_ctx->data == '"' && node->key == NULL)  {
            store_string(&jp_ctx->data, &node->key);
            continue;
        }

        // compute value and type of nodes
        if (*jp_ctx->data == ':') {
            *jp_ctx->data++;
            continue;
        }

        // string node
        if (*jp_ctx->data == '"' && node->key != NULL && node->value == NULL) {
            store_string(&jp_ctx->data, (char**)&node->value);
            node->type = NODE_STRING;
            continue;
        }

        // number node
        if (*jp_ctx->data >= '0' && *jp_ctx->data <= '9' && node->key != NULL && node->value == NULL) {
            store_number(&jp_ctx->data, (char**)&node->value);
            node->type = NODE_NUMBER;
            continue;
        }

        // boolean node
        if (*jp_ctx->data == 'f' || *jp_ctx->data == 't') {
            store_boolean(&jp_ctx->data, (char**)&node->value);
            node->type = NODE_BOOLEAN;
            continue;
        }

        // array node
        if (*jp_ctx->data == '[') {
            // consume '['
            jp_ctx->data++;

            node->value = parse_nodes(jp_ctx);
            node->type = NODE_ARRAY;

            continue;
        }

        // object node
        if (*jp_ctx->data == '{') {
            // consume '{'
            jp_ctx->data++;

            node->value = parse_nodes(jp_ctx);
            node->type = NODE_OBJECT;

            continue;
        }

        if (*jp_ctx->data == '}' || *jp_ctx->data == ']') {
            // consume '}' or ']'
            jp_ctx->data++;
            break;
        }

        if (*jp_ctx->data == '\0') {
            break;
        }

        printf("Not found: '%c', '%d'\n", *jp_ctx->data, *jp_ctx->data);
        exit(0);
    }

    return node->head;
}

void store_string(char **data, char **dest) {
  // consume '"'
  (*data)++;

  size_t str_len = 0;
  while (**data != '"') { (*data)++; str_len++; }
  char *tmp = calloc(str_len + 1, sizeof(char));
  (*dest) = tmp;
  memcpy(*dest, *data - str_len, str_len);

  // consume '"'
  (*data)++;
}

void store_number(char **data, char **dest) {
  size_t str_len = 0;
  while (**data >= '0' && **data <= '9') { 
    (*data)++;
    str_len++;
  }

  // can be a float number
  if (**data == '.') {
    char next_char = *(*data) + 1;

    // parse the whole float number
    if (next_char >= '0' && next_char <= '9') {
      // skip '.'
      (*data)++;
      str_len++;

      while (**data >= '0' && **data <= '9') { 
        (*data)++;
        str_len++;
      }
    }
  }

  *dest = calloc(str_len + 1, sizeof(char));
  memcpy(*dest, *data - str_len, str_len);
}

void store_boolean(char **data, char **dest) {
  size_t str_len = 0;
  while (**data >= 'a' && **data <= 'z') {
    (*data)++;
    str_len++;
  }

  *dest = calloc(str_len + 1, sizeof(char));
  memcpy(*dest, *data - str_len, str_len);
}

void print_node(jp_ctx *ctx, jp_node *node, size_t lvl) {
    char *str;
    
    // ----- calculate string length -----

    // indentation + \0
    size_t str_len = ctx->args->indent * lvl + 1;

    // key
    if (node->key != NULL) {
        // '"<key_length>": '
        str_len += 1 + strlen(node->key) + 1 + 1 + 1;
    }

    // value
    if (node->type == NODE_OBJECT || node->type == NODE_ARRAY) {
        // '{' or '['
        str_len++;
    } else if (node->type == NODE_STRING) {
        // '"<value_length>"'
        str_len += 1 + strlen(node->value) + 1;
    } else if (node->type == NODE_NUMBER  || node->type == NODE_BOOLEAN) {
        // '<value_length>'
        str_len += strlen(node->value);
    }

    // content or next value
    if ((node->type == NODE_OBJECT || node->type == NODE_ARRAY) && ctx->args->compact == 0) {
        // '\n'
        str_len++;
    } else if (node->next) {
        if (ctx->args->compact == 1) {
            // ','
            str_len++;
        } else {
            // ',\n'
            str_len += 2;
        }
    }
    

    
    // ----- build string -----
    str = calloc(str_len, sizeof(char));

    // indentation
    for (int i = 0; i < ctx->args->indent * lvl; i++) {
        strcat(str, " ");
    }

    // key
    if (node->key != NULL) {
        strcat(str, "\"");
        strcat(str, node->key);
        strcat(str, "\"");
        strcat(str, ":");
        strcat(str, " ");
    }

    // value
    if (node->type == NODE_OBJECT || node->type == NODE_ARRAY) {
        strcat(str, node->type == NODE_OBJECT ? "{" : "[");
        if (ctx->args->compact == 0) {
            strcat(str, "\n");
        }
    } else if (node->type == NODE_STRING ) {
        strcat(str, "\"");
        strcat(str, node->value);
        strcat(str, "\"");

        if (node->next) {
            strcat(str, ",");
            if (ctx->args->compact == 0) {
                strcat(str, "\n");
            }
        }
    }
    else if (node->type == NODE_NUMBER || node->type == NODE_BOOLEAN) {
        strcat(str, node->value);

        if (node->next) {
            strcat(str, ",");
            if (ctx->args->compact == 0) {
                strcat(str, "\n");
            }
        }
    }

    printf("%s", str);

    // loop on children
    if (node->type == NODE_OBJECT || node->type == NODE_ARRAY) {
        jp_node *current = (jp_node*)node->value;
        while (current != NULL) {
            print_node(ctx, current, lvl + 1);
            current = current->next;
        }
        if (ctx->args->compact == 0) {
            strcat(str, "\n");
        }
        for (int i = 0; i < ctx->args->indent * lvl; i++) {
            printf("%s", " ");
        }
        printf(node->type == NODE_OBJECT ? "}" : "]");
        if (node->next != NULL) {
            printf(",");
            if (ctx->args->compact == 0) {
                strcat(str, "\n");
            }
        }
    }
}
