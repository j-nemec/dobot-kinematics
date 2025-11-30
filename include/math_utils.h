/*
 *  math_utils.h
 *  Jednoduché matematické utility pro převod mezi stupni a radiány.
 *  Mohl bych to řešit statickými funckemi v kinematics.c a d_draw_robot.c, 
 *  ale pro přehlednost a opětovné použití je lepší mít funkce v samostatném modulu.
 */

#ifndef MATH_UTILS_H
#define MATH_UTILS_H 

double rad2deg(double radians);
double deg2rad(double degrees);

#endif /* MATH_UTILS_H */