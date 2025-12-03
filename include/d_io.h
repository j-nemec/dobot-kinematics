#ifndef D_IO_H
#define D_IO_H

#include <stdio.h>
#include "kinematics.h"

typedef enum {
    IO_OK = 0,
    IO_ERR_OPEN = -1,
    IO_ERR_FORMAT = -2
} IO_Status;

IO_Status d_io_save_joints(const char *filename, const JointsDeg *joints);
IO_Status d_io_save_tcp(const char *filename, const TCP_Position *position);
IO_Status d_io_read_joints(FILE *fr, JointsDeg *joints);
IO_Status d_io_read_tcp(FILE *fr, TCP_Position *position);

#endif /* D_IO_H */
