#include <stdio.h>
#include <math.h>
#include "kinematics.h"
#include "math_utils.h"
#include "config.h"

KinematicsStatus CheckAngles(JointsDeg *joints) { // Check if joint angles are within valid ranges
    if(joints->J1_deg >= J1_MIN_DEG && joints->J1_deg <= J1_MAX_DEG &&
       joints->J2_deg >= J2_MIN_DEG && joints->J2_deg <= J2_MAX_DEG &&
       joints->J3_deg >= J3_MIN_DEG && joints->J3_deg <= J3_MAX_DEG) {
       return K_SUCCESS;
    } else {
        return K_ERR_INVALID_ANGLES; // One or more angles are out of range
    }
}

KinematicsStatus KInverse(TCP_Position *position, JointsDeg *joints) { // Inverse kinematics: Calculate joint angles from TCP position
    /* if(position->x < 0 || position->y < 0 || position->z < 0) {
        return K_ERR_UNREACHABLE; // Unreachable position
    } */
    double c = hypot((position->x - EFFECTOR_OFFSET_X), position->z);
    // double c = sqrt(pow(position->x - EFFECTOR_OFFSET_X, 2) + pow(position->z, 2));
    double alpha = atan2(position->z, position->x - EFFECTOR_OFFSET_X);
    double beta = acos(-(L2*L2 - L1*L1 - c*c)/(2 * L1 * c));
    joints->J2_deg = 90 - rad2deg(alpha + beta);
    joints->J3_deg = 180 - rad2deg(alpha + beta + acos((L1*L1 + L2*L2 - c*c)/(2 * L1 * L2)));

    return K_SUCCESS; // Success 
}

// Forward kinematics: Calculate TCP position from joint angles
KinematicsStatus KForward(JointsDeg *joints, TCP_Position *position) { // Forward kinematics: Calculate TCP position from joint angles
    switch(CheckAngles(joints))  {
       case K_SUCCESS: {
            JointsRad joints_rad;

            joints_rad.J1 = deg2rad(joints->J1_deg);
            joints_rad.J2 = deg2rad(joints->J2_deg);
            joints_rad.J3 = deg2rad(joints->J3_deg);    
            
            position->z = L1 * cos(joints_rad.J2) - L2 * sin(joints_rad.J3);
            position->x = L1 * sin(joints_rad.J2) + L2 * cos(joints_rad.J3);
            position->y = 0; // Assuming planar movement for simplicity

            return K_SUCCESS; // Success
       }
       case K_ERR_INVALID_ANGLES:
           return K_ERR_INVALID_ANGLES; // Invalid angles
       default:
           return K_ERR_UNREACHABLE; // Unreachable position
   } 
}