# from Makefile to here, no trailing space or slash
DIR := test/src

# Test file that you don't want to compile.
EXCLUDE := test_cir_circuit.cpp

SRC += $(filter-out $(patsubst %,$(DIR)/%,$(EXCLUDE)),$(wildcard $(DIR)/*.cpp))
LIB += 
