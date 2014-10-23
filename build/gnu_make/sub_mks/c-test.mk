
TARGET := c-test

SRC_BASE_DIR := ../../../src

TGT_LDFLAGS := -L${TARGET_DIR}

TGT_LDLIBS  := -lben-jose 
TGT_PREREQS := libben-jose.a 

SOURCES := $(SRC_BASE_DIR)/programs/tests/c_test.c

SRC_CFLAGS := -Wall -std=c99

SRC_INCDIRS := $(SRC_BASE_DIR)/library/interface


