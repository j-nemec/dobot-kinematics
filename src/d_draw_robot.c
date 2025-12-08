/*  
 * ========================= d_robot_canvas.c ========================= 
 *  Implementace kreslení robota na textové plátno v terminálu.
 *  Autor: Josef Němec
 *  Datum: 2025-10-05
 * 
 *  Popis:
 *  Tento modul poskytuje funkce pro kreslení jednoduchého 2D
 *  modelu robota na textové plátno v terminálu pomocí ASCII
 *  znaků a barev. Kreslení probíhá na virtuální canvas (dvourozměrné pole),
 *  který je následně vykreslen do terminálu.
 *  Kreslení robota je založeno na zadaných úhlech ramen a délce ramen.
 *  Měřítko je definováno konstantou DR_MM_PER_PIXEL.
 *  Funkce využívají Bresenhamův algoritmus pro kreslení úseček.
 *  Tento algoritmus funguje na principu inkrementálního rozhodování
 *  o tom, který pixel vyplnit, aby vznikla co nejpřesnější přímka
 *  mezi dvěma body v diskrétní mřížce. 
 *  Struktura D_RobotPoints obsahuje klíčové body robota v pixelové mřížce.
 *  Funkce umožňují výpočet bodů robota z úhlů kloubů a jejich vykreslení.
 *  Tento modul závisí na modulu d_terminal pro práci s terminálem.
 *  
 *  ASCII art je umění budoucnosti! :D      
 */

#include <stdio.h>
#include <stdlib.h>   /* abs() */
#include <string.h>
#include <math.h>

#include "config.h"       /* Ramena L1, L2 v mm, OFFSETY */
#include "math_utils.h"   /* deg2rad() */
#include "d_terminal.h"
#include "d_draw_robot.h"
#include "d_terminal.h"
#include "ui.h"
#include "d_io.h"

/* ====== Datové struktury canvasu ====== */

/* Jedna buňka canvasu: znak + barva */
typedef struct {
    char    ch;
    t_color color;
} D_Cell;

/* Vnitřní 2D pole jako pixelová mapa */
static D_Cell dr_canvas[DR_CANVAS_H][DR_CANVAS_W];

/* Převod délky v mm na pixely dle měřítka */
int d_mm_to_px(double mm) {
    return (int)lround(mm / DR_MM_PER_PIXEL); // zaokrouhlení na nejbližší pixel (původne long, stačí int, tak přetypováno)
}

/* Vymazání plátna */
void d_canvas_clear(void) { // Vyplní plátno mezerami ve stylu clrscr :D
    for (int y = 0; y < DR_CANVAS_H; ++y) {
        for (int x = 0; x < DR_CANVAS_W; ++x) {
            dr_canvas[y][x].ch    = ' ';
            dr_canvas[y][x].color = COLOR_DEFAULT;
        }
    }
}

/* Nastavení jednoho pixelu (znak + barva) */
void d_canvas_set_pixel(int x, int y, char ch, t_color color) {
    if (x < 0 || x >= DR_CANVAS_W) return;
    if (y < 0 || y >= DR_CANVAS_H) return;
    dr_canvas[y][x].ch    = ch;
    dr_canvas[y][x].color = color;
}

/* Bresenhamova úsečka v pixelové mřížce (s barvou) */
void d_canvas_draw_line(int x0, int y0, int x1, int y1, char ch, t_color color) {
    int dx = abs(x1 - x0); // rozdíl v x
    int sx = (x0 < x1) ? 1 : -1; // směrový krok v x (kladný nebo záporný) ternární operátory jsou malý zázrak :)
    int dy = -abs(y1 - y0); // záporná hodnota pro y (protože y roste dolů)
    int sy = (y0 < y1) ? 1 : -1; // směrový krok v y (kladný nebo záporný)
    int err = dx + dy; // počáteční chyba (= dx - abs(dy))

    while (1) { 
        // nekonečná smyčka, ukončená breakem - někdy to může být přehlednější než složité podmínky ve while a zároveň
        // může být opravdovým peklem, pokud zapomenete break :D

        d_canvas_set_pixel(x0, y0, ch, color); // nastavit pixel na aktuální pozici (x0, y0)
        if (x0 == x1 && y0 == y1) break; // pokud jsme dosáhli cílového bodu, ukončit smyčku
        int e2 = 2 * err; // dvojnásobek chyby
        if (e2 >= dy) { err += dy; x0 += sx; } // posun v x směru
        if (e2 <= dx) { err += dx; y0 += sy; } // posun v y směru
    }
}

