CC = gcc

CFLAGS = -O2 -Wall -Wextra
LDFLAGS = -lncursesw

# Directories
WIN_DIR = windows
LIN_DIR = linux

# Targets
WIN_BIN = windows.exe
LIN_BIN = linux.exe

# Sources
WIN_SRC = $(WIN_DIR)/main.c
LIN_SRC = $(LIN_DIR)/main.c

all: $(WIN_BIN) $(LIN_BIN)

$(WIN_BIN): $(WIN_SRC)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(LIN_BIN): $(LIN_SRC)
	$(CC) $(CFLAGS) $(LIN_DIR)/pwsh10k.c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(WIN_BIN) $(LIN_BIN)

.PHONY: all clean