#include <stdio.h>
#include "kinematics.h"
#include "config.h"

int main() {
    JointsDeg joints;

    joints.J1_deg = 0;
    joints.J2_deg = 45;
    joints.J3_deg = 65;

    printf("Ahoj - tohle je moje první aplikace.\n");
    printf("Kofigurace:\n\tL1 = %.2f\n\tL2 = %.2f\n", L1, L2);

    test(&joints); // Předání adresy struktury
    test(&joints);
    return 0;
}
