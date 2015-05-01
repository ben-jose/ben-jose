
TARGET := bj-solver

SRC_BASE_DIR := ../../../src

TGT_LDFLAGS := -rdynamic -L${TARGET_DIR}

TGT_LDLIBS  := -lben-jose -lstdc++ -lgmpxx -lgmp 
TGT_PREREQS := libben-jose.a 

SOURCES := \
	$(SRC_BASE_DIR)/programs/solver/batch_solver.cpp \
	$(SRC_BASE_DIR)/programs/solver/solver_main.cpp

SRC_INCDIRS := \
	$(SRC_BASE_DIR)/library/api \
	$(SRC_BASE_DIR)/external \
	$(SRC_BASE_DIR)/utils 

