# from Makefile to here, no trailing space or slash
DIR := src/main

# Test file that you don't want to compile.
EXCLUDE := 

SRC += $(filter-out $(patsubst %,$(DIR)/%,$(EXCLUDE)),$(wildcard $(DIR)/*.cpp))
