# All files
SOURCES = array_utils.c audio_wrapper.c complex_numbers.c frame_processes.c window_funcs.c wrapper_utils.c main.c

# Source Path
DIRS = src
SEARCHC = $(addsuffix /*.c ,$(DIRS))
SRCS = $(wildcard $(SEARCHC))

# Object files
OBJECTS = $(SOURCES:%.c=%.o)

# Executable
EXE = rvdian

# Flags
CFLAGS = -Wall -g
LDFLAGS = -lm

# Libraries
LIBS = 

# Compiler
CC = gcc

#vpath %.c $(DIRS)

default: all	
all: $(EXE)

$(EXE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXE) $(LIBS) $(LDFLAGS)

.PHONY: clean
clean:
	@rm -f $(EXE) $(OBJECTS) *~

help:
	@echo 'SOURCES:'
	@echo $(SRCS)
	@echo 'OBJECTS:'
	@echo $(OBJECTS)