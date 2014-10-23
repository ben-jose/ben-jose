

TARGET_DIR := bin

# CXXFLAGS := -g -O0 -Wall -pipe
# DBG_FLAGS = -DFULL_DEBUG -rdynamic  

CXXFLAGS := -DFULL_DEBUG -DDBG_GLB_MEM_USE -rdynamic -Wall 
# CXXFLAGS := -DFULL_DEBUG -rdynamic -Wall 
# CXXFLAGS := -rdynamic -Wall 

LDFLAGS := -rdynamic

SUBMAKEFILES := \
	sub_mks/lib-ben-jose.mk \
	sub_mks/ben-jose-solver.mk \
	sub_mks/bj-test-rbt.mk \
	sub_mks/c-test.mk \
	sub_mks/bj-test.mk

# $(VERBOSE).SILENT:

default: all

install: 
	@echo "Copy the files in ./bin to the desired install directories."

help: 
	@echo "See documentation in <base_dir>/docs."
