/* d_robot_canvas.c 
 * 
 *  Implementace kreslení robota na textové plátno v terminálu.
 * 
 *  Autor: Josef Němec
 *  Datum: 2025-10-05
 * 
 *  Popis:
 * 
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
#include <math.h>
#include <stdlib.h>   /* abs() */

#include "config.h"       /* Ramena L1, L2 v mm, OFFSETY */
#include "d_terminal.h"
#include "d_draw_robot.h"
#include "math_utils.h"   /* deg2rad() */

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
    return (int)lround(mm / DR_MM_PER_PIXEL); // zaokrouhlení na nejbližší pixel
}

/* Vymazání plátna */
void d_canvas_clear(void) { // Vyplní plátno mezerami ve stylu 90. let :D
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
    int dx = abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (1) {
        d_canvas_set_pixel(x0, y0, ch, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

/* Vypsání plátna na obrazovku */
void d_canvas_render(void) {
    t_gotoxy(1, 1);  /* začneme levo nahoře */

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
void d_robot_compute_points(int base_x,
                            int base_y,
                            int L1_px,
                            int L2_px,
                            double shoulder_deg,
                            double elbow_deg,
                            D_RobotPoints *out)
{
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
    double jx = base_x + L1_px * cos(th1);
    double jy = base_y - L1_px * sin(th1);

    result.joint.x = (int)lround(jx);
    result.joint.y = (int)lround(jy);

    /* Konec druhého ramene (TCP) – úhel (th1 + th2) */
    double t_total = th1 + th2;
    double tx = jx + L2_px * cos(t_total);
    double ty = jy - L2_px * sin(t_total);

    result.tcp.x = (int)lround(tx);
    result.tcp.y = (int)lround(ty);

    *out = result;
}

/* Nakreslení robota podle bodů */
void d_robot_draw(const D_RobotPoints *rp) {
    /* Základna jako "<=====>" okolo base.x, base.y */
    const char *base_str = "<=====>";
    int len = 0;
    while (base_str[len] != '\0') len++;

    int start_x = rp->base.x - len / 2;
    for (int i = 0; i < len; ++i) {
        d_canvas_set_pixel(start_x + i, rp->base.y, base_str[i], B_CYAN);
    }

    /* Ramena jako '*' */
    d_canvas_draw_line(rp->base.x,  rp->base.y - 1,  /* base kloub nad základnou */
                       rp->joint.x, rp->joint.y,
                       '*', B_CYAN);
    d_canvas_draw_line(rp->joint.x, rp->joint.y,
                       rp->tcp.x,   rp->tcp.y,
                       '*', B_CYAN);

    /* Klouby jako 'O' */
    d_canvas_set_pixel(rp->base.x,  rp->base.y - 1, 'O', B_RED);    /* base kloub */
    d_canvas_set_pixel(rp->joint.x, rp->joint.y,    'O', B_RED);    /* loket */

    /* TCP jako '+' */
    d_canvas_set_pixel(rp->tcp.x,   rp->tcp.y,      '+', B_YELLOW);
}

/* Školní funkce: spočítá body a rovnou nakreslí robota (školní úhly) */
void d_robot_draw_from_angles(int base_x,
                              int base_y,
                              int L1_px,
                              int L2_px,
                              double shoulder_deg,
                              double elbow_deg)
{
    D_RobotPoints rp;
    d_robot_compute_points(base_x, base_y,
                           L1_px, L2_px,
                           shoulder_deg, elbow_deg,
                           &rp);
    d_robot_draw(&rp);
}

/* Výpočet bodů podle DOBOT úhlů J2/J3 a L1/L2 v mm */
/* KInverse + KForward používají:
   joint:  x = L1 * sin(J2),  z = L1 * cos(J2)
   tcp:    x = L1 * sin(J2) + L2 * cos(J3)
           z = L1 * cos(J2) - L2 * sin(J3)
*/
void d_robot_compute_points_dobot(int base_x,
                                  int base_y,
                                  const JointsDeg *joints,
                                  D_RobotPoints *out)
{
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

    /* Převod mm → pixely, z nahoru → y dolů */
    result.joint.x = base_x + d_mm_to_px(joint_x_mm);
    result.joint.y = base_y - d_mm_to_px(joint_z_mm);

    result.tcp.x   = base_x + d_mm_to_px(tcp_x_mm);
    result.tcp.y   = base_y - d_mm_to_px(tcp_z_mm);

    *out = result;
}

/* Vykreslení robota podle J2/J3 (Dobota) – základna na daných pixelech */
void d_robot_draw_from_joints(int base_x,
                              int base_y,
                              const JointsDeg *joints)
{
    D_RobotPoints rp;
    d_robot_compute_points_dobot(base_x, base_y, joints, &rp);
    //d_canvas_clear();
    d_robot_draw(&rp);
    d_canvas_render();
}

/* Komfortní varianta – base doprostřed dole */
void d_robot_draw_from_joints_default(const JointsDeg *joints)
{
    int base_x = DR_CANVAS_W / 2;
    int base_y = DR_CANVAS_H - 2;
    d_robot_draw_from_joints(base_x, base_y, joints);
}

/* Jednoduchý výpis textu na jeden řádek (s barvou) */
void d_print(int x, int y, const char *text, t_color color)
{
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
void d_print_box(int x, int y, int width, const char *text, t_color color)
{
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
