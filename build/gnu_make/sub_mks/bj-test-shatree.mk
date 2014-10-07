
TARGET := bj-test-shatree

SRC_BASE_DIR := ../../../src

TGT_LDFLAGS := -L${TARGET_DIR}

TGT_LDLIBS  := -lben-jose -lstdc++ -lgmpxx -lgmp 

SOURCES := $(SRC_BASE_DIR)/sat_tools/tests/test_shatree.cpp

TGT_PREREQS := libben-jose.a 

SRC_INCDIRS := \
	$(SRC_BASE_DIR)/sat_tools/brain \
	$(SRC_BASE_DIR)/sat_tools/debug \
	$(SRC_BASE_DIR)/sat_tools/dimacs \
	$(SRC_BASE_DIR)/sat_tools/solver \
	$(SRC_BASE_DIR)/sat_tools/unsat_db \
	$(SRC_BASE_DIR)/containers \
	$(SRC_BASE_DIR)/utils 



