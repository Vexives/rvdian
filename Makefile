# All files
CSOURCES = audio_wrapper.c array_utils.c complex_numbers.c frame_processes.c window_funcs.c wrapper_utils.c
CXXSOURCES = main.cpp

# Executable
EXE = rvdian
RM = rm -f
RMW = -del

# Flags
CFLAGS = -Wall -Wextra -pg
CXXFLAGS = -Wall -Wextra -pg -std=c++17
LDFLAGS = -lm

# Compiler
CC = gcc
CXX = g++

# Libraries
LIBS = 

# Source Path
DIRS = src
OBJDIR = bin
SEARCHC = $(addsuffix /*.c,$(DIRS))
SEARCHCXX = $(addsuffix /*.cpp,$(DIRS))
SRCS = $(wildcard $(SEARCHC))
SRCSXX = $(wildcard $(SEARCHCXX))

$(OBJDIR)/%.o: $(DIRS)%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(DIRS)%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Object files
COBJECTS = $(SRCS:$(DIRS)%.c=$(OBJDIR)%.o)
CXXOBJECTS = $(SRCSXX:$(DIRS)%.cpp=$(OBJDIR)%.o)

default: all	
all: $(EXE)

$(COBJECTS): $(OBJDIR)/%.o: $(DIRS)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(CXXOBJECTS): $(OBJDIR)/%.o: $(DIRS)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXE): $(COBJECTS) $(CXXOBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(COBJECTS) $(CXXOBJECTS) -o $(EXE) $(LIBS)

.PHONY: clean
clean:
		$(RM) $(EXE) $(OBJECTS) gmon.out *~

cleanexe:
		$(RM) $(EXE) gmon.out *~

cleanobj:
		$(RM) $(OBJECTS) gmon.out *~

clean_win:
		$(RMW) $(OBJDIR)\*.o $(EXE).exe gmon.out

cleanexe_win:
		$(RMW) $(EXE).exe gmon.out

cleanobj_win:
		$(RMW) $(OBJDIR)\*.o gmon.out

help:
	@echo 'SOURCES:'
	@echo $(SRCS)
	@echo 'OBJECTS:'
	@echo $(COBJECTS) $(CXXOBJECTS)