#ifndef KINEMATICS_H
#define KINEMATICS_H

typedef struct {
    double J1_deg, J2_deg, J3_deg;
} JointsDeg;

typedef struct {
    double J1,     J2,     J3;
} JointsRad;

void test(JointsDeg *joints);

#endif
