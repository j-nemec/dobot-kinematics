/*
 * Poznámka k návrhu UX:
 * Po letech mimo textové terminály jsem si znovu zvykal na omezení
 * ASCII rozhraní (proměnlivá velikost okna, scrollování, ruční vykreslování).
 * Aktuální implementace rámuje jednoduchý „framework“ pro ASCII grafiku,
 * což přináší něco navíc v komplexitě, ale dovoluje plnou kontrolu nad výstupem.
 *
 * Jazyk C je mocný, ale práce s ukazateli a pamětí vyžaduje opatrnost – i malá
 * nepozornost se může projevit v nepředvídaném chování aplikace.
 * 
 * Interní poznámka autora:
 * Další pointer otevře bránu do podsvětí – staré paměťové bloky povstanou a ovládnou
 * svět OFFSETY a trigonometrií. :)
 * 
 * Varování: 
 * Inkrementace ukazatele v této oblasti kódu může vyvolat „Rituál probuzení paměťových stínů“.
 * ----- Segmentation fault hluboko v pekle. -----
 */

#include <stdio.h>
#include <string.h>
#include "kinematics.h"
#include "d_terminal.h"
#include "d_draw_robot.h"
#include "d_io.h"
#include "ui.h"
#include "cli.h"
#include "config.h"