/* Vypsání plátna na obrazovku */
void d_canvas_render(void) {
    t_gotoxy(1, DR_UI_LINES + 1);  // Začátek vlevo nahoře - znalci pascalu vědí proč :D

    t_color last_color = COLOR_DEFAULT;

    for (int y = 0; y < DR_CANVAS_H; ++y) {
        for (int x = 0; x < DR_CANVAS_W; ++x) {
            D_Cell cell = dr_canvas[y][x];

            if (cell.color != last_color) {
                t_textcolor(cell.color);
                last_color = cell.color;
            }

            putchar(cell.ch);
        }
        putchar('\n');
    }
    t_reset_color();
}

/* „Školní“ výpočet bodů robota v PIXELOVÉ mřížce
   - canvas: x doprava, y dolů
   - úhly: 0° = doprava, 90° = nahoru
*/

/* Máme sice knihovn kinematics.h, ale tato funkce je pro vykreslování a nezávislá na funkci KForward/KInverse, 
 * aby nebylo třeba ji přepisovat pro výpočty souřadnic kloubů robota, protože Canvas má rostoucí Y dolů.
 * Logicky - je to pole znaků a indexuje od 0,0 v levém horním rohu.
 */

void d_robot_compute_points(int base_x, int base_y, int L1_px, int L2_px, double shoulder_deg, double elbow_deg, D_RobotPoints *out) {
    D_RobotPoints result;
    result.base.x = base_x;
    result.base.y = base_y;

    double th1 = deg2rad(shoulder_deg);
    double th2 = deg2rad(elbow_deg);

    /* Směr v canvasu:
       x = cos(theta)
       y = -sin(theta)   (kladný úhel proti hodinám, ale y roste dolů)
    */

    /* Konec prvního ramene (joint) */
    double jx = base_x + L1_px * cos(th1); // x roste doprava - vykreslení uprostřed základny base_x
    double jy = base_y - L1_px * sin(th1); // y roste dolů - vykreslení nad základnou base_y

    result.joint.x = (int)lround(jx);
    result.joint.y = (int)lround(jy);

    /* Konec druhého ramene (TCP) – úhel (th1 + th2) */
    double t_total = th1 + th2;
    double tx = jx + L2_px * cos(t_total); // souřadnice TCP - tx
    double ty = jy - L2_px * sin(t_total); // souřadnice TCP - ty 

    result.tcp.x = (int)lround(tx); // zaokrouhlení na celé číslo (2D pole je integer)
    result.tcp.y = (int)lround(ty); 

    *out = result;
}

/* Nakreslení robota podle bodů */
void d_robot_draw_xz(const D_RobotPoints *rp) {
    /* Základna jako "<=====>" okolo base.x, base.y */
    const char *base_str = "<=====>";
    int len = 0;
    while (base_str[len] != '\0') len++; // délka řetězce základny

    int start_x = rp->base.x - len / 2; // začátek základny tak, aby byla centrovaná na base.x
    for (int i = 0; i < len; ++i) {
        d_canvas_set_pixel(start_x + i, rp->base.y, base_str[i], B_CYAN); /* základna modrá */
    }

    /* Ramena jako '*' */
    d_canvas_draw_line(rp->base.x,  rp->base.y - 1, rp->joint.x, rp->joint.y, '*', B_CYAN); // rameno L1
    d_canvas_draw_line(rp->joint.x, rp->joint.y, rp->tcp.x,   rp->tcp.y, '*', B_CYAN); // rameno L2

    /* Klouby jako 'O' */
    d_canvas_set_pixel(rp->base.x,  rp->base.y - 1, 'O', B_RED);    /* J0 kloub */
    d_canvas_set_pixel(rp->joint.x, rp->joint.y,    'O', B_RED);    /* J1 kloub */

    /* TCP jako '+' */
    d_canvas_set_pixel(rp->tcp.x,   rp->tcp.y,      '+', B_YELLOW); /* TCP */
}

