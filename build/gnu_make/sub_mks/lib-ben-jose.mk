
TARGET := libben-jose.a

SRC_BASE_DIR := ../../../src

POSTMK_JS_DIR := ../../src/library/unsat_db/javascript
POSTMK_HTM_DIR := ../../src/library/unsat_db/html

#	du -a $(TARGET_DIR)/draw_cnf_js_lib
#	du -a $(TARGET_DIR)/show_proof_js_lib

define COPY_JS_LIBS
	rm -rf $(TARGET_DIR)/draw_cnf_js_lib
	rm -rf $(TARGET_DIR)/show_proof_js_lib
	rm -rf $(TARGET_DIR)/html
	cp -r $(POSTMK_JS_DIR)/draw_cnf_js_lib $(TARGET_DIR)/draw_cnf_js_lib
	cp -r $(POSTMK_JS_DIR)/show_proof_js_lib $(TARGET_DIR)/show_proof_js_lib
	cp -r $(POSTMK_HTM_DIR) $(TARGET_DIR)/html
	echo "Finished building libben-jose.a"
endef

TGT_POSTMAKE := ${COPY_JS_LIBS}

SOURCES := \
	$(SRC_BASE_DIR)/external/bj_mem.cpp \
	$(SRC_BASE_DIR)/external/bj_stream.cpp \
	$(SRC_BASE_DIR)/external/stack_trace.cpp \
	$(SRC_BASE_DIR)/external/top_exception.cpp \
	$(SRC_BASE_DIR)/library/api/ben_jose.cpp \
	$(SRC_BASE_DIR)/library/brain/brain.cpp \
	$(SRC_BASE_DIR)/library/brain/deducer.cpp \
	$(SRC_BASE_DIR)/library/brain/sortor.cpp \
	$(SRC_BASE_DIR)/library/brain/neuromap.cpp \
	$(SRC_BASE_DIR)/library/dimacs/dimacs.cpp \
	$(SRC_BASE_DIR)/library/shuffler/shuffler.cpp \
	$(SRC_BASE_DIR)/library/unsat_db/skeleton.cpp \
	$(SRC_BASE_DIR)/library/unsat_db/proof.cpp \
	$(SRC_BASE_DIR)/library/debug/dbg_config.cpp \
	$(SRC_BASE_DIR)/library/debug/dbg_brain.cpp \
	$(SRC_BASE_DIR)/library/debug/dbg_cy_htm.cpp \
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


