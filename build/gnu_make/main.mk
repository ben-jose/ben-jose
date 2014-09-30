

TARGET_DIR := bin

# CXXFLAGS := -g -O0 -Wall -pipe
# DBG_FLAGS = -DFULL_DEBUG -rdynamic  

CXXFLAGS := -DFULL_DEBUG -rdynamic -Wall 

SUBMAKEFILES := sub_mks/lib-ben-jose.mk sub_mks/ben-jose-solver.mk

# $(VERBOSE).SILENT:

default: all

install: 
	@echo "Copy the files in ./bin to the desired install directories."

help: 
	@echo "See documentation in <base_dir>/docs."
