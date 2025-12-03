CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -O2 -Iinclude
LDFLAGS := -lm

SRC := src/main.c src/kinematics.c src/d_terminal.c src/d_draw_robot.c src/math_utils.c src/d_io.c src/ui.c
BIN := ./build/dobot_kin

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDFLAGS)

clean:
	rm -f $(BIN)
