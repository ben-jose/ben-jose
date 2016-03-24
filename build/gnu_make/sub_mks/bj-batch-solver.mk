
TARGET := bj-batch-solver

SRC_BASE_DIR := ../../../src

TGT_LDFLAGS := -rdynamic -L${TARGET_DIR}

TGT_LDLIBS  := -lben-jose -lstdc++ -lgmpxx -lgmp 
TGT_PREREQS := libben-jose.a 

TGT_POSTMAKE := echo "Finished building DEBUG program bj-batch-solver"

SOURCES := \
	$(SRC_BASE_DIR)/programs/tests/solver/batch_solver.cpp \
	$(SRC_BASE_DIR)/programs/tests/solver/solver_main.cpp

SRC_INCDIRS := \
	$(SRC_BASE_DIR)/library/api \
	$(SRC_BASE_DIR)/external \
	$(SRC_BASE_DIR)/utils 

