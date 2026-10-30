C     = gcc
CFLAGS = -Wall -Wextra -O2
SRC    = src/server.c

ifeq ($(OS),Windows_NT)
    TARGET = server.exe
    LDLIBS = -lws2_32
    RUN    = $(TARGET)
    RM     = del /Q
else
    TARGET = server
    LDLIBS =
    RUN    = ./$(TARGET)
    RM     = rm -f
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

run: $(TARGET)
	$(RUN)

clean:
	$(RM) $(TARGET)

.PHONY: all run clean
