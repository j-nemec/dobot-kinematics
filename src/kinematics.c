/*
 * ======================== kinematics.c =========================
 * Implementace kinematiky pro 2DOF robotické rameno Dobot.
 * Autor: Josef Němec
 * Datum: 2025-09-15
 * 
 * Popis:
 * Tento modul obsahuje funkce pro výpočet inverzní a forward kinematiky
 * pro robotické rameno Dobot s dvěma stupni volnosti (2DOF).
 * Kinematika je založena na geometrických vztazích mezi délkami ramen
 * a úhly kloubů.
 * Konstanty L1 a L2 představují délky ramen v mm,
 * které jsou definovány v souboru config.h.
 * Funkce používají struktury JointsDeg a TCP_Position pro reprezentaci
 * úhlů kloubů ve stupních a pozice TCP v mm.
 * Funkce vracejí stav kinematiky pomocí výčtového typu KinematicsStatus.
 * 
 * Matematika je královnou všech věd! :) Hýbe nejen robotem, ale i světem. :D
 */

#include <stdio.h>
#include <math.h>
#include "kinematics.h"
#include "math_utils.h"
#include "config.h"

KinematicsStatus CheckAngles(JointsDeg *joints) { // Test zda jsou úhly v platném rozsahu
    if(joints->J1_deg >= J1_MIN_DEG && joints->J1_deg <= J1_MAX_DEG && // Ke struktuře přistupuji přes ukazatel
       joints->J2_deg >= J2_MIN_DEG && joints->J2_deg <= J2_MAX_DEG &&
       joints->J3_deg >= J3_MIN_DEG && joints->J3_deg <= J3_MAX_DEG) {
       return K_SUCCESS;
    } else {
        return K_ERR_INVALID_ANGLES; // Jeden z úhlů je mimo rozsah
    }
}

KinematicsStatus KInverse(TCP_Position *position, JointsDeg *joints) { // Inverzní kinematika: Spočítá úhly kloubů z pozice TCP
    /* if(position->x < 0 || position->y < 0 || position->z < 0) {
        return K_ERR_UNREACHABLE; // Unreachable position
    } */

    /*
        Přístup k výpočtu inverzní kinematiky:
        1. Vypočítat vzdálenost c od základny k TCP
        2. Vypočítat úhel alpha mezi osou X a přímkou od základny k TCP
        3. Vypočítat úhel beta pomocí kosinové věty
        4. Spočítat J2 a J3 z těchto úhlů a převést na stupně
        Problém je v počítání úhlů Dobota - je potřeb uvažovat, že v případě J2 = 0° je rameno svisle nahoru a ne vodorovně.
        Stejný problém je u j3 = 0°, kdy je rameno natažené vodorovně.
    */
    double dynamic_x;
    if(position->y == 0) { 
        dynamic_x = position->x - EFFECTOR_OFFSET_X;
    } else {
        dynamic_x = hypot(position->x - EFFECTOR_OFFSET_X, position->y);
    }

    double c = hypot(dynamic_x, position->z);
    // double c = sqrt(pow(position->x - EFFECTOR_OFFSET_X, 2) + pow(position->z, 2));
    double alpha = atan2(position->z, dynamic_x);
    double beta = acos(-(L2*L2 - L1*L1 - c*c)/(2 * L1 * c));
    joints->J2_deg = 90 - rad2deg(alpha + beta); // Rameno L1 úhel J2 = 0° je svisle nahoru
    joints->J3_deg = 180 - rad2deg(alpha + beta + acos((L1*L1 + L2*L2 - c*c)/(2 * L1 * L2))); 
    // Rameno L2 úhel J3 = 0° je vodorovně
    joints->J1_deg = rad2deg(asin(position->y / dynamic_x));

    return K_SUCCESS; // Success 
}

// Forward kinematika: Spočítá pozici TCP z úhlů kloubů
KinematicsStatus KForward(JointsDeg *joints, TCP_Position *position) { 
    switch(CheckAngles(joints)) {
       case K_SUCCESS: {
            JointsRad joints_rad;

            joints_rad.J1 = deg2rad(joints->J1_deg);
            joints_rad.J2 = deg2rad(joints->J2_deg);
            joints_rad.J3 = deg2rad(joints->J3_deg);   
            
            position->z = L1 * cos(joints_rad.J2) - L2 * sin(joints_rad.J3);
            double dynamic_x = L1 * sin(joints_rad.J2) + L2 * cos(joints_rad.J3);
            position->x = dynamic_x * cos(joints_rad.J1);
            position->y = dynamic_x * sin(joints_rad.J1); // trhám si vlasy

            return K_SUCCESS; // Vše v pořádku return místo break :)
       }
       case K_ERR_INVALID_ANGLES:
           return K_ERR_INVALID_ANGLES; // Špatné úhly
       default:
           return K_ERR_UNREACHABLE; // Nedosažitelná pozice
   } 
}