# All files
SOURCES = audio_wrapper.c array_utils.c complex_numbers.c frame_processes.c window_funcs.c wrapper_utils.c main.c

# Executable
EXE = rvdian
RM = rm -f
RMW = -del

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
		$(RM) $(EXE) $(OBJECTS) gmon.out *~

cleanexe:
		$(RM) $(EXE) gmon.out *~

cleanobj:
		$(RM) $(OBJECTS) *~

clean_win:
		$(RMW) $(OBJDIR)\*.o $(EXE).exe gmon.out

cleanexe_win:
		$(RMW) $(EXE).exe gmon.out

cleanobj_win:
		$(RMW) $(OBJDIR)\*.o

help:
	@echo 'SOURCES:'
	@echo $(SRCS)
	@echo 'OBJECTS:'
	@echo $(OBJECTS)