int main(int argc, char *argv[]) {
    CliOptions opts;
    JointsDeg joints;
    TCP_Position position;

    FILE *fr, *fw;

    int volba = -1;
    char c = '\0';
    char banner[130]; // Tohle peklo v pythonu není :D

    
    if (!cli_parse(argc, argv, &opts)) {
        print_help();
        return -1;
    }
   
    if (opts.mode != MODE_NONE) {
        if (opts.mode == MODE_FORWARD) {
            printf("Přímá\n");
            printf("file: %s\n", opts.input_file);
            printf("output: %s\n", opts.output_file);
        } else { /* MODE_INVERSE */
            printf("Inverzní\n");
            printf("file: %s\n", opts.input_file);
            printf("output: %s\n", opts.output_file);
        }
        return 0;
    } 

    d_ui_init();
    
    while(volba != 0) {
        d_menu(); // Hlavní nabídka 
        printf("\nVolba: ");
        t_textcolor(B_GREEN);
        volba = t_get_choice(10);
        switch(volba) {
            case 1:
                while(volba!=0) {
                    t_clrscr();
                    d_menu_forward(); // Zobrazení nabídky pro přímou kinematiku
                    printf("\nVolba: ");
                    t_textcolor(B_GREEN);
                    volba = t_get_choice(9);
                    t_textcolor(B_YELLOW);
                    switch(volba) {
                        case 1:
                            t_clrscr();
                            d_draw_title_bar("Zadání hodnot J1, J2, J3 a převod na TCP[x, y, z]");
                            printf("\nZadej úhly kloubů: J1, J2, J3: ");
                            t_textcolor(B_GREEN);
                            while(scanf("%lf, %lf, %lf", &joints.J1_deg, &joints.J2_deg, &joints.J3_deg)!=3) {
                                t_clean_buff();
                                t_clrscr();
                                d_draw_title_bar("Zadání hodnot J1, J2, J3 a převod na TCP[x, y, z]");
                                t_textcolor(RED);
                                printf("Špatně zadané hodnoty kloubů - opakuji zadávání!");
                                t_textcolor(WHITE);
                                printf("\nZadej úhly kloubů: J1, J2, J3: ");
                                t_textcolor(B_GREEN);
                            };
                            t_textcolor(B_YELLOW);
                            switch(KForward(&joints, &position))  {
                                case K_SUCCESS:
                                    t_gotoxy(1, 5);
                                    printf("Souřadnice TCP - \x1b[32mOK\x1b[0m\n\n");
                                    printf("\tx = %.2f\n", position.x);
                                    printf("\ty = %.2f\n", position.y);
                                    printf("\tz = %.2f\n", position.z);
                                    break;
                                case K_ERR_INVALID_ANGLES:
                                    t_textcolor(RED);
                                    t_gotoxy((SCREEN_W/2) - 7, SCREEN_H/2);
                                    printf("Neplatne uhly!\n");
                                    t_textcolor(B_GREEN);
                                    break;
                                case K_ERR_UNREACHABLE:
                                    t_textcolor(RED);
                                    t_gotoxy((SCREEN_W/2) - 9, SCREEN_H/2);
                                    printf("Nedostupna pozice!\n");
                                    t_textcolor(B_GREEN);
                                    break;
                                case K_ERR_NO_SOLUTION:
                                    t_textcolor(RED);
                                    t_gotoxy((SCREEN_W/2) - 6, SCREEN_H/2);
                                    printf("Neni reseni!\n");
                                    t_textcolor(B_GREEN);
                                    break;
                                default:
                                    t_textcolor(RED);
                                    t_gotoxy((SCREEN_W/2) - 8, SCREEN_H/2);
                                    printf("Neznámý problem!\n");
                                    t_textcolor(B_GREEN);
                                    break;
                            }
                            t_clean_buff();
                            printf("\nUložit do souboru(zpět)? (A/N): ");
                            
                            c = getchar();
                            switch(TO_UPPER(c)) {
                                case 'A':
                                    if(d_io_save_tcp(F_POSITIONS, &position)==IO_ERR_OPEN) {
                                        t_gotoxy((SCREEN_W/2) - 6, SCREEN_H/2);
                                        t_textcolor(RED);
                                        printf("Soubor neexistuje nebo nelze otevřít pro zápis!\n");
                                        t_textcolor(B_GREEN);
                                        break;
                                    }
                                    t_keypress_wait(CLEAN_BUFF);
                                case 'N':
                                    break;
                                default:
                                    break;
                            }
                            t_clrscr();
                            break;
                        case 2:
                            t_clrscr();
                            snprintf(banner, sizeof(banner), "Převod hodnot J1, J2, J3 -> TCP[x, y, z] ze souboru %s do souboru %s", F_JOINTS, RESULTS_POSITIONS);
                            d_draw_title_bar(banner);
                            t_gotoxy(1, 3);
                            t_textcolor(B_GREEN);
                            if((fr=fopen(F_JOINTS, "r")) == NULL) { // Pokus o otevření souboru pro čtení
                                t_gotoxy((SCREEN_W/2) - 23, SCREEN_H/2);
                                t_textcolor(RED);
                                printf("Chyba otevření souboru - soubor nezle otevřít!\n");
                                t_textcolor(B_YELLOW);
                                t_gotoxy(1, 30);
                                t_keypress_wait(NO_CLEAN_BUFF);
                                t_clrscr();
                                break;
                            }
                            if((fw = fopen(RESULTS_POSITIONS, "w")) == NULL) { // Pokus o otevření souboru pro zápis
                                    t_gotoxy((SCREEN_W/2) - 23, SCREEN_H/2);
                                    t_textcolor(RED);
                                    printf("Chyba otevření souboru - soubor nezle otevřít pro zápis!\n");
                                    t_textcolor(B_YELLOW);
                                    t_gotoxy(1, 30);
                                    t_keypress_wait(CLEAN_BUFF);
                                    t_clrscr();
                                    break;
                            }
                            int i=2;
                            while(d_io_read_joints(fr, &joints)!=IO_ERR_FORMAT) {
                                switch(KForward(&joints, &position)) {
                                    case K_ERR_INVALID_ANGLES:
                                        printf("\tJ1=%.3f°, J2=%.3f°, J3=%.3f°\t->\t", joints.J1_deg, joints.J2_deg, joints.J3_deg);
                                        t_textcolor(RED);
                                        printf("Neplatné úhly!\n");
                                        t_textcolor(B_GREEN);
                                        break;
                                    case K_ERR_UNREACHABLE:
                                        printf("\tJ1=%.3f°, J2=%.3f°, J3=%.3f°\t->\t", joints.J1_deg, joints.J2_deg, joints.J3_deg);
                                        t_textcolor(RED);
                                        printf("Nedostupná pozice!\n");
                                        t_textcolor(B_GREEN);
                                        break;
                                    case K_ERR_NO_SOLUTION:
                                        printf("\tJ1=%.3f°, J2=%.3f°, J3=%.3f°\t->\t", joints.J1_deg, joints.J2_deg, joints.J3_deg);
                                        t_textcolor(RED);
                                        printf("Nemá řešení\n");
                                        t_textcolor(B_GREEN);
                                        break;
                                    case K_SUCCESS:
                                        printf("\tJ1=%7.3f°, J2=%7.3f°, J3=%7.3f°\t->\t", joints.J1_deg, joints.J2_deg, joints.J3_deg);
                                        printf("TCP[%7.3f, %7.3f, %7.3f]\n", position.x, position.y, position.z);
                                        fprintf(fw, "%.3f %.3f %.3f\n", position.x, position.y, position.z);
                                        break;
                                    default:
                                        t_textcolor(RED);
                                        printf("->\tNeznamý problém!\n");
                                        t_textcolor(B_GREEN);
                                        break;
                                }
                                i++;
                                if (i==22) { // omezení počtu vypsaných řádků soboru 
                                    i = 0;
                                    t_gotoxy(1, 30);
                                    t_textcolor(B_YELLOW);
                                    t_keypress_wait(CLEAN_BUFF);
                                    t_clrscr(); 
                                    d_draw_title_bar(banner);
                                    t_textcolor(B_GREEN);
                                    t_gotoxy(1, 3);
                                }
                            }
                            fclose(fr);
                            fclose(fw);
                            
                            t_gotoxy(1, 30);
                            t_textcolor(B_YELLOW);
                            t_keypress_wait(CLEAN_BUFF);
                            t_clrscr();
                            break;

                        case 3:
                            t_clrscr();
                            t_clean_buff();
                            snprintf(banner, sizeof(banner), "DOBOT MAGICIAN - ASCII vizualizace s krokováním ze souboru %s", F_JOINTS);
                            d_draw_title_bar(banner);
                            t_textcolor(B_YELLOW);
                            if((fr=fopen(F_JOINTS, "r")) == NULL) { // Pokus o otevření souboru pro čtení
                                t_gotoxy((SCREEN_W/2) - 23, SCREEN_H/2);
                                t_textcolor(RED);
                                printf("Chyba otevření souboru - soubor nezle otevřít!\n");
                                t_textcolor(B_YELLOW);
                                t_gotoxy(1, 30);
                                t_keypress_wait(NO_CLEAN_BUFF);
                                t_clrscr();
                                break;
                            }
                            while(d_io_read_joints(fr, &joints)!=IO_ERR_FORMAT) {
                                if(KForward(&joints, &position)==K_SUCCESS) {
                                    t_clrscr();
                                    d_draw_title_bar(banner);
                                    t_textcolor(B_YELLOW);
                                    printf("\nTCP: x = %.3f mm, y = %.3f mm, z = %.3f mm\n", position.x, position.y, position.z);
                                    printf("Klouby: J1 = %.3f°, J2 = %.3f°, J3 = %.3f°\n", joints.J1_deg, joints.J2_deg, joints.J3_deg);
                                    d_canvas_clear();
                                    d_print_box(6, DR_CANVAS_H - 2, 13, "Zobrazení XZ", MAGENTA);
                                    d_print_box(DR_CANVAS_W - 60, DR_CANVAS_H - 2, 13, "Zobrazení XY", MAGENTA);
                                    d_robot_draw_from_joints_xz(10, DR_CANVAS_H - 4, &joints);
                                    d_robot_draw_from_joints_xy(70, (DR_CANVAS_H / 2), &joints);
                                    d_canvas_render();
                                    t_keypress_wait(NO_CLEAN_BUFF);
                                }
                                //t_keypress_wait(NO_CLEAN_BUFF);
                            }
                            t_clrscr();
                            break;
                        case 0:
                            break;
                    }
                }
                
                volba = 1;
                t_clrscr();
                break;
            case 2:
                t_clrscr();
                while(volba!=0) {
                    d_menu_inverse(); // Zobrazení nabídky pro inverzní kinematiku
                    printf("\nVolba: ");
                    t_textcolor(B_GREEN);
                    volba = t_get_choice(9);
                    t_textcolor(B_YELLOW);
                    switch(volba) {
                        case 1:
                            t_clrscr();
                            d_draw_title_bar("Zadání pozic TCP[x, y, z] a převod na úhly J1, J2, J3");
                            printf("\nZadej pozice TCP x, y, z: ");
                            t_textcolor(B_GREEN);
                            while(scanf("%lf, %lf, %lf", &joints.J1_deg, &joints.J2_deg, &joints.J3_deg)!=3) {
                                t_clean_buff();
                                t_clrscr();
                                d_draw_title_bar("Zadání pozic TCP[x, y, z] a převod na úhly J1, J2, J3");
                                t_textcolor(RED);
                                printf("Špatně zadané hodnoty TCP - opakuji zadávání!");
                                t_textcolor(WHITE);
                                printf("\nZadej pozice TCP x, y, z: ");
                                t_textcolor(B_GREEN);
                            }
                            t_textcolor(B_YELLOW);
                            switch(KInverse(&position, &joints))  {
                                case K_SUCCESS:
                                    t_gotoxy(1, 5);
                                    printf("Inverzní kinematika - \x1b[32mOK\x1b[0m\n"); // \x1b[32mToto je zelený text\x1b[0m\n
                                    printf("\tJ1 = %.2f\n", joints.J1_deg);
                                    printf("\tJ2 = %.2f\n", joints.J2_deg);
                                    printf("\tJ3 = %.2f\n", joints.J3_deg);
                                    break;
                                case K_ERR_INVALID_ANGLES:
                                    t_textcolor(RED);
                                    t_gotoxy((SCREEN_W/2) - 7, SCREEN_H/2);
                                    printf("Neplatne uhly!\n");
                                    t_textcolor(B_GREEN);
                                    break;
                                case K_ERR_UNREACHABLE:
                                    t_textcolor(RED);
                                    t_gotoxy((SCREEN_W/2) - 9, SCREEN_H/2);
                                    printf("Nedostupna pozice!\n");
                                    t_textcolor(B_GREEN);
                                    break;
                                case K_ERR_NO_SOLUTION:
                                    t_textcolor(RED);
                                    t_gotoxy((SCREEN_W/2) - 6, SCREEN_H/2);
                                    printf("Neni reseni!\n");
                                    t_textcolor(B_GREEN);
                                    break;
                                default:
                                    t_textcolor(RED);
                                    t_gotoxy((SCREEN_W/2) - 8, SCREEN_H/2);
                                    printf("Neznámý problem!\n");
                                    t_textcolor(B_GREEN);
                                    break;
                            }
                            t_clean_buff();
                            printf("\nUložit do souboru(zpět)? (A/N): ");
                            
                            c = getchar();
                            switch(TO_UPPER(c)) {
                                case 'A':
                                    if(d_io_save_joints(F_JOINTS, &joints)==IO_ERR_OPEN) {
                                        t_gotoxy((SCREEN_W/2) - 6, SCREEN_H/2);
                                        t_textcolor(RED);
                                        printf("Soubor neexistuje nebo nelze otevřít pro zápis!\n");
                                        t_textcolor(B_YELLOW);
                                        break;
                                    }
                                    t_keypress_wait(CLEAN_BUFF);
                                case 'N':
                                    break;
                                default:
                                    break;
                            }
                            t_clrscr();
                            break;
                        case 2:
                            t_clrscr();
                            snprintf(banner, sizeof(banner), "Převod pozic TCP[x, y, z] -> J1, J2, J3 ze souboru %s do souboru %s", F_POSITIONS, RESULTS_JOINTS);
                            d_draw_title_bar(banner);
                            t_gotoxy(1, 3);
                            t_textcolor(B_GREEN);
                            if((fr=fopen(F_POSITIONS, "r")) == NULL) { // Pokus o otevření souboru pro čtení
                                t_gotoxy((SCREEN_W/2) - 23, SCREEN_H/2);
                                t_textcolor(RED);
                                printf("Chyba otevření souboru - soubor nezle otevřít!\n");
                                t_textcolor(B_YELLOW);
                                t_gotoxy(1, 30);
                                t_keypress_wait(NO_CLEAN_BUFF);
                                t_clrscr();
                                break;
                            }
                            if((fw = fopen(RESULTS_JOINTS, "w")) == NULL) { // Pokus o otevření souboru pro zápis
                                    t_gotoxy((SCREEN_W/2) - 23, SCREEN_H/2);
                                    t_textcolor(RED);
                                    printf("Chyba otevření souboru - soubor nezle otevřít pro zápis!\n");
                                    t_textcolor(B_YELLOW);
                                    t_gotoxy(1, 30);
                                    t_keypress_wait(CLEAN_BUFF);
                                    t_clrscr();
                                    break;
                            }
                            int i=2;
                            while(d_io_read_tcp(fr, &position)!=IO_ERR_FORMAT) {
                                switch(KInverse(&position, &joints)) {
                                    case K_ERR_INVALID_ANGLES:
                                        printf("\tTCP[%7.3f, %7.3f, %7.3f]\t->\t", position.x, position.y, position.z);
                                        t_textcolor(RED);
                                        printf("Neplatné úhly!\n");
                                        t_textcolor(B_GREEN);
                                        break;
                                    case K_ERR_UNREACHABLE:
                                        printf("\tTCP[%7.3f, %7.3f, %7.3f]\t->\t", position.x, position.y, position.z);
                                        t_textcolor(RED);
                                        printf("Nedostupná pozice!\n");
                                        t_textcolor(B_GREEN);
                                        break;
                                    case K_ERR_NO_SOLUTION:
                                        printf("\tTCP[%7.3f, %7.3f, %7.3f]\t->\t", position.x, position.y, position.z);
                                        t_textcolor(RED);
                                        printf("Nemá řešení\n");
                                        t_textcolor(B_GREEN);
                                        break;
                                    case K_SUCCESS:
                                        printf("\tTCP[%7.3f, %7.3f, %7.3f]\t->\t", position.x, position.y, position.z);
                                        printf("J1 = %7.3f, J2 = %7.3f, J3 = %7.3f]\n", joints.J1_deg, joints.J2_deg, joints.J3_deg);
                                        fprintf(fw, "%.3f %.3f %.3f\n", joints.J1_deg, joints.J2_deg, joints.J3_deg);
                                        break;
                                    default:
                                        printf("->\tNeznamý problém!\n");
                                        break;
                                }
                                i++;
                                if (i==22) { // omezení počtu vypsaných řádků soboru 
                                    i = 0;
                                    t_gotoxy(1, 30);
                                    t_textcolor(B_YELLOW);
                                    t_keypress_wait(CLEAN_BUFF);
                                    t_clrscr(); 
                                    d_draw_title_bar(banner);
                                    t_textcolor(B_GREEN);
                                    t_gotoxy(1, 3);
                                }
                            }
                            fclose(fr);
                            fclose(fw);
                            
                            t_gotoxy(1, 30);
                            t_textcolor(B_YELLOW);
                            t_keypress_wait(CLEAN_BUFF);
                            t_clrscr();
                            break;
                        case 3:
                            t_clrscr();
                            t_clean_buff();
                            snprintf(banner, sizeof(banner), "DOBOT MAGICIAN - ASCII vizualizace s krokováním ze souboru %s", F_POSITIONS);
                            d_draw_title_bar(banner);
                            t_textcolor(B_YELLOW);
                            if((fr=fopen(F_POSITIONS, "r")) == NULL) { // Pokus o otevření souboru pro čtení
                                t_gotoxy((SCREEN_W/2) - 23, SCREEN_H/2);
                                t_textcolor(RED);
                                printf("Chyba otevření souboru - soubor nezle otevřít!\n");
                                t_textcolor(B_YELLOW);
                                t_gotoxy(1, 30);
                                t_keypress_wait(NO_CLEAN_BUFF);
                                t_clrscr();
                                break;
                            }
                            while(d_io_read_tcp(fr, &position)!=IO_ERR_FORMAT) {
                                if(KInverse(&position, &joints)==K_SUCCESS) {
                                    t_clrscr();
                                    d_draw_title_bar(banner);
                                    t_textcolor(B_YELLOW);
                                    printf("\nTCP: x = %.3f mm, y = %.3f mm, z = %.3f mm\n", position.x, position.y, position.z);
                                    printf("Klouby: J1 = %.3f°, J2 = %.3f°, J3 = %.3f°\n", joints.J1_deg, joints.J2_deg, joints.J3_deg);
                                    d_canvas_clear();
                                    d_print_box(6, DR_CANVAS_H - 2, 13, "Zobrazení XZ", MAGENTA);
                                    d_print_box(DR_CANVAS_W - 60, DR_CANVAS_H - 2, 13, "Zobrazení XY", MAGENTA);
                                    d_robot_draw_from_joints_xz(10, DR_CANVAS_H - 4, &joints);
                                    d_robot_draw_from_joints_xy(70, (DR_CANVAS_H / 2), &joints);
                                    d_canvas_render();
                                    t_keypress_wait(NO_CLEAN_BUFF);
                                }
                            }
                            t_clrscr();
                            break;
                        case 0:
                            break;
                    }
                }          
                volba = 2;
                t_clrscr();
                break;
            case 3:
                t_clrscr();
                d_draw_title_bar("DOBOT MAGICIAN - ASCII vizualizace");
                t_textcolor(B_YELLOW);
                printf("\nTCP: x=%.1fmm, y=%.1fmm, z=%.1fmm\n", position.x, position.y, position.z);
                printf("Angles: J1=%.1f°, J2=%.1f°, J3=%.1f°\n", joints.J1_deg, joints.J2_deg, joints.J3_deg);
                d_canvas_clear();
                d_print_box(6, DR_CANVAS_H - 2, 13, "Zobrazení XZ", MAGENTA);
                d_print_box(DR_CANVAS_W - 60, DR_CANVAS_H - 2, 13, "Zobrazení XY", MAGENTA);
                d_robot_draw_from_joints_xz(10, DR_CANVAS_H - 4, &joints);
                d_robot_draw_from_joints_xy(70, (DR_CANVAS_H / 2), &joints);
                d_canvas_render();
                t_keypress_wait(CLEAN_BUFF);
                t_clrscr();
                break;
            
            default:
                break;
        }
    }
    
    t_show_cursor();
    t_shutdown();
    return 0;
}
