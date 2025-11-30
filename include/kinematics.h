#ifndef KINEMATICS_H
#define KINEMATICS_H

typedef enum { // Kinematics function return status codes
    K_SUCCESS = 0,
    K_ERR_UNREACHABLE = -1,
    K_ERR_INVALID_ANGLES = -2,
    K_ERR_NO_SOLUTION = -3
} KinematicsStatus; 

typedef struct { // Joint angles in degrees
    double J1_deg, J2_deg, J3_deg;
} JointsDeg;

typedef struct { // Joint angles in radians
    double J1,     J2,     J3;
} JointsRad;

typedef struct { // Tool Center Point (TCP) position in mm
    double x, y, z;
} TCP_Position;

//double rad2deg(double radians);

/* Check if joint angles are within valid ranges */
KinematicsStatus CheckAngles(JointsDeg *joints); 

/* Inverse kinematics: Calculate joint angles from TCP position */
KinematicsStatus KInverse(TCP_Position *position, JointsDeg *joints); 

/* Forward kinematics: Calculate TCP position from joint angles */
KinematicsStatus KForward(JointsDeg *joints, TCP_Position *position);

#endif
