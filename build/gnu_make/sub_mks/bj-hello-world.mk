
TARGET := bj-hello-world

SRC_BASE_DIR := ../../../src

#TGT_CXXFLAGS := $(NORMAL_DBG_FLAGS) 

TGT_LDFLAGS := -L${TARGET_DIR}

TGT_LDLIBS  := -lben-jose -lstdc++ -lgmpxx -lgmp 
#TGT_LDLIBS  := -ldbg-ben-jose -lstdc++ -lgmpxx -lgmp 

TGT_PREREQS := libben-jose.a 
#TGT_PREREQS := libdbg-ben-jose.a 

TGT_POSTMAKE := echo "Finished building EXAMPLE program bj-hello-world"

SOURCES := $(SRC_BASE_DIR)/programs/bj-hello-world/bj-hello-world.c

SRC_CFLAGS := -Wall -std=c99

SRC_INCDIRS := $(SRC_BASE_DIR)/library/api

