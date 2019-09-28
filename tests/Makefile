CLANG = clang++

SRC_DIR ?= test_cases
OUT_DIR ?= outputs
FLAGS = -cc1 -fsyntax-only -fobjc-arc -Wno-objc-root-class -std=c++11 -ast-dump

all:
	$(CLANG) $(FLAGS) $(SRC_DIR)/* 
