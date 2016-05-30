# from Makefile to here, no trailing space or slash
DIR := test/src/util

# Test file that you don't want to compile.
EXCLUDE := 

SRC += $(filter-out $(patsubst %,$(DIR)/%,$(EXCLUDE)),$(wildcard $(DIR)/*.cpp))
