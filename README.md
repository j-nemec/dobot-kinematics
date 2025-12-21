# dobot-kinematics
C-based CLI simulation of the Dobot Magician robotic arm with forward and inverse kinematics

A smart command-line application for validating and converting kinematic coordinates of the Dobot Magician robot.
This project was developed as a semester assignment for the C programming language course.

Features:
    Forward kinematics
        Converts joint angles (J1, J2, J3) into TCP position (x, y, z)
    Inverse kinematics
        Converts TCP position (x, y, z) into joint angles (J1, J2, J3)
    ASCII visualization
        Simple textual visualization for approximate spatial validation
    File-based processing
        Read input data from files and write results to output files
    Non-interactive CLI
        Designed for automation and batch validation without user interaction

📁 Project Structure
    /src/        Source files
    /include/    Header files
    /build/      Build output (final executable)

📄 Data Files
    The application works with the following files:
    File name	Description
    joints.dat	Input file containing joint angles J1, J2, J3
    positions.dat	Input file containing TCP positions x, y, z
    results_joints.dat	Output file: TCP → J1, J2, J3
    results_positions.dat	Output file: J1, J2, J3 → TCP x, y, z
    Robot parameters and default file names are defined in: /include/config.h

🛠️ Compilation
    macOS / Linux
        Requirements:
            gcc
            make
            make
    Windows (MSYS2 recommended)
        Download MSYS2:
            👉 https://www.msys2.org
        Install required packages:
            pacman -S mingw-w64-x86_64-gcc
            pacman -S make
    Enable UTF-8 support in the Windows terminal:
        chcp 65001

Running the Application
    ./build/dobot_kin

⚙️ Command-Line Options
Help
    ./build/dobot_kin -h
    Displays application help.
Forward Kinematics (Joints → TCP)
    ./build/dobot_kin -f joints.dat -o out.dat
    Converts joint angles J1, J2, J3 from joints.dat into TCP positions x, y, z, and saves the result to out.dat.
Inverse Kinematics (TCP → Joints)
    ./build/dobot_kin -i positions.dat -o out.dat
    Converts TCP positions x, y, z from positions.dat into joint angles J1, J2, J3, and saves the result to out.dat.
ASCII Visualization
    ./build/dobot_kin -f joints.dat -a
    Displays an ASCII visualization of the robot while stepping through joint data from joints.dat.

🎓 Academic Context
    This project was created as a semester project in C, with emphasis on:
    structured program design
        file I/O
        command-line interfaces
        mathematical modeling of robotic kinematics




