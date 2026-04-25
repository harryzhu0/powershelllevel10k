CC = gcc
CFLAGS = -O2 -lncursesw -Wall -Wextra

# Directories
WIN_DIR = windows
LIN_DIR  = linux

# Targets
WIN_BIN = windows.exe
LIN_BIN  = linux.exe

all: $(WIN_BIN) $(LIN_BIN)

$(WIN_BIN): $(WIN_DIR)/main.c
	$(CC) $(CFLAGS) $< -o $@

$(LIN_BIN): $(LIN_DIR)/main.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f */*.o
	rm -f $(WIN_BIN) $(LIN_BIN)

.PHONY: all clean