/* Nakreslení robota pro XY pohled (pohled shora)
 * - základna jako vertikální sloupek
 * - ramena od bodu base.x, base.y
 */
void d_robot_draw_xy(const D_RobotPoints *rp) {
    /* Základna jako vertikální "sloupek" | okolo base.y */
    int half_h = 2;  // výška půlky sloupku (celkem 2*half_h+1)
    for (int dy = -half_h; dy <= half_h; ++dy) {
        d_canvas_set_pixel(rp->base.x, rp->base.y + dy, '|', B_CYAN);
    }

    /* Ramena jako '*' – od středu základny */
    d_canvas_draw_line(rp->base.x, rp->base.y,
                       rp->joint.x, rp->joint.y, '*', B_CYAN); // L1

    d_canvas_draw_line(rp->joint.x, rp->joint.y,
                       rp->tcp.x,   rp->tcp.y,   '*', B_CYAN); // L2

    /* Klouby jako 'O' */
    d_canvas_set_pixel(rp->base.x,  rp->base.y, 'O', B_RED);     /* J0 */
    d_canvas_set_pixel(rp->joint.x, rp->joint.y, 'O', B_RED);    /* J1 */

    /* TCP jako '+' */
    d_canvas_set_pixel(rp->tcp.x, rp->tcp.y, '+', B_YELLOW);
}

/* Školní funkce: spočítá body a rovnou nakreslí robota (školní úhly) */
void d_robot_draw_from_angles(int base_x, int base_y, int L1_px, int L2_px, double shoulder_deg, double elbow_deg) {
    D_RobotPoints rp;
    d_robot_compute_points(base_x, base_y,
                           L1_px, L2_px,
                           shoulder_deg, elbow_deg,
                           &rp);
    d_robot_draw_xz(&rp);
}

/* Výpočet bodů podle pekla DOBOT úhlů J2/J3 a L1/L2 v mm
    - základna na [base_x, base_y] v pixelech
    - J2/J3 ve stupních (Dobotovské úhly)
    - výstup do out (D_RobotPoints)

   KInverse + KForward používají:
   joint:  x = L1 * sin(J2),  z = L1 * cos(J2)
   tcp:    x = L1 * sin(J2) + L2 * cos(J3)
           z = L1 * cos(J2) - L2 * sin(J3)
*/
void d_robot_compute_points_dobot(int base_x, int base_y, const JointsDeg *joints, D_RobotPoints *out){
    D_RobotPoints result;
    result.base.x = base_x;
    result.base.y = base_y;

    double j2 = deg2rad(joints->J2_deg);
    double j3 = deg2rad(joints->J3_deg);

    /* Kloub v mm (v souřadném systému robota) */
    double joint_x_mm = L1 * sin(j2);
    double joint_z_mm = L1 * cos(j2);

    /* TCP/wrist v mm (stejný model jako KForward) */
    double tcp_x_mm = L1 * sin(j2) + L2 * cos(j3);
    double tcp_z_mm = L1 * cos(j2) - L2 * sin(j3);

    /* Pokud chceš kreslit skutečný TCP včetně offsetu v X:
       tcp_x_mm += EFFECTOR_OFFSET_X;
    */

    /* Převod mm na pixely, z nahoru a y dolů */
    result.joint.x = base_x + d_mm_to_px(joint_x_mm);
    result.joint.y = base_y - d_mm_to_px(joint_z_mm);

    result.tcp.x   = base_x + d_mm_to_px(tcp_x_mm);
    result.tcp.y   = base_y - d_mm_to_px(tcp_z_mm);

    *out = result;
}

