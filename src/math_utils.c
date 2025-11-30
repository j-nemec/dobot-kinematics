#include <math.h>
#define _USE_MATH_DEFINES


double rad2deg(double radians) { // Convert radians to degrees - only for internal use
    return radians*180/M_PI;
}

double deg2rad(double degrees) { // Convert degrees to radians - only for internal use
    return degrees*M_PI/180;
}
