# this is example-file: Makefile.am

THE_LIB_NAME := libben-jose.a
THE_PRG_NAME := ben-jose-solver

GNU_MAKE_OUT_DIR = ../gnu_make/bin

bin_installdir = $(prefix)/bin
bin_install_DATA = $(GNU_MAKE_OUT_DIR)/$(THE_PRG_NAME)

lib_installdir = $(prefix)/lib
lib_install_DATA = $(GNU_MAKE_OUT_DIR)/$(THE_LIB_NAME)

$(GNU_MAKE_OUT_DIR)/$(THE_LIB_NAME): all-local
	@echo "$(THE_LIB_NAME) builded."

$(GNU_MAKE_OUT_DIR)/$(THE_PRG_NAME): all-local
	@echo "$(THE_PRG_NAME) builded."

all-local:
	cd ../gnu_make && $(MAKE) $(AM_MAKEFLAGS) all

clean-local:
	cd ../gnu_make && $(MAKE) $(AM_MAKEFLAGS) clean
 