/* -------------------------------------------------------------------------
 * Výpočet bodů v projekci do roviny X-Y (pohled shora).
 *
 * Předpoklad:
 *  - J1 = otočení základny kolem osy Z
 *  - J2, J3 = stejné ramenové úhly jako v "vertikální" kinematice
 *
 * Postup:
 * 1) spočteme horizontální projekci vzdálenosti od osy rotační základny:
 *      r_joint = L1 * cos(J2)
 *      r_tcp   = L1 * cos(J2) + L2 * cos(J2 + J3)
 * 2) tuto vzdálenost rozložíme do X, Y pomocí J1:
 *      joint_x = r_joint * cos(J1)
 *      joint_y = r_joint * sin(J1)
 *      tcp_x   = r_tcp   * cos(J1)
 *      tcp_y   = r_tcp   * sin(J1)
 * 3) převedeme z mm do pixelů a namapujeme na canvas:
 *      canvas_x = base_x + mm_to_px(...)
 *      canvas_y = base_y - mm_to_px(...)  // Y roste dolů, proto mínus
 * ------------------------------------------------------------------------- */
void d_robot_compute_points_dobot_xy(int base_x, int base_y, const JointsDeg *joints, D_RobotPoints *out) {
    D_RobotPoints result;
    result.base.x = base_x;
    result.base.y = base_y;

    /* přepočet na radiany */
    double j1 = deg2rad(joints->J1_deg);
    double j2 = deg2rad(joints->J2_deg);
    double j3 = deg2rad(joints->J3_deg);

    /* --- 1) horizontální vzdálenosti od osy Z (stejné X jako v XZ modelu) --- */
    double joint_r_mm = L1 * sin(j2);                    // joint_x_mm v XZ
    double tcp_r_mm   = L1 * sin(j2) + L2 * cos(j3);     // tcp_x_mm v XZ

    /* pokud chceš kreslit skutečný TCP včetně offsetu v X:
       tcp_r_mm += EFFECTOR_OFFSET_X;
    */

    /* --- 2) rozložení do X-Y podle J1 --- */
    double joint_x_mm = joint_r_mm * cos(j1);
    double joint_y_mm = joint_r_mm * sin(j1);

    double tcp_x_mm   = tcp_r_mm * cos(j1);
    double tcp_y_mm   = tcp_r_mm * sin(j1);

    /* --- 3) převod na pixely a mapování do canvasu --- */
    result.joint.x = base_x + d_mm_to_px(joint_x_mm);
    result.joint.y = base_y - d_mm_to_px(joint_y_mm);  /* y roste dolů */

    result.tcp.x   = base_x + d_mm_to_px(tcp_x_mm);
    result.tcp.y   = base_y - d_mm_to_px(tcp_y_mm);

    *out = result;
}


/* Vykreslení robota podle J2/J3 (Dobota) – základna na daných pixelech */
void d_robot_draw_from_joints_xz(int base_x, int base_y, const JointsDeg *joints) {
    D_RobotPoints rp;
    d_robot_compute_points_dobot(base_x, base_y, joints, &rp);
    //d_canvas_clear();
    d_robot_draw_xz(&rp);
    d_canvas_render();
}

/* Komfortní varianta – base doprostřed dole */
void d_robot_draw_from_joints_xz_default(const JointsDeg *joints) {
    int base_x = DR_CANVAS_W / 2;
    int base_y = DR_CANVAS_H - 2;
    d_robot_draw_from_joints_xz(base_x, base_y, joints);
}

/* Vykreslení robota v projekci X-Y podle J1/J2/J3 (pohled shora)
   – základna na daných pixelech (base_x, base_y) */
void d_robot_draw_from_joints_xy(int base_x, int base_y, const JointsDeg *joints) {
    D_RobotPoints rp;

    d_robot_compute_points_dobot_xy(base_x, base_y, joints, &rp);
    d_robot_draw_xy(&rp);
    d_canvas_render();
}

