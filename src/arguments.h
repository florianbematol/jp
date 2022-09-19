#ifndef ARGUMENTS_H
#define ARGUMENTS_H

typedef struct jp_args {
    int help;
    int version;
    int compact;
    int indent;
    char *filename;
} jp_args;

jp_args *get_args(int argc, char *const *argv);

void usage();
void version();

#endif