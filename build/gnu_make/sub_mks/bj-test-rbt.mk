
TARGET := bj-test-rbt

SRC_BASE_DIR := ../../../src

TGT_LDFLAGS := -L${TARGET_DIR}

TGT_LDLIBS  := -lben-jose -lstdc++ -lgmpxx -lgmp 

TGT_PREREQS := libben-jose.a 

SOURCES := $(SRC_BASE_DIR)/sat_tools/tests/test_rbt.cpp

SRC_INCDIRS := \
	$(SRC_BASE_DIR)/containers \
	$(SRC_BASE_DIR)/utils 


