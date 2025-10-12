CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -O2 -Iinclude
LDFLAGS := -lm

SRC := src/main.c src/kinematics.c
BIN := dobot_kin

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDFLAGS)

clean:
	rm -f $(BIN)
