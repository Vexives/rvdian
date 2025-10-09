# All files
SOURCES = audio_wrapper.c array_utils.c complex_numbers.c frame_processes.c window_funcs.c wrapper_utils.c main.c

# Executable
EXE = rvdian
RM = -del

# Flags
CFLAGS = -Wall -pg
LDFLAGS = -lm

# Compiler
CC = gcc

# Libraries
LIBS = 

# Source Path
DIRS = src
OBJDIR = bin
SEARCHC = $(addsuffix /*.c,$(DIRS))
SRCS = $(wildcard $(SEARCHC))

$(OBJDIR)/%.o: $(DIRS)%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Object files
OBJECTS = $(SRCS:$(DIRS)%.c=$(OBJDIR)%.o)

default: all	
all: $(EXE)

$(OBJECTS): $(OBJDIR)/%.o: $(DIRS)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(EXE) $(LIBS)

.PHONY: clean
clean:
		rm -f $(EXE) $(OBJECTS) *~

cleanexe:
		rm -f $(EXE) *~

cleanobj:
		rm -f $(OBJECTS) *~

clean_win:
		-del $(OBJDIR)\*.o $(EXE).exe

cleanexe_win:
		-del $(EXE).exe

cleanobj_win:
		-del $(OBJDIR)\*.o

help:
	@echo 'SOURCES:'
	@echo $(SRCS)
	@echo 'OBJECTS:'
	@echo $(OBJECTS)