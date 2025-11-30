#include <stdio.h>
#include "kinematics.h"
#include "d_terminal.h"
#include "d_draw_robot.h"
#include "config.h"
#include <string.h>

int main() {
    JointsDeg joints;
    TCP_Position position;

    position.x = L2;
    position.y = 0;
    position.z = 135;   
    
    t_clrscr();
    t_init();
   
    printf("Kofigurace:\n\tL1 = %.2f\n\tL2 = %.2f\n", L1, L2);
    /*
    printf("Inverse kinematics test:\n");
    printf("Position (mm: x, 0, z): ");
    scanf("%lf, %lf, %lf", &position.x, &position.y, &position.z);
    printf("x=%.2f, y=%.2f, z=%.2f\n", position.x, position.y, position.z);

    switch(KInverse(&position, &joints))  {
       case K_SUCCESS:
           printf("Inverse kinematics: \x1b[32mOK\x1b[0m\n"); // \x1b[32mToto je zelený text\x1b[0m\n
           printf("\tJ1 = %.2f\n", joints.J1_deg);
           printf("\tJ2 = %.2f\n", joints.J2_deg);
           printf("\tJ3 = %.2f\n", joints.J3_deg);
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
    */
    printf("\nForward kinematics test:\n");
    joints.J1_deg = 0; // Základna rovně
    joints.J2_deg = 20; // Rameno 1
    joints.J3_deg = 0; // Rameno 2
   
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
    t_textcolor(YELLOW);
    t_keypress_wait();
    t_clrscr();
    t_hide_cursor();
    d_canvas_clear();

    char info[128];
    snprintf(info, sizeof(info), "TCP: x=%.1fmm z=%.1fmm\nJ2=%.1f deg J3=%.1f deg", position.x, position.z, joints.J2_deg, joints.J3_deg);

    /* třeba dolů pod robota, pár řádků nad spodní hranou */
    int text_x = 2;
    int text_y = DR_CANVAS_H - 4;
    
    d_robot_draw_from_joints(2, DR_CANVAS_H - 2, &joints);
    d_robot_draw_from_joints(30, DR_CANVAS_H - 2, &joints);
    t_textcolor(B_YELLOW);
    d_print_box(2, 2, DR_CANVAS_W - 4, info, B_YELLOW);

    d_canvas_render();
    t_keypress_wait();
    t_show_cursor();
    t_shutdown();
    return 0;
}
