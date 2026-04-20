CC      := gcc
CFLAGS  := -Wall -Wextra -O2

# Your program name
TARGET  := pwsh10k

# All .c files in the current directory
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

# Default rule
all: $(TARGET)

# Link step
$(TARGET): $(OBJS)
	$(CC) $(SRC) $(OBJS) -o $@

# Compile step
%.o: %.c
	$(CC) $(SRC) $(CFLAGS) -c $< -o $@

# Cleanup
clean:
	rm -f $(OBJS)

print:
	@echo SRCS=$(SRCS)
	@echo OBJS=$(OBJS)

.PHONY: all clean
