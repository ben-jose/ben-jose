
TARGET := libben-jose.a

SRC_BASE_DIR := ../../../src

SOURCES := \
	$(SRC_BASE_DIR)/external/bj_mem.cpp \
	$(SRC_BASE_DIR)/external/stack_trace.cpp \
	$(SRC_BASE_DIR)/external/top_exception.cpp \
	$(SRC_BASE_DIR)/library/brain/analyser.cpp \
	$(SRC_BASE_DIR)/library/brain/brain.cpp \
	$(SRC_BASE_DIR)/library/brain/sortor.cpp \
	$(SRC_BASE_DIR)/library/brain/neuromap.cpp \
	$(SRC_BASE_DIR)/library/dimacs/dimacs.cpp \
	$(SRC_BASE_DIR)/library/api/ben_jose.cpp \
	$(SRC_BASE_DIR)/library/shuffler/shuffler.cpp \
	$(SRC_BASE_DIR)/library/unsat_db/skeleton.cpp \
	$(SRC_BASE_DIR)/library/debug/dbg_config.cpp \
	$(SRC_BASE_DIR)/library/debug/dbg_brain.cpp \
	$(SRC_BASE_DIR)/library/debug/dbg_ic.cpp \
	$(SRC_BASE_DIR)/library/debug/dbg_run_satex.cpp \
	$(SRC_BASE_DIR)/library/debug/dbg_prt.cpp \
	$(SRC_BASE_DIR)/utils/parse_funcs.cpp \
	$(SRC_BASE_DIR)/utils/binder.cpp \
	$(SRC_BASE_DIR)/utils/file_funcs.cpp \
	$(SRC_BASE_DIR)/utils/util_funcs.cpp \
	$(SRC_BASE_DIR)/utils/tak_mak.cpp \
	$(SRC_BASE_DIR)/utils/sha2.cpp


SRC_INCDIRS := \
	$(SRC_BASE_DIR)/library/brain \
	$(SRC_BASE_DIR)/library/debug \
	$(SRC_BASE_DIR)/library/dimacs \
	$(SRC_BASE_DIR)/library/api \
	$(SRC_BASE_DIR)/library/unsat_db \
	$(SRC_BASE_DIR)/external \
	$(SRC_BASE_DIR)/utils 


