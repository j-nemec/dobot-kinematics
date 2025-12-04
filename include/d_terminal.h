/* d_terminal.h */
#ifndef TERM_H
#define TERM_H

#define SCREEN_W 130
#define SCREEN_H 30

#define TO_UPPER(c) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - ('a' - 'A')) : (c) )

typedef enum {
    NO_CLEAN_BUFF = 0,
    CLEAN_BUFF = 1
} buffer_mode;

/* Jednoduchý výčet barev pro text (ANSI kódy) */
typedef enum {
    COLOR_DEFAULT = 0,  // reset na výchozí barvu

    BLACK   = 30,
    RED     = 31,
    GREEN   = 32,
    YELLOW  = 33,
    BLUE    = 34,
    MAGENTA = 35,
    CYAN    = 36,
    WHITE   = 37,

    /* světlé barvy (Bright) */
    B_BLACK   = 90,
    B_RED     = 91,
    B_GREEN   = 92,
    B_YELLOW  = 93,
    B_BLUE    = 94,
    B_MAGENTA = 95,
    B_CYAN    = 96,
    B_WHITE   = 97,

    /* pozadí (Background) */
    BG_BLACK   = 40,
    BG_RED     = 41,
    BG_GREEN   = 42,
    BG_YELLOW  = 43,
    BG_BLUE    = 44,
    BG_MAGENTA = 45,
    BG_CYAN    = 46,
    BG_WHITE   = 47
} t_color;

/* Inicializace terminálu (povolení ANSI na Windows) */
void t_init(void);

/* Ukončení – vrátí mód konzole a zobrazí kurzor */
void t_shutdown(void);

/* Vymazání obrazovky + kurzor na 1,1 (clrscr) */
void t_clrscr(void);

/* Posun kurzoru na daný sloupec x a řádek y (1-based) */
void t_gotoxy(int x, int y);

/* Nastavení barvy textu */
void t_textcolor(t_color color);

/* Reset barvy a atributů na výchozí */
void t_reset_color(void);

/* Schování / zobrazení kurzoru */
void t_hide_cursor(void);
void t_show_cursor(void);

/* Načtení celého řádku - umožňuje načíst i prázdný řádek - to scanf neumí */
void t_get_line(char *buffer, size_t size);

/* Čekání na stisk klávesy Enter */
void t_keypress_wait(buffer_mode);
void t_clean_buff(void);
void t_mouse_enable(void);
void t_mouse_disable(void);
#endif /* TERM_H */
