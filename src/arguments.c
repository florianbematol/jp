#include "stdio.h"
#include "getopt.h"
#include "stdlib.h"

#include "arguments.h"

jp_args *get_args(int argc, char * const *argv) {

    jp_args *args = calloc(1, sizeof(jp_args));
    args->help = 1;
    args->version = 0;
    args->compact = 0;
    args->indent = 2;
    args->filename = 0x0;

    int option_index = 0;
    struct option long_options[] = {
        { "help", no_argument, NULL, 'h'},
        { "version", no_argument, NULL, 'v'},
        { "compact", no_argument, NULL, 'c'},
        { "indent", optional_argument, NULL, 'i'},
        { "filename", optional_argument, NULL, 'f'}
    };
    
    int opt = -2;
    while((opt = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                args->help = 1;
                break;
            case 'v':
                args->help = 0;
                args->version = 1;
                break;
            case 'c':
                args->compact = 1;
                break;
            case 'i':
                if (optarg == 0x0) {
                    args->indent = atoi(argv[optind]);                   
                } else {
                    args->indent = atoi(optarg);
                }
                break;
            case 'f':
                args->help = 0;
                args->filename = optarg;
                break;
        }
    }

    if (args->compact == 1) {
        args->indent = 0;
    }

    if (optind < argc) {
        args->help = 0;
        args->filename = argv[optind];
    }
    
    return args;
}

void usage() {
  printf("jp - command line tool to process JSON\n");
  printf("\n");
  printf("Usage: jp [options] filename\n");
  printf("\n");
  printf("jp is a tool to process JSON inputs.\n");
  printf("\n");
  printf("Example:\n");
  printf("\n");
  printf("        $ echo '{\"foo\": 0}' | jp\n");
  printf("        {\n");
  printf("            \"foo\": 0\n");
  printf("        }\n");
  printf("\n");
  printf("Options:\n");
  printf("\n");
  printf("  --help      Display help message.\n");
  printf("  --version   Display jp version.\n");
  printf("  --filename  Path to JSON file to process (Equivalent to 'filename').\n");
  printf("  --indent    Indentation of output JSON (Default: 2).\n");
  printf("  --compact   Output JSON is compacted (Cannot be used with '--indent').\n");
}

void version() {
  printf("jp-1.0.0\n");
}