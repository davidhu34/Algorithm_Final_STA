# How to Use This Project Directory And Makefile
#
# To add a new module (directory that contain .cpp and .h files)
# - Create a new module (directory) under src/
# - Copy module.mk from src/util/ to src/your_module/
# - Add "src/your_module" to MODULES in Makefile
#
# To add a new .cpp or .h file
# - Just add it under any module
#
# Any files under src/main/ (e.g. main.cpp) won't be linked in Maketest
# - Testing has its own main function (test/src/main/main.cpp)
# - So don't put files that you want to do unit test under src/main/

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
# -std=standard       : Use this standard
CFLAGS := -Wall -Wextra -Werror=return-type -DNDEBUG -O2 -std=gnu++11

# Compiler flags for debugging
# -g        : Add information for debugger in that object file.
# -pg       : Add information for gnu profiler in that object file.
# -D<macro> : Tell preprocessor to define "macro".
CFLAGS := -Wall -Wextra -Werror=return-type -O0 -g -std=gnu++11

# Archiver to create static library
AR := ar  

# Archiver flags
# r (operation) : Insert files to archive, replace if files already exist.
# c (modifier)  : If archive doesn't exist, do not issue warning.
# v (modifier)  : Verbose.
# s (modifier)  : Write an object-file index into the archive.
ARFLAGS := rcv

OUTPUT := bin/compress

# Directories
MODULES := src/main src/util src/hc src/ac src/lz77 src/lzw src/bwt
LIB_DIR := lib

# Look for include files in each modules (directories)
INCLUDE := $(patsubst %,-I%,$(MODULES))
LIB_DIR := $(patsubst %,-L%,$(LIB_DIR))
CFLAGS += $(INCLUDE)

# Each modules' file will add to these variables
SRC :=
MAIN_SRC :=
LIB :=

# Include the descriptor (file list) for each module
include $(patsubst %,%/module.mk,$(MODULES))

# Determine the object files
OBJ := $(patsubst %.cpp,%.o,$(SRC))
MAIN_OBJ := $(patsubst %.cpp,%.o,$(MAIN_SRC))
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

$(OUTPUT): $(OBJ) $(MAIN_OBJ)
	@echo "Linking..."
	@$(CC) -o $@ $(LIB_DIR) $(OBJ) $(MAIN_OBJ) $(LIB)
	@echo $(OUTPUT)" build succeed."

# Include dependencies
-include $(patsubst %.o,%.d,$(OBJ))
-include $(patsubst %.o,%.d,$(MAIN_OBJ))

# clean all the .o and executable files
clean:
	@for D in $(MODULES); do \
	   rm $$D/*.o $$D/*.d ;\
	 done; \
	 rm $(OUTPUT)

