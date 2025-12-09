#include <math.h>
#define _USE_MATH_DEFINES

#ifndef M_PI // Prostě Windows
#define M_PI 3.14159265358979323846 
#endif

double rad2deg(double radians) { // Převod radiánů na stupně - pouze pro interní použití
    return radians*180/M_PI;
}

double deg2rad(double degrees) { // Převod stupňů na radiány - pouze pro interní použití
    return degrees*M_PI/180;
}
