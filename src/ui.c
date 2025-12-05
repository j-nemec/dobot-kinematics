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
    printf("\t3 - Zobrazení ASCII vizualizace (poslední výpočet)\n");
    printf("\t0 - Konec programu\n");
    t_reset_color();
}

void d_menu_forward(void) {
    d_draw_title_bar("DOBOT MAGICIAN - Přímá kinematika: J1, J2, J3 -> TCP[X, Y, Z]");
    t_textcolor(B_YELLOW);
    printf("\nPřímá kinematika - menu:\n\n");
    printf("\t1 - Zadání hodnot J1, J2, J3 a převod na TCP[x, y, z]\n");
    printf("\t2 - Převod hodnot J1, J2, J3 -> TCP[x, y, z] ze souboru\n");
    printf("\t3 - Spustit ASCII vizualizaci s krokováním\n");
    printf("\t0 - Zpět\n");
    t_reset_color();
}

void d_menu_inverse(void) {
    d_draw_title_bar("DOBOT MAGICIAN - Inverzní kinematika: TCP[X, Y, Z] -> J1, J2, J3");
    t_textcolor(B_YELLOW);
    printf("\nInverzní kinematika - menu:\n\n");
    printf("\t1 - Zadání pozic TCP[x, y, z] a převod na úhly J1, J2, J3\n");
    printf("\t2 - Převod pozic TCP[x, y, z] -> J1, J2, J3 ze souboru\n");
    printf("\t3 - Spustit ASCII vizualizaci s krokováním\n");
    printf("\t0 - Zpět\n");
    t_reset_color();
}
