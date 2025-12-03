#include <stdio.h>
#include "d_io.h"
#include "kinematics.h"


IO_Status d_io_save_joints(const char *filename, const JointsDeg *joints) {
    FILE *fw;
    if(!(fw=fopen(filename, "a"))) {
        return IO_ERR_OPEN;
    }
    fprintf(fw, "%.6f %.6f %.6f\n", joints->J1_deg, joints->J2_deg, joints->J3_deg);
    fclose(fw);
    return IO_OK;
}

IO_Status d_io_save_tcp(const char *filename, const TCP_Position *position) {
    FILE *fw;
    if(!(fw=fopen(filename, "a"))) {
        return IO_ERR_OPEN;
    }
    fprintf(fw, "%.6f %.6f %.6f\n", position->x, position->y, position->z);
    fclose(fw);
    return IO_OK;
}

IO_Status d_io_read_joints(FILE *fr, JointsDeg *joints) {
    double j1, j2, j3;

    if(fscanf(fr, "%lf %lf %lf", &j1, &j2, &j3) != 3) {
        return IO_ERR_FORMAT;
    }

    joints->J1_deg = j1;
    joints->J2_deg = j2;
    joints->J3_deg = j3;

    return IO_OK;
}

IO_Status d_io_read_tcp(FILE *fr, TCP_Position *position) {
    double x, y, z;

    if(fscanf(fr, "%lf %lf %lf", &x, &y, &z) != 3) {
        return IO_ERR_FORMAT;
    }

    position->x = x;
    position->y = y;
    position->z = z;

    return IO_OK;
}