/* Komfortní varianta – základnu dej doprostřed canvasu (pohled shora) */
void d_robot_draw_from_joints_xy_default(const JointsDeg *joints) {
    int base_x = DR_CANVAS_W / 2;
    int base_y = DR_CANVAS_H / 2;   /* pro top view dává smysl střed canvasu */

    d_robot_draw_from_joints_xy(base_x, base_y, joints);
}


/* Jednoduchý výpis textu na jeden řádek (s barvou) */
void d_print(int x, int y, const char *text, t_color color) {
    if (y < 0 || y >= DR_CANVAS_H) return;
    if (text == NULL) return;

    int cx = x;
    for (int i = 0; text[i] != '\0'; ++i) {
        if (cx >= 0 && cx < DR_CANVAS_W) {
            d_canvas_set_pixel(cx, y, text[i], color);
        }
        cx++;
        if (cx >= DR_CANVAS_W) break;  // konec řádku plátna
    }
}

/* Víceřádkový text do okénka (s barvou) */
void d_print_box(int x, int y, int width, const char *text, t_color color) {
    if (width <= 0 || text == NULL) return;

    int cx = x;
    int cy = y;

    for (int i = 0; text[i] != '\0'; ++i) {
        char ch = text[i]; // Text znak po znaku

        if (ch == '\n' || cx >= x + width) {
            /* nový řádek */
            cy++;
            cx = x;
            if (ch == '\n') continue;   // znak '\n' už nekreslíme
        }

        if (cy < 0 || cy >= DR_CANVAS_H) break;

        if (cx >= 0 && cx < DR_CANVAS_W) {
            d_canvas_set_pixel(cx, cy, ch, color);
        }
        cx++;
    }
}

void d_show_dobot(FILE *file, char *banner, JointsDeg *joints, TCP_Position *position, CliOptions *dmod) {
    if (dmod->mode == MODE_FORWARD) {
        while(d_io_read_joints(file, joints)!=IO_ERR_FORMAT) {
            if(KForward(joints, position)==K_SUCCESS) {
                t_clrscr();
                d_draw_title_bar(banner);
                t_textcolor(B_YELLOW);
                printf("\nTCP: x = %.3f mm, y = %.3f mm, z = %.3f mm\n", position->x, position->y, position->z);
                printf("Klouby: J1 = %.3f°, J2 = %.3f°, J3 = %.3f°\n", joints->J1_deg, joints->J2_deg, joints->J3_deg);
                d_canvas_clear();
                d_print_box(6, DR_CANVAS_H - 2, 13, "Zobrazení XZ", MAGENTA);
                d_print_box(DR_CANVAS_W - 60, DR_CANVAS_H - 2, 13, "Zobrazení XY", MAGENTA);
                d_robot_draw_from_joints_xz(10, DR_CANVAS_H - 4, joints);
                d_robot_draw_from_joints_xy(70, (DR_CANVAS_H / 2), joints);
                d_canvas_render();
                t_keypress_wait(NO_CLEAN_BUFF);
            }
        }
    } else if (dmod->mode == MODE_INVERSE) {
        while(d_io_read_tcp(file, position)!=IO_ERR_FORMAT) {
            if(KInverse(position, joints)==K_SUCCESS) {
                t_clrscr();
                d_draw_title_bar(banner);
                t_textcolor(B_YELLOW);
                printf("\nTCP: x = %.3f mm, y = %.3f mm, z = %.3f mm\n", position->x, position->y, position->z);
                printf("Klouby: J1 = %.3f°, J2 = %.3f°, J3 = %.3f°\n", joints->J1_deg, joints->J2_deg, joints->J3_deg);
                d_canvas_clear();
                d_print_box(6, DR_CANVAS_H - 2, 13, "Zobrazení XZ", MAGENTA);
                d_print_box(DR_CANVAS_W - 60, DR_CANVAS_H - 2, 13, "Zobrazení XY", MAGENTA);
                d_robot_draw_from_joints_xz(10, DR_CANVAS_H - 4, joints);
                d_robot_draw_from_joints_xy(70, (DR_CANVAS_H / 2), joints);
                d_canvas_render();
                t_keypress_wait(NO_CLEAN_BUFF);
            }
        }
    }
    t_clrscr();
}