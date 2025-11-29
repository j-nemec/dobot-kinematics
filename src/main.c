#include <stdio.h>
#include "kinematics.h"
#include "config.h"

int main() {
    JointsDeg joints;
    TCP_Position position;

    position.x = L2;
    position.y = 0;
    position.z = 135;   
    
    printf("Kofigurace:\n\tL1 = %.2f\n\tL2 = %.2f\n", L1, L2);

    printf("Inverse kinematics test:\n");
    printf("Position (mm: x, 0, z): ");
    scanf("%lf, %lf, %lf", &position.x, &position.y, &position.z);
    printf("x=%.2f, y=%.2f, z=%.2f\n", position.x, position.y, position.z);

    switch(KInverse(&position, &joints))  {
       case K_SUCCESS:
           printf("Inverse kinematics: OK\n");
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

    printf("\nForward kinematics test:\n");
   
    switch(KForward(&joints, &position))  {
       case K_SUCCESS:
           printf("Forward kinematics: OK\n");
           printf("\tx = %.2f\n", position.x + EFFECTOR_OFFSET_X);
           printf("\ty = %.2f\n", position.y);
           printf("\tz = %.2f\n", position.z);
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

    return 0;
}
