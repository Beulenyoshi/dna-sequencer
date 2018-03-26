#
#  makefile
#
#  Created by Thomas Johannesmeyer (thomas@geeky.gent) on 30.11.2017.
#  Copyright (c) 2017 www.geeky.gent. All rights reserved.
#


# Set-up the compiler to use
CXX = clang++

# Set-up the basic compiler flags
CXX_FLAGS = -std=c++14                   # set-up the C++ standard to use
CXX_FLAGS += -Wall                       # enable compiler warnings
CXX_FLAGS += -Wextra                     # enable some more warnings
CXX_FLAGS += -MMD                        # header file detection stuff
CXX_FLAGS += -MP                         # header file detection stuff
CXX_FLAGS += -stdlib=libc++               # libc++ for GCC, libc++ for Clang
CXX_FLAGS += -Ofast                      # use all compiler optimizations that are possible
CXX_FLAGS += -march=native               # compile the code for this very architecture
CXX_FLAGS += -Wno-unknown-warning-option # ignore unknown warnings (as '-Wno-maybe-uninitialized' resulting from internal bugs)
CXX_FLAGS += -Qunused-arguments          # ignore unused compiler arguments
CXX_FLAGS += -pipe                       # do not use temporary files, use pipes instead
#CXX_FLAGS += -g # compile debug symbols into the program (use when debugging only, and disable optimizations before)

# Define makefunction
recwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call recwildcard,$d/,$2))

# Name of the executable
EXE = main

# Basic project directories
BIN = bin/
OBJDIR = obj/
DOC = doc/
SRC = src/
ALL_SRC = $(sort $(dir $(call recwildcard,$(SRC)**/*/)))

# Set the virtual (search) path
VPATH = $(SRC):$(ALL_SRC)

# Determine all object files that we need in order to produce an executable
OBJ = $(addprefix $(OBJDIR),$(notdir $(patsubst %.cpp,%.o,$(call recwildcard,src/,*.cpp))))

# To determine source (.cpp) and header (.hh) dependencies
DEP = $(OBJ:.o=.d)

# Important scripts and binaries to use
AUTOFORMAT_SCRIPT := misc/format_src_code.py

# Thread model to use
THREAD_MODEL := -pthread

# Libraries to link against
MATH_LIBS := -lm

.PHONY: clean

all: $(OBJDIR) $(BIN) $(BIN)$(EXE)

# To resolve header dependencies
-include $(DEP)

# Create object directory
$(OBJDIR):
	mkdir $@

# Create binary directory
$(BIN):
	mkdir $@

# Link all object code (against additional libraries) to produce the machine code
$(BIN)$(EXE): $(OBJ)
	$(CXX) $(CXX_FLAGS) $^ $(MATH_LIBS) -o $@ $(THREAD_MODEL)
	@echo "done"

# Compile source code to object code
$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_INCL) $(LLVM_FLAGS) -c $< -o $@

# Create the code documentation
doc:
	@echo "Building the documentation of the source code..."
	cd $(SRC); \
	doxygen doxy_config.conf

# Format the source code of this project
code-format:
	@echo "Formatting the projects source code using clang-format..."
	python3 $(AUTOFORMAT_SCRIPT)

# Clean all auto-generated stuff
clean:
	rm -rf $(BIN)
	rm -rf $(OBJDIR)
	rm -rf $(DOC)
