#
# Copyright (c) 2012-2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
VALGRIND:=--memcheck="valgrind --leak-check=full --error-exitcode=1"
DRMEMORY:=--memcheck="drmemory -light -batch -exit_code_if_errors 1"
BS?=cmake
TOOLSET?=clang
CLANG_FORMAT?=clang-format
CLANG_TIDY?=clang-tidy
PYTHON?=python
MKDOCS?=mkdocs
GENERATOR?="Unix Makefiles"

.PHONY: all clean doc

all: all_$(BS)

all_bjam:
	@cd test && bjam -j2 -q --toolset=$(TOOLSET) --user-config=../.user-config.jam debug-symbols=off $(VARIANT) $($(MEMCHECK)) cxxflags=" $(CXXFLAGS)" linkflags=" $(LDFLAGS)"
	@cd example && bjam -j2 -q --toolset=$(TOOLSET) --user-config=../.user-config.jam debug-symbols=off $($(MEMCHECK)) cxxflags=" $(CXXFLAGS)" linkflags=" $(LDFLAGS)"
	@cd extension && bjam -j2 -q --toolset=$(TOOLSET) --user-config=../.user-config.jam debug-symbols=off $($(MEMCHECK)) cxxflags=" $(CXXFLAGS)" linkflags=" $(LDFLAGS)"

all_cmake:
	@-mkdir build
	@cd build && cmake .. && cmake --build . && ctest --output-on-failure

clean: clean_$(BS)

clean_bjam:
	@bjam --clean

clean_cmake:
	@rm -rf build

pph:
	@tools/pph.sh

check: check_pph check_style

check_pph: pph
	@git diff include/boost/di.hpp
	@git diff --quiet include/boost/di.hpp

check_style:
	@find include example extension test -iname "*.hpp" -or -iname "*.cpp" | xargs $(CLANG_FORMAT) -i
	@git diff include example extension test
	@exit `git ls-files -m include example extension test | wc -l`

check_static_analysis:
	@$(CLANG_TIDY) -header-filter='boost/di' `find example extension test -type f -iname "*.cpp"` -- -std=c++1y -I include -I test -include common/test.hpp

doc:
	cd doc && $(MKDOCS) build --clean && $(PYTHON) boost/scripts/update_readme_toc.py mkdocs.yml ../README.md http://boost-experimental.github.io/di

release: all check
