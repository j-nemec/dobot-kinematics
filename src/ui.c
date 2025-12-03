#include <stdio.h>
#include <string.h>
#include <d_terminal.h>

void d_ui_init(void) {
    t_init();
    t_clrscr();
    t_reset_color();
    t_gotoxy(1, 1);
}

void d_draw_title_bar(char *title) {
    int title_len = strlen(title);
    t_gotoxy(1, 1);
    t_textcolor(BG_BLUE);
    t_textcolor(B_WHITE);
    for(int i=0; i<SCREEN_W; i++) {
        putchar(' ');
    }
    t_gotoxy((SCREEN_W-title_len)/2, 1);
    printf("%s\n", title);
    t_reset_color();
}

void d_menu(void) {
    d_draw_title_bar("DOBOT MAGICIAN SIMULATOR V1.0");
    t_textcolor(B_YELLOW);
    printf("\nHlavní nabídka: \n\n");
    printf("\t1 - Přímá kinematika\n");
    printf("\t2 - Inverzní kinematika\n");
    printf("\t3 - Zobrazení ASCII vizualizace\n");
    printf("\t4 - Načtení úhlů ze souboru\n");
    printf("\t5 - Načtení pozic TCP ze souboru\n");
    printf("\t6 - Uložení výpočtu inverzní kinematiky do souboru\n");
    printf("\t7 - Uložení výpočtu přímé kinematiky do souboru\n");
    printf("\t0 - Konec programu\n");
    t_reset_color();
}
