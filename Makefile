# ### How to Work With This Makefile
# 
# Module is a directory that contains many source and header files.
# Use module to group closely related classes and functions.
# 
# Procedure for creating a new module:
# 
# 1.  Create a new directory under `src/`, e.g. `src/foo`.
# 
# 2.  Copy `src/util/module.mk` to `src/foo/`.
# 
# 3.  Modify the first line of `src/foo/module.mk`, change it to
#     `DIR := src/foo`.
# 
# 4.  Modify `Makefile`, add your module (`src/foo`) to the line start
#     with `MODULES := `. 
# 
# That's it. You can add whatever headers and sources in your new
# module. Please put all these functions and classes under namespace
# with the name same as your module, e.g. `Foo`.
# 
# To include a header file, always prefix it with its module name.
# E.g. `#include "foo/parser.h"`.
# 
# Any files under `src/main/` (e.g. main.cpp) won't be compiled while
# doing unit test. Testing has its own main function, so don't put
# files that you want to do unit test under `src/main/`.
# How to Use This Project Directory And Makefile

# Variables

# =  : Will do recursive expansion.
# := : Won't do recursive expansion.

# Compiler
CC := g++

# Compiler Flags
# -Wall               : Warn all possible mistakes.
# -Wextra             : Extra warning.
# -Werror=return-type : Turn "forget to return" warning to error.
# -O2                 : Optimization level 2 (0, 1, 2, 3).
# -std=<standard>     : Use this standard
#CFLAGS := -Wall -Wextra -Werror=return-type -DNDEBUG -O2

# Compiler flags for debugging
# -g        : Add information for debugger in that object file.
# -pg       : Add information for gnu profiler in that object file.
# -D<macro> : Tell preprocessor to define "macro".
CFLAGS := -Wall -Wextra -Werror=return-type -O0 -g

# Archiver to create static library
AR := ar  

# Archiver flags
# r (operation) : Insert files to archive, replace if files already exist.
# c (modifier)  : If archive doesn't exist, do not issue warning.
# v (modifier)  : Verbose.
# s (modifier)  : Write an object-file index into the archive.
ARFLAGS := rcv

OUTPUT := bin/sta

# Directories
MODULES := src/main src/util
LIB_DIR := lib

# Look for include files in each modules (directories)
INCLUDE := -Isrc
LIB_DIR := $(patsubst %,-L%,$(LIB_DIR))
CFLAGS += $(INCLUDE)

# Each modules' file will add to these variables
SRC :=
LIB :=

# Include the descriptor (file list) for each module
include $(patsubst %,%/module.mk,$(MODULES))

# Determine the object files
OBJ := $(patsubst %.cpp,%.o,$(SRC))
LIB := $(patsubst lib%,-l%,$(LIB))

# Pattern rules
# <target> : <dependency> ...
# .cpp depends on .o
# @<cmd> : Do not print this line of command, just execute it.
#
# $@    : Target (bin/NTU_sort).
# $<    : First prerequisites (sort_tool_opt.o).
# $?    : All prerequisites newer than target.
# $^    : All prerequisites.
# $*    : Match %, used in implicit rules only.
# $(^D) : Directories of all prerequisites.
#
# -c : Compile and create object file.
# -o : Output to file.
#
# $(filter pattern...,strings)
# - Return string that match any pattern.
# - E.g. $(filter %.c %.o,1.c 2.c 3.o 4.h 5.h) -> 1.c 2.c 3.o
#
# $(patsubst pattern,replacement,strings)
# - For each string, match with pattern, then do replacement.
# - E.g. $(patsubst %.d,%.c,a.d b.d c) -> a.c b.c c.c
%.o: %.cpp
	@echo "Compiling $<..."
	@$(CC) -c $< $(CFLAGS) -o $*.o

# Calculate Cpp include dependencies
#
# -MM   : Create dependencies.
#
# sed   : "file.o:" -> "dir/file.d dir/file.o:"
# sed   : remove "file.o:"
# sed   : remove backslash at end of line
# fmt -1: print word by word
# sed   : remove space at beginning of line
# sed   : add colon at end of line
%.d: %.cpp
	@echo "Building dependency for $<..."
	@$(CC) $(CFLAGS) $(INCLUDE) -MM $*.cpp > $*.tempd
	@sed -e 's|.*\.o:|$*.d $*.o:|' < $*.tempd > $*.d
	@sed -e 's|.*:||' -e 's|\\$$||g' < $*.tempd | fmt -1 | \
	 sed -e 's|^ *||' -e 's|$$|:|' >> $*.d
	@rm $*.tempd

# Phony target (not real file)
.PHONY: clean all

# Link the program
all: $(OUTPUT)

$(OUTPUT): $(OBJ)
	@echo "Linking..."
	@$(CC) -o $@ $(LIB_DIR) $(OBJ) $(LIB)
	@echo $(OUTPUT)" build succeed."

# Include dependencies
-include $(patsubst %.o,%.d,$(OBJ))

# clean all the .o and executable files
clean:
	@for D in $(MODULES); do \
	   rm $$D/*.o $$D/*.d ;\
	 done; \
	 rm $(OUTPUT)

