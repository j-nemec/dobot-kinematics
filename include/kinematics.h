#ifndef KINEMATICS_H
#define KINEMATICS_H

typedef enum { // Kinematics status - typ s návratovými kódy kinematiky robota
    K_SUCCESS = 0,
    K_ERR_UNREACHABLE = -1,
    K_ERR_INVALID_ANGLES = -2,
    K_ERR_NO_SOLUTION = -3
} KinematicsStatus; 

typedef struct { // Klouby - úhly ve stupních
    double J1_deg, J2_deg, J3_deg;
} JointsDeg;

typedef struct { // Klouby - úhly v radiánech
    double J1,     J2,     J3;
} JointsRad;

typedef struct { // Tool Center Point (TCP) pozice v mm
    double x, y, z;
} TCP_Position;

/* Test zda jsou úhly v platném rozsahu */
KinematicsStatus CheckAngles(JointsDeg *joints); 

/* Inverzní kinematika: Spočítá úhly kloubů z pozice TCP */
KinematicsStatus KInverse(TCP_Position *position, JointsDeg *joints); 

/* Forward kinematika: Spočítá pozici TCP z úhlů kloubů */
KinematicsStatus KForward(JointsDeg *joints, TCP_Position *position);

#endif
