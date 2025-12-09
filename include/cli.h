#ifndef CLI_H
#define CLI_H

typedef enum {
    MODE_NONE = 0,  /* Klasika - textový interaktivní režim */
    MODE_FORWARD,   /* -f přímá automatizace */
    MODE_INVERSE    /* -i inverzní automatizace */
} AppMode;

typedef struct {
    AppMode mode;           /* žádný / přímá / inverzní kinematika */
    const char *input_file; /* z -f nebo -i */
    const char *output_file;/* z -o, default "results.dat" */
    int ascii_visual;       /* 0/1 podle -a */
    int help;
} CliOptions;

typedef enum {
    PARAM_ERROR = -1,
    PARAM_OK = 1,
    APP_DONE = 0
} AppExitCode;

void cli_options_init(CliOptions *opts);
int cli_parse(int argc, char **argv, CliOptions *opts);
void print_help(char *app_name);

#endif /* CLI_H */  