/* d_draw_robot.h */
#ifndef D_DRAW_ROBOT_H
#define D_DRAW_ROBOT_H

#include "d_terminal.h"
#include "kinematics.h"  /* kvůli JointsDeg */

/* Rozměry plátna (v pixelech/znacích) */
#define DR_CANVAS_W 80
#define DR_CANVAS_H 30

/* Měřítko: kolik mm odpovídá jednomu pixelu */
#define DR_MM_PER_PIXEL 8.0   /* 10 mm = 1 pixel, klidně změň */

/* Jeden bod v pixelové mřížce */
typedef struct {
    int x;  /* 0 .. DR_CANVAS_W-1 */
    int y;  /* 0 .. DR_CANVAS_H-1 */
} D_Pixel;

/* Všechny klíčové body robota */
typedef struct {
    D_Pixel base;   /* základna */
    D_Pixel joint;  /* kloub mezi L1 a L2 */
    D_Pixel tcp;    /* TCP */
} D_RobotPoints;

/* mm -> pixel */
int d_mm_to_px(double mm);

/* Canvas API */
void d_canvas_clear(void);
void d_canvas_set_pixel(int x, int y, char ch, t_color color);
void d_canvas_draw_line(int x0, int y0, int x1, int y1, char ch, t_color color);
void d_canvas_render(void);

/* Spočítá klíčové body robota podle zadaných úhlů ramen (výpočet do výuky) */
void d_robot_compute_points(int base_x, int base_y, int L1_px, int L2_px, double shoulder_deg, double elbow_deg, D_RobotPoints *out);
void d_robot_draw(const D_RobotPoints *rp);
void d_robot_draw_from_angles(int base_x, int base_y, int L1_px, int L2_px, double shoulder_deg, double elbow_deg);

/* spočítá body z úhlů Dobota (J2_deg, J3_deg) + L1, L2 z config.h */
void d_robot_compute_points_dobot(int base_x, int base_y, const JointsDeg *joints, D_RobotPoints *out);

/* vykreslí robota podle J2/J3 – základna na [base_x, base_y] */
void d_robot_draw_from_joints(int base_x, int base_y, const JointsDeg *joints);

/* Bezpozicové zobrazení – základna doprostřed dole */
void d_robot_draw_from_joints_default(const JointsDeg *joints);

/*  d_print a d_print_box pro výpis textu na plátno */
void d_print(int x, int y, const char *text, t_color color); 
void d_print_box(int x, int y, int width, const char *text, t_color color);

#endif /* D_DRAW_ROBOT_H */
