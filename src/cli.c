#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cli.h>

void cli_options_init(CliOptions *opts) {
    opts->mode        = MODE_NONE;
    opts->input_file  = NULL;
    opts->output_file = "results.dat";  /* Pokud nebude zvlolen parametr -o */
    opts->ascii_visual = 0;
}

int cli_parse(int argc, char **argv, CliOptions *opts) {
    int i;

    cli_options_init(opts);

    for (i = 1; i < argc; ++i) { // Parametr po parametru... 
        const char *arg = argv[i];

        if (strcmp(arg, "-f") == 0) {
            /* kontrola, že není současně -i */
            if (opts->mode == MODE_INVERSE) {
                fprintf(stderr, "Chyba: parametry -f a -i nelze kombinovat.\n");
                return 0;
            }
            if (i + 1 >= argc) {
                fprintf(stderr, "Chyba: za -f chybi jmeno souboru.\n");
                return 0;
            }
            opts->mode = MODE_FORWARD;
            opts->input_file = argv[++i];  /* vezmeme další argument jako název souboru */
        }
        else if (strcmp(arg, "-i") == 0) {
            if (opts->mode == MODE_FORWARD) {
                fprintf(stderr, "Chyba: parametry -f a -i nelze kombinovat.\n");
                return 0;
            }
            if (i + 1 >= argc) {
                fprintf(stderr, "Chyba: za -i chybi jmeno souboru.\n");
                return 0;
            }
            opts->mode = MODE_INVERSE;
            opts->input_file = argv[++i];
        }
        else if (strcmp(arg, "-o") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Chyba: za -o chybi jmeno souboru.\n");
                return 0;
            }
            opts->output_file = argv[++i];
        }
        else if (strcmp(arg, "-a") == 0) {
            opts->ascii_visual = 1;
        }
        else {
            fprintf(stderr, "Neznamy parametr: %s\n", arg);
            return 0;
        }
    }

    /* Byl naplněn input_file? */
    if ((opts->mode == MODE_FORWARD || opts->mode == MODE_INVERSE) &&
        opts->input_file == NULL) {
        fprintf(stderr, "Chyba: pro -f/-i musi byt zadany vstupni soubor.\n");
        return 0;
    }

    return 1; /* OK */
}

void print_help(void) {
    printf( "Použití aplikace: [volby]\n");
    printf("\t-f file.dat   přímá kinematika, načíst klouby ze souboru\n");
    printf("\t-i file.dat   inverzní kinematika, načíst TCP ze souboru\n");
    printf("\t-o file.dat   výstupní soubor (default results.dat)\n");
    printf("\t-a            ASCII vizualizace (krokovani)\n");
    printf("Bez parametrů se spusti interaktivni textove menu.\n");
}

