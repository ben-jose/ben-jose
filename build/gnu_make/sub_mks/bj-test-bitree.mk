
TARGET := bj-test-bitree

SRC_BASE_DIR := ../../../src

TGT_LDFLAGS := -L${TARGET_DIR}

TGT_LDLIBS  := -lben-jose -lstdc++ -lgmpxx -lgmp 

SOURCES := $(SRC_BASE_DIR)/sat_tools/tests/test_bitree.cpp

TGT_PREREQS := libben-jose.a 

SRC_INCDIRS := \
	$(SRC_BASE_DIR)/containers \
	$(SRC_BASE_DIR)/utils 



