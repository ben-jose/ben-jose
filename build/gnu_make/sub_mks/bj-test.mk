
TARGET := bj-test

SRC_BASE_DIR := ../../../src

TGT_LDFLAGS := -L${TARGET_DIR}

TGT_LDLIBS  := -lben-jose -lstdc++ -lgmpxx -lgmp 
TGT_PREREQS := libben-jose.a 

SOURCES := $(SRC_BASE_DIR)/programs/tests/test_main.cpp

SRC_INCDIRS := \
	$(SRC_BASE_DIR)/programs/solver \
	$(SRC_BASE_DIR)/library/brain \
	$(SRC_BASE_DIR)/library/debug \
	$(SRC_BASE_DIR)/library/dimacs \
	$(SRC_BASE_DIR)/library/unsat_db \
	$(SRC_BASE_DIR)/external \
	$(SRC_BASE_DIR)/utils 


