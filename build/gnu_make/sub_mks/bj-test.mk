
TARGET := bj-test

SRC_BASE_DIR := ../../../src

TGT_LDFLAGS := -L${TARGET_DIR}

TGT_CXXFLAGS := $(NORMAL_DBG_FLAGS) 

TGT_LDLIBS  := -ldbg-ben-jose -lstdc++ -lgmpxx -lgmp 
TGT_PREREQS := libdbg-ben-jose.a 

TGT_POSTMAKE := echo "Finished building DEBUG program bj-test"

SOURCES := \
	$(SRC_BASE_DIR)/programs/tests/test.cpp \
	$(SRC_BASE_DIR)/programs/tests/test_main.cpp

SRC_INCDIRS := \
	$(SRC_BASE_DIR)/library/brain \
	$(SRC_BASE_DIR)/library/debug \
	$(SRC_BASE_DIR)/library/dimacs \
	$(SRC_BASE_DIR)/library/api \
	$(SRC_BASE_DIR)/library/unsat_db \
	$(SRC_BASE_DIR)/external \
	$(SRC_BASE_DIR)/utils 


