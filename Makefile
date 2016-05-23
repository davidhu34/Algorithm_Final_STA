# Compiler.
CC := g++

# Mode can be `debug` (default) or `release`.
mode := debug

ifeq ($(mode),release) # Release version.
    $(info Building release version.)

    # Directories.
    MODULES := src/main src/util
    LIB_DIR := lib

    # Preprocessor flags
    PFLAGS := -Isrc

    # Compiler flags.
    CFLAGS := -Wall -Wextra -Werror=return-type -DNDEBUG -O3

    # Linker flags.
    LFLAGS := $(patsubst %,-L%,$(LIB_DIR))

    # Output file.
    OUTPUT := bin/sta

else ifeq ($(mode),debug) # Debug version.
    $(info Building debug version.)

    # Directories
    MODULES := src/util src/cir test/src test/src/util
    LIB_DIR := lib test/lib

    # Preprocessor flags
    PFLAGS := -Isrc -I.

    # Compiler flags.
    CFLAGS := -Wall -Wextra -Werror=return-type -O0 -g

    # Linker flags.
    LFLAGS := $(patsubst %,-L%,$(LIB_DIR))

    # Output file.
    OUTPUT := bin/unit_test

else 
    # Mode not recognized.
    $(error Mode $(mode) not recognized!)

endif

# Archiver.
AR := ar

# Archiver flags.
ARFLAGS := rcv

# Each modules' file will add to these variables
SRC :=
LIB :=

# Include subdirectories' makefile.
include $(patsubst %,%/module.mk,$(MODULES))

# Determine the object files.
ifeq ($(mode),release)
    # Modify object file name to include `_rel` before `.o` extension.
    OBJ := $(patsubst %.cpp,%_rel.o,$(SRC))
else
    # Modify object file name to include `_dbg` before `.o` extension.
    OBJ := $(patsubst %.cpp,%_dbg.o,$(SRC))
endif

LFLAGS += $(patsubst lib%,-l%,$(LIB))

# Pattern rules.
%_rel.o: %.cpp
	@echo "Compiling $<..."
	@$(CC) -c $< $(PFLAGS) $(CFLAGS) -o $*_rel.o

%_dbg.o: %.cpp
	@echo "Compiling $<..."
	@$(CC) -c $< $(PFLAGS) $(CFLAGS) -o $*_dbg.o

# Calculate Cpp include dependencies.
#
# --------  -------------------------------------------------
# `sed   `  "file.o:" -> "dir/file.d dir/file.o:"
# `sed   `  Remove "file.o:".
# `sed   `  Remove targets and colon.
# `sed   `  Remove backslash at the end of line.
# `fmt -1`  Print word by word into lines.
# `sed   `  Remove space at beginning of line
# `sed   `  Add colon at end of line.
# --------  -------------------------------------------------
#
%.d: %.cpp
	@echo "Building dependency for $<..."
	@$(CC) $(PFLAGS) -MM $*.cpp > $*.tempd
	@sed -e 's|.*\.o:|$*.d $*.o:|' < $*.tempd > $*.d
	@sed -e 's|.*:||' -e 's|\\$$||g' < $*.tempd | fmt -1 | \
	 sed -e 's|^ *||' -e 's|$$|:|' >> $*.d
	@rm $*.tempd

# Phony target.
.PHONY: clean all

# Link the program
all: $(OUTPUT)

$(OUTPUT): $(OBJ)
	@echo "Linking..."
	@$(CC) -o $@ $(OBJ) $(LFLAGS)
	@echo $(OUTPUT)" build succeed."
ifeq ($(mode),debug)
	@echo
	@echo "Start unit testing..."
	@-$(OUTPUT)
endif

# Include `.d` file only if target is not `clean`.
ifneq ($(.DEFAULT_GOAL),clean)

# Include dependency (rule) description files.
-include $(patsubst %.cpp,%.d,$(SRC))

endif

# clean all the .o and executable files
clean:
	@for D in $(MODULES); do \
	   rm $$D/*.o $$D/*.d ;\
	 done; \
	 rm $(OUTPUT)

