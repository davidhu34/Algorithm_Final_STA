# Compiler.
CC := g++

# Version can be `utest` (default), `dbg` or `rel`.
ver := utest

# Preprocessor flags.
PFLAGS := -I.. -Ilib -Wall

# Compiler flags.
CFLAGS := -Wall -Wextra

# Linker flags.
LFLAGS := 

ifeq ($(ver),rel) # Release version.
    $(info Building release version.)

    # Directories.
    MODULES := src/main src/util src/cir src/ana
    LIB_DIR := lib/minisat_blbd/bin
    LIB     := libminisat_blbd_rel.a

    PFLAGS += -DNDEBUG 
    CFLAGS += -O3
    LFLAGS += 

    # Output file.
    OUTPUT := bin/sta

    # Version extension.
    VEXT := _rel

else ifeq ($(ver),dbg) # Debug version.
    $(info Building debug version.)

    # Directories.
    MODULES := src/main src/util src/cir src/ana
    LIB_DIR := lib/minisat_blbd/bin
    LIB     := libminisat_blbd_dbg.a

    PFLAGS += 
    CFLAGS += -O0 -g
    LFLAGS += 

    # Output file.
    OUTPUT := bin/sta

    # Version extension.
    VEXT := _dbg

else ifeq ($(ver),utest) # Unit test version.
    $(info Building unit test.)

    # Directories.
    MODULES := src/util src/cir test/src test/src/util
    LIB_DIR := test/lib lib/minisat_blbd/bin
    LIB     := libminisat_blbd_dbg.a

    PFLAGS += 
    CFLAGS += -O0 -g
    LFLAGS += 

    # Output file.
    OUTPUT := bin/unit_test

    # Version extension.
    VEXT := _dbg

else 
    # Mode not recognized.
    $(error Mode $(ver) not recognized!)

endif

LFLAGS += $(patsubst %,-L%,$(LIB_DIR)) $(patsubst lib%.a,-l%,$(LIB))

# Archiver.
AR := ar

# Archiver flags.
ARFLAGS := rcvs

# Each modules' file will add to these variables
SRC :=

# Include subdirectories' makefile.
include $(patsubst %,%/module.mk,$(MODULES))

# Modify object file name to include `_<version_extension>` before
# `.o` extension.
OBJ := $(patsubst %.cpp,%$(VEXT).o,$(SRC))

# Pattern rules.
%$(VEXT).o: %.cpp
	@echo "Compiling $<..."
	@$(CC) -c $< $(PFLAGS) $(CFLAGS) -o $*$(VEXT).o

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
%$(VEXT).d: %.cpp
	@echo "Building dependency for $<..."
	@$(CC) $(PFLAGS) -MM $*.cpp > $*$(VEXT).tempd
	@sed -e 's|.*\.o:|$*$(VEXT).d $*$(VEXT).o:|' \
	 < $*$(VEXT).tempd > $*$(VEXT).d
	@sed -e 's|.*:||' -e 's|\\$$||g' < $*$(VEXT).tempd | fmt -1 | \
	 sed -e 's|^ *||' -e 's|$$|:|' >> $*$(VEXT).d
	@rm $*$(VEXT).tempd

# Phony target.
.PHONY: clean all

# Link the program
all: $(OUTPUT)

$(OUTPUT): $(OBJ)
	@echo "Linking..."
	@$(CC) -o $@ $(OBJ) $(LFLAGS)
	@echo $(OUTPUT)" build succeed."
ifeq ($(ver),utest)
	@echo
	@echo "Start unit testing..."
	@-$(OUTPUT)
endif

# Include `.d` file only if target is not `clean`.
ifneq ($(MAKECMDGOALS),clean)

# Include dependency (rule) description files.
-include $(patsubst %.cpp,%$(VEXT).d,$(SRC))

endif

# clean all the .o and executable files
clean:
	@-for D in $(MODULES); do \
	   rm $$D/*.o $$D/*.d ;\
	 done; \
	 rm $(OUTPUT)

