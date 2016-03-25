

TARGET_DIR := bin

# CXXFLAGS := -g -O0 -Wall -pipe
# DBG_FLAGS = -DFULL_DEBUG -rdynamic  

# NORMAL_DBG_FLAGS := -DFULL_DEBUG -DDBG_GLB_MEM_USE -rdynamic -Wall 
NORMAL_DBG_FLAGS := -DFULL_DEBUG -DDBG_GLB_MEM_USE
NORMAL_RUN_FLAGS := -rdynamic -Wall 

CXXFLAGS := $(NORMAL_RUN_FLAGS)
# CXXFLAGS := $(NORMAL_RUN_FLAGS) -DONLY_DEDUC_DBG
# CXXFLAGS := $(NORMAL_RUN_FLAGS) -DONLY_DEDUC_DBG -DKEEP_LEARNED_DBG
# CXXFLAGS := $(NORMAL_RUN_FLAGS) -DAS_DBG_WF

# CXXFLAGS := $(NORMAL_DBG_FLAGS) 
# CXXFLAGS := $(NORMAL_DBG_FLAGS) -DDBG_GLB_MEM_WITH_PT_DIR

LDFLAGS := -rdynamic

SUBMAKEFILES := \
	sub_mks/lib-ben-jose.mk \
	sub_mks/lib-dbg-ben-jose.mk \
	sub_mks/bj-solver-debug.mk \
	sub_mks/bj-batch-solver.mk \
	sub_mks/bj-test-sortor.mk \
	sub_mks/bj-phi-test.mk \
	sub_mks/bj-test.mk \
	sub_mks/bj-hello-world.mk 

# $(VERBOSE).SILENT:

default: all

install: 
	@echo "Copy the files in ./bin to the desired install directories."

help: 
	@echo "See documentation in <base_dir>/docs."
