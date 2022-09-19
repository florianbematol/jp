#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "jp.h"
#include "arguments.h"

int main(int argc, char *const *argv)
{
  // get arguments
  jp_args *args = get_args(argc, argv);

  // check for any args not related to json process
  if (args->help) {
    usage();
    return 0;
  }

  if (args->version) {
    version();
    return 0;
  }

  if (args->filename == 0x0) {

  }

  // init jp
  jp_ctx *ctx = init_ctx(args);

  // parse file
  jp_node *node = parse_nodes(ctx);

  // print node tree
  print_node(ctx, node, 0);
  printf("\n");
  return 0;
}