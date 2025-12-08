/*
 * ======================== cli.c =========================
 * User Interface
 * Autor: Josef Němec
 * Datum: 2025-12-6
 * 
 * Popis:
 * Základní funkce pro práci s parametry příkazové řádky.
 * - Parser parametrů
 * - Nápověda k programu
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "d_terminal.h"
#include "cli.h"

void cli_options_init(CliOptions *opts) {
    opts->mode        = MODE_NONE;
    opts->input_file  = NULL;
    opts->output_file = "results.dat";  /* Pokud nebude zvlolen parametr -o */
    opts->ascii_visual = 0;
    opts->help = 0;
}

int cli_parse(int argc, char **argv, CliOptions *opts) {
    int i;

    cli_options_init(opts);

    for (i = 1; i < argc; ++i) { // Parametr po parametru... 
        const char *arg = argv[i];

        if (strcmp(arg, "-f") == 0) {
            /* kontrola, že není současně -i */
            if (opts->mode == MODE_INVERSE) {
                fprintf(stderr, "Chyba! Parametry -f a -i nelze kombinovat.\n"); // Zkušený uživatel může chytat chybová hlášení do logu :D
                return 0;
            }
            if (i + 1 >= argc) {
                fprintf(stderr, "Chyba! Za -f chybi jmeno souboru.\n");
                return 0;
            }
            opts->mode = MODE_FORWARD;
            opts->input_file = argv[++i];  /* vezmeme další argument jako název souboru */
        } else if (strcmp(arg, "-i") == 0) {
            if (opts->mode == MODE_FORWARD) {
                fprintf(stderr, "Chyba! Parametry -f a -i nelze kombinovat.\n");
                return 0;
            }
            if (i + 1 >= argc) {
                fprintf(stderr, "Chyba! Za -i chybi jmeno souboru.\n");
                return 0;
            }
            opts->mode = MODE_INVERSE;
            opts->input_file = argv[++i];
        } else if (strcmp(arg, "-o") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Chyba! Za -o chybi jmeno souboru.\n");
                return 0;
            }
            opts->output_file = argv[++i];
        } else if (strcmp(arg, "-a") == 0) {
            opts->ascii_visual = 1;
        } else if (strcmp(arg, "-h") == 0) {
            opts->help = 1;
        }
        else {
            fprintf(stderr, "Neznamy parametr: %s\n", arg);
            return 0;
        }
    }

    /* Byl naplněn input_file? */
    if ((opts->mode == MODE_FORWARD || opts->mode == MODE_INVERSE) &&
        opts->input_file == NULL) {
        fprintf(stderr, "Chyba! Pro -f/-i musi byt zadany vstupni soubor.\n");
        return 0;
    }

    return 1; /* OK */
}

void print_help(char *app_name) {
    t_textcolor(B_CYAN);
    printf("Dobot MAGICIAN - simulátor\n");
    t_textcolor(B_YELLOW);
    printf("Použití aplikace: %s [parametry]\n", app_name);
    printf("\t-f file.dat"); t_textcolor(B_WHITE); printf("\tpřímá kinematika, načíst klouby ze souboru\n");
    t_textcolor(B_YELLOW); printf("\t-i file.dat"); t_textcolor(B_WHITE); printf("\tinverzní kinematika, načíst TCP ze souboru\n");
    t_textcolor(B_YELLOW); printf("\t-o file.dat"); t_textcolor(B_WHITE); printf("\tvýstupní soubor (default results.dat)\n");
    t_textcolor(B_YELLOW); printf("\t-a");          t_textcolor(B_WHITE); printf("\t\tASCII vizualizace (krokovani) - pouze s použitím parametru -f nebo -i\n");
    t_textcolor(B_YELLOW); printf("\t-h");          t_textcolor(B_WHITE); printf("\t\tzobratzení této nápovědy\n");
    printf("\nBez parametrů se spusti interaktivní textové menu.\n");
    printf("Příklad použití: ");
    t_textcolor(B_YELLOW);
    printf("\t%s -f joints.dat -a\n", app_name);
    t_textcolor(B_GREEN);
    printf("Vytvořil: Josef Němec\n");
    t_textcolor(COLOR_DEFAULT);
}

