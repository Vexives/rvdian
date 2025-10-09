# All files
SOURCES = audio_wrapper.c array_utils.c complex_numbers.c frame_processes.c window_funcs.c wrapper_utils.c main.c

# Source Path
DIRS = src
BIN = bin
SEARCHC = $(addsuffix /*.c ,$(DIRS))
SRCS = $(wildcard $(SEARCHC))

# Object files
OBJECTS = $(SRCS:%.c=%.o)

# Executable
EXE = rvdian

# Flags
CFLAGS = -Wall -pg
LDFLAGS = -lm

# Libraries
LIBS = 

# Compiler
CC = gcc

default: all	
all: $(EXE)

$(EXE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(EXE) $(LIBS)

.PHONY: clean
cleanall:
	rm -f $(EXE) $(OBJECTS) *~

cleanexe:
	rm -f $(EXE) *~

cleanobj:
	rm -f $(OBJECTS) *~

help:
	@echo 'SOURCES:'
	@echo $(SRCS)
	@echo 'OBJECTS:'
	@echo $(OBJECTS)