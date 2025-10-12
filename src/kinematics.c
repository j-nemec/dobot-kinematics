#include <stdio.h>
#include "kinematics.h"

void test(JointsDeg *joints) {
    printf("J1 = %.2f\n", joints->J1_deg);
    printf("J2 = %.2f\n", joints->J2_deg);
    printf("J3 = %.2f\n", joints->J3_deg);
    joints->J1_deg = 90;
}
