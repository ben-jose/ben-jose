
TARGET := bj-solver-debug

SRC_BASE_DIR := ../../../src

TGT_CXXFLAGS := $(NORMAL_DBG_FLAGS) 

TGT_LDFLAGS := -rdynamic -L${TARGET_DIR}

TGT_LDLIBS  := -ldbg-ben-jose -lstdc++ -lgmpxx -lgmp 
TGT_PREREQS := libdbg-ben-jose.a 

TGT_POSTMAKE := echo "Finished building DEBUG program bj-solver-debug"

SOURCES := \
	$(SRC_BASE_DIR)/programs/tests/solver/batch_solver.cpp \
	$(SRC_BASE_DIR)/programs/tests/solver/solver_main.cpp

SRC_INCDIRS := \
	$(SRC_BASE_DIR)/library/api \
	$(SRC_BASE_DIR)/external \
	$(SRC_BASE_DIR)/utils 

