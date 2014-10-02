
TARGET := bj-test

SRC_BASE_DIR := ../../../src

TGT_LDFLAGS := -L${TARGET_DIR}

TGT_LDLIBS  := -lben-jose -lstdc++ -lgmpxx -lgmp 
TGT_PREREQS := libben-jose.a 

SOURCES := $(SRC_BASE_DIR)/sat_tools/solver/test_main.cpp

SRC_INCDIRS := \
	$(SRC_BASE_DIR)/sat_tools/brain \
	$(SRC_BASE_DIR)/sat_tools/debug \
	$(SRC_BASE_DIR)/sat_tools/dimacs \
	$(SRC_BASE_DIR)/sat_tools/solver \
	$(SRC_BASE_DIR)/sat_tools/unsat_db \
	$(SRC_BASE_DIR)/containers \
	$(SRC_BASE_DIR)/crypto \
	$(SRC_BASE_DIR)/utils 


