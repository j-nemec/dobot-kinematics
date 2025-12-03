#include <stdio.h>
#include "kinematics.h"
#include "d_terminal.h"
#include "d_draw_robot.h"
#include "d_io.h"
#include "ui.h"
#include "config.h"
#include <string.h>

int main() {
    JointsDeg joints;
    TCP_Position position;
    int volba = -1;
    FILE *fr;

    d_ui_init();
    while(volba != 0) {
        d_menu();
        printf("\nVolba: ");
        scanf("%d", &volba);
        switch(volba) {
            case 1:
                t_clrscr();
                d_draw_title_bar("Přímá kinematika - J1, J2, J3 -> TCP");
                t_textcolor(B_YELLOW);
                printf("\nZadej úhly kloubů: J1, J2, J3: ");
                t_textcolor(B_GREEN);
                scanf("%lf, %lf, %lf", &joints.J1_deg, &joints.J2_deg, &joints.J3_deg);
                t_textcolor(B_YELLOW);
                switch(KForward(&joints, &position))  {
                    case K_SUCCESS:
                        t_gotoxy(1, 5);
                        printf("Souřadnice TCP - \x1b[32mOK\x1b[0m\n\n");
                        printf("\tx = %.2f\n", position.x + EFFECTOR_OFFSET_X);
                        printf("\ty = %.2f\n", position.y);
                        printf("\tz = %.2f\n", position.z);
                        break;
                    case K_ERR_INVALID_ANGLES:
                        t_textcolor(RED);
                        t_gotoxy((SCREEN_W/2) - 7, SCREEN_H/2);
                        printf("Neplatne uhly!\n");
                        break;
                    case K_ERR_UNREACHABLE:
                        t_gotoxy((SCREEN_W/2) - 9, SCREEN_H/2);
                        printf("Nedostupna pozice!\n");
                        break;
                    case K_ERR_NO_SOLUTION:
                        t_gotoxy((SCREEN_W/2) - 6, SCREEN_H/2);
                        printf("Neni reseni!\n");
                        break;
                    default:
                        t_gotoxy((SCREEN_W/2) - 8, SCREEN_H/2);
                        printf("Neznámý problem!\n");
                        break;
                }
                t_gotoxy(1, 30);
                t_textcolor(B_YELLOW);
                t_keypress_wait(CLEAN_BUFF);
                t_clrscr();
                break;
            case 2:
                t_clrscr();
                d_draw_title_bar("Inverzní kinematika - TCP[X, Y, Z] -> J1, J2, J3");
                t_textcolor(B_YELLOW);
                printf("\nZadej souřadnice TCP (x, y, z): ");
                t_textcolor(B_GREEN);
                scanf("%lf, %lf, %lf", &position.x, &position.y, &position.z);
                t_textcolor(B_WHITE);
                switch(KInverse(&position, &joints))  {
                    case K_SUCCESS:
                        printf("Inverzní kinematika - \x1b[32mOK\x1b[0m\n"); // \x1b[32mToto je zelený text\x1b[0m\n
                        printf("\tJ1 = %.2f\n", joints.J1_deg);
                        printf("\tJ2 = %.2f\n", joints.J2_deg);
                        printf("\tJ3 = %.2f\n", joints.J3_deg);
                        break;
                    case K_ERR_INVALID_ANGLES:
                        t_textcolor(RED);
                        t_gotoxy((SCREEN_W/2) - 7, SCREEN_H/2);
                        printf("Neplatne uhly!\n");
                        break;
                    case K_ERR_UNREACHABLE:
                        t_gotoxy((SCREEN_W/2) - 9, SCREEN_H/2);
                        printf("Nedostupna pozice!\n");
                        break;
                    case K_ERR_NO_SOLUTION:
                        t_gotoxy((SCREEN_W/2) - 6, SCREEN_H/2);
                        printf("Neni reseni!\n");
                        break;
                    default:
                        t_gotoxy((SCREEN_W/2) - 8, SCREEN_H/2);
                        printf("Neznamy problem!\n");
                        break;
                }
                t_gotoxy(1, 30);
                t_textcolor(B_YELLOW);
                t_keypress_wait(CLEAN_BUFF);
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
                t_gotoxy(1, 30);
                t_textcolor(B_YELLOW);
                t_keypress_wait(CLEAN_BUFF);
                t_clrscr();
                break;
            case 5:
                t_clrscr();
                char filename[120] = "";
                d_draw_title_bar("DOBOT MAGICIAN - Načtení pozic ze souboru");
                t_textcolor(B_YELLOW);
                t_gotoxy(1, 3);
                printf("Zadej název souboru <positions.dat>: ");
                t_textcolor(B_GREEN);
                fflush(stdin);
                t_clean_buff();
                t_get_line(filename, sizeof(filename));
                if (filename[0] == '\0') { // Test, zda byl řádek prázdný
                    snprintf(filename, 14, "positions.dat");
                }
                //printf("%s", filename);
                if((fr=fopen(filename, "r")) == NULL) { // Pokus o otevření souboru pro čtení
                    t_gotoxy((SCREEN_W/2) - 23, SCREEN_H/2);
                    t_textcolor(RED);
                    printf("Chyba otevření souboru - soubor nezle otevřít!\n");
                    t_textcolor(B_YELLOW);
                    t_gotoxy(1, 30);
                    t_keypress_wait(NO_CLEAN_BUFF);
                    t_clrscr();
                    break;
                }
                t_clrscr();
                char banner[130]; // Tohle peklo v pythonu není :D
                snprintf(banner, sizeof(banner), "DOBOT MAGICIAN - Otevřen soubor %s", filename);
                while(volba != 0) {
                    d_draw_title_bar(banner);
                    t_gotoxy(1, 3);
                    t_textcolor(B_YELLOW);
                    printf("Práce s daty ze souboru:\n\n");
                    printf("\t1 - Převod TCP[x, y, z] -> J1, J2, J3\n");
                    printf("\t2 - Převod N. řádku TCP[x, y, z] -> J1, J2, J3\n");
                    printf("\t3 - Spustit ASCII vizualizaci s krokováním\n");
                    printf("\t0 - Zpět\n");
                    t_textcolor(WHITE);
                    printf("\nVolba: ");
                    t_textcolor(B_GREEN);
                    scanf("%d", &volba);
                    t_textcolor(COLOR_DEFAULT);
                    switch(volba) {
                        case 1:
                            t_clrscr();
                            d_draw_title_bar(banner);
                            t_textcolor(B_GREEN);
                            t_gotoxy(1, 3);
                            int i=0;
                            while(d_io_read_tcp(fr, &position)!=IO_ERR_FORMAT) {
                                switch(KInverse(&position, &joints)) {
                                    case K_ERR_INVALID_ANGLES:
                                        printf("TCP[%.3f, %.3f, %.3f]\t\t->\t", position.x, position.y, position.z);
                                        t_textcolor(RED);
                                        printf("Neplatné úhly!\n");
                                        break;
                                    case K_ERR_UNREACHABLE:
                                        printf("TCP[%.3f, %.3f, %.3f]\t\t->\t", position.x, position.y, position.z);
                                        t_textcolor(RED);
                                        printf("Nedostupná pozice!\n");
                                        break;
                                    case K_ERR_NO_SOLUTION:
                                        printf("TCP[%.3f, %.3f, %.3f]\t\t->\t", position.x, position.y, position.z);
                                        t_textcolor(RED);
                                        printf("Nemá řešení\n");
                                        break;
                                    case K_SUCCESS:
                                        printf("TCP[%.3f, %.3f, %.3f]\t->\t", position.x, position.y, position.z);
                                        printf("J1=%.3f°, J2=%.3f°, J3=%.3f°\n", joints.J1_deg, joints.J2_deg, joints.J3_deg);
                                        break;
                                    default:
                                        printf("Neznamý problém!\n");
                                        break;
                                }
                                i++;
                                if (i==24) {
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
                            t_gotoxy(1, 30);
                            t_textcolor(B_YELLOW);
                            t_keypress_wait(CLEAN_BUFF);
                            t_clrscr();
                            break;
                        case 2:
                            break;
                        case 3:
                            break;
                        default:
                            break;
                    }
                }
                /* t_gotoxy(1, 30);
                t_textcolor(B_YELLOW);
                t_keypress_wait(CLEAN_BUFF);*/
                t_clrscr();
                volba = 4; // Vrácení aktuální volby 
                break;
            default:
                break;
        }
    }

    t_gotoxy(1, 30);
    t_keypress_wait(NO_CLEAN_BUFF);
   
    if((fr=fopen("positions.dat", "r")) == NULL) {
        printf("Chyba otevření souboru - soubor nezle otevřít!\n");
        return IO_ERR_OPEN;
    }

    while(d_io_read_tcp(fr, &position)!=IO_ERR_FORMAT){
        //printf("x=%.2f, y=%.2f, z=%.2f\n", position.x, position.y, position.z);
        switch(KInverse(&position, &joints))  {
            case K_SUCCESS:
                /*printf("Inverse kinematics: \x1b[32mOK\x1b[0m\n"); // \x1b[32mToto je zelený text\x1b[0m\n
                printf("\tJ1 = %.2f\n", joints.J1_deg);
                printf("\tJ2 = %.2f\n", joints.J2_deg);
                printf("\tJ3 = %.2f\n", joints.J3_deg);*/
                break;
            case K_ERR_INVALID_ANGLES:
                printf("Neplatne uhly!\n");
                break;
            case K_ERR_UNREACHABLE:
                printf("Nedostupna pozice!\n");
                break;
            case K_ERR_NO_SOLUTION:
                printf("Neni reseni!\n");
                break;
            default:
                printf("Neznamy problem!\n");
                break;
        }
    
        /*printf("\nForward kinematics test:\n");
   
        switch(KForward(&joints, &position))  {
            case K_SUCCESS:
                printf("Forward kinematics: \x1b[32mOK\x1b[0m\n");
                printf("\tx = %.2f\n", position.x + EFFECTOR_OFFSET_X);
                printf("\ty = %.2f\n", position.y);
                printf("\tz = %.2f\n", position.z);
                break;
            case K_ERR_INVALID_ANGLES:
                t_textcolor(RED);
                t_gotoxy(15, 15);
                printf("Neplatne uhly!\n");
                break;
            case K_ERR_UNREACHABLE:
                printf("Nedostupna pozice!\n");
                break;
            case K_ERR_NO_SOLUTION:
                printf("Neni reseni!\n");
                break;
            default:
                printf("Neznamy problem!\n");
                break;
        }
        */
        t_textcolor(YELLOW);
        //t_keypress_wait(CLEAN_BUFF);
        //t_clrscr();
        d_canvas_clear();
        char info[128];
        snprintf(info, sizeof(info), 
            "TCP: x=%.1f mm y=%.1f mm z=%.1f mm\nAngles: J1=%.1f° J2=%.1f° J3=%.1f°", 
            position.x, position.y, position.z, joints.J1_deg, joints.J2_deg, joints.J3_deg);
    
        d_robot_draw_from_joints_xz(10, DR_CANVAS_H - 2, &joints);
        d_robot_draw_from_joints_xy(65, DR_CANVAS_H / 2, &joints);
        //d_robot_draw_from_joints(30, DR_CANVAS_H - 2, &joints);
        t_textcolor(B_YELLOW);
        d_print_box(2, 2, DR_CANVAS_W - 4, info, B_YELLOW);

        d_canvas_render();
        t_keypress_wait(NO_CLEAN_BUFF);
        //t_clrscr();
    }
    fclose(fr);
    t_show_cursor();
    t_shutdown();
    return 0;
}
