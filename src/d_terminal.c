/* 
 * =================    d_terminal.c   =================
 * Implementace funkcí pro práci s terminálem (ANSI kódy)
 * https://cs.wikipedia.org/wiki/ANSI_escape_kód
 * 
 * Autor: Josef Němec
 * Datum: 2025-07-20 
 * 
 * Popis:
 * Tento modul obsahuje funkce pro inicializaci terminálu,
 * nastavení barev textu, pohyb kurzoru, vymazání obrazovky
 * a schování/zobrazení kurzoru pomocí ANSI escape kódů.
 * 
 * Funkce t_init() povolí zpracování ANSI kódů na Windows.
 * Funkce t_shutdown() obnoví původní mód konzole.
 * Funkce t_clrscr() vymaže obrazovku a nastaví kurzor na pozici 1,1.
 * Funkce t_gotoxy() přesune kurzor na zadané souřadnice (1-based).
 * Funkce t_textcolor() nastaví barvu textu podle výčtového typu t_color.
 * Funkce t_reset_color() obnoví výchozí barvu textu.
 * Funkce t_hide_cursor() a t_show_cursor() schovají nebo zobrazí kurzor.
 * Funkce t_keypress_wait(mode) čeká na stisk klávesy Enter.    
 * 
 * 
 * Escape sekvence pro ANSI jsou univerzální a fungují na většině terminálů,
 * včetně Linuxu a macOS, takže zde není potřeba žádná speciální inicializace.
 * V rámci kompatibility jsem zavrhnul použití knihovny ncurses a conio.h, 
 * protože projekt cílí na jednoduchost a přenositelnost.
 */   

#include <stdio.h>
#include <string.h>
#include "d_terminal.h"

#ifdef _WIN32   // Test, zda program kompiluji na Windows (pro povolení ANSI kódů) 
    #include <windows.h>
    static DWORD g_origMode = 0; // uloží původní mód konzole
    static int g_modeChanged = 0; // příznak, zda byl mód změněn
#endif

void t_init(void) { // Inicializace terminálu (povolení ANSI na Windows)
    /* Tato funkce byla vytvořena s pomocí AI - codito ergo sum... I code therefore I'am the vibecoder... :D*/
    #ifdef _WIN32
        HANDLE hOut; // handle na standardní výstup
        DWORD mode; // mód konzole

        hOut = GetStdHandle(STD_OUTPUT_HANDLE); // získat handle na standardní výstup
        if (hOut == INVALID_HANDLE_VALUE) { // ověření platnosti handle
            return;
        }

        if (!GetConsoleMode(hOut, &mode)) { // získat aktuální mód konzole
            return;
        }

        g_origMode = mode; // uložit původní mód

        /* Povolit ANSI escape sekvence (Virtual Terminal Processing) */
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; // povolit VT zpracování
        if (SetConsoleMode(hOut, mode)) { // nastavit nový mód
            g_modeChanged = 1; // mód byl změněn
        }
    #endif
    /* Alternativní screen - vypnutí scrollbacku */
    printf("\033[?1049h");
    printf("\033[?1007h");
    /* Vypnutí myši nechci scroll a vstupy do scanf() */
    printf("\033[?9h");      // X10 mouse
    
    fflush(stdout);
}

void t_shutdown(void) { // Ukončení – vrátí mód konzole a zobrazí kurzor
    t_show_cursor();
    t_reset_color();

    #ifdef _WIN32  // Windows specifické - vytvořeno s pomocí AI
        if (g_modeChanged) {
            HANDLE hOut;

            hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut != INVALID_HANDLE_VALUE) {
                SetConsoleMode(hOut, g_origMode);
            }
            g_modeChanged = 0;
        }
    #endif
    printf("\033[?1049l");   // ukončení alternate screen
    fflush(stdout);
}

/* clrscr */
void t_clrscr(void) {
    /* ESC[2J – clear screen, ESC[H – kurzor na 1;1 */
    printf("\x1b[2J\x1b[H");
    fflush(stdout); // okamžitý výstup fflush - vyprázdnění bufferu - bez toho se někdy neprojeví hned! 
    // Některé terminály to mají bufferované.  
}

/* gotoxy – pozice (x,y), 1-based */
void t_gotoxy(int x, int y) {
    if (x < 1) x = 1;
    if (y < 1) y = 1;
    printf("\x1b[%d;%dH", y, x);
    fflush(stdout); // Okamžitý flusanec - vyprázdnění bufferu
}

/* textcolor */
void t_textcolor(t_color color) { // nastavení barvy textu  
    if (color == COLOR_DEFAULT) {
        printf("\x1b[0m");
    } else {
        printf("\x1b[%dm", (int)color);
    }
    fflush(stdout);
}

/* reset barvy a atributů */
void t_reset_color(void) {
    printf("\x1b[0m");
    fflush(stdout);
}

void t_hide_cursor(void) { // schování kurzoru
    printf("\x1b[?25l"); 
    fflush(stdout);
}

void t_show_cursor(void) {
    printf("\x1b[?25h");
    fflush(stdout);
}

void t_get_line(char *buffer, size_t size) { 
    /* 
        Používám size_t, protože je to standardní typ pro velikosti polí a vrací ho sizeof.
        Funkci to dělá univerzální a bezpečnou i pro větší buffery, než by zvládlo int.
    */
    t_textcolor(B_GREEN);
    if (fgets(buffer, size, stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';    // odstranění znaku nového řádku
    }
    t_textcolor(COLOR_DEFAULT);
}

void t_keypress_wait(buffer_mode mode) { // čekání na stisk klávesy Enter
    t_gotoxy(1, 30);
    t_textcolor(B_YELLOW);
    if(mode == CLEAN_BUFF) { 
        t_clean_buff();
    }
    printf("Press Enter..."); 
    fflush(stdout);
    while (getchar() != '\n');
}

void t_mouse_enable(void) {
    //printf("\033[?9h");      // X10 mouse
    printf("\033[?1000h");   // normal tracking
    printf("\033[?1002h");   // button tracking
    printf("\033[?1006h");   // SGR extended mouse*/
}

void t_mouse_disable(void) {
    printf("\033[?1000l");
    printf("\033[?1002l");
    printf("\033[?1006l");
}

void t_clean_buff(void) {
    /*  
     * Vyprázdnění bufferu po scanf() - není-li provedeno, může dojít k postupnému 
     * vypadávání vlasů uživatele při opakovaném volání této funkce
     * protože scanf() nechává Enter v bufferu 
     */ 
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int t_get_choice(int y){ /* Není potřeba chybová kódy - tato funkce slouží jen k načtení čísla volby tzv. ošetření vstupu...  */
    int number = 0;
    while(scanf("%d", &number)!=1) {
        t_clean_buff(); // V bufferu je sajrajt v podobě znaků - "Cože? Na sajrajt se musí se smetákem. To říká moje máma!"
        t_gotoxy(0, y);
        t_textcolor(B_RED);
        printf("Nebylo zadáno číslo!");
        t_textcolor(WHITE);
        for(int n = 0; n<130; n++) {
            t_gotoxy(n, y+1);
            printf(" ");
        }
        t_gotoxy(0, y+1);printf("Volba: ");
        t_textcolor(B_GREEN);
    }
    t_textcolor(COLOR_DEFAULT);
    return number;
}

