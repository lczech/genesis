# --------------------------------
#   Program
# --------------------------------

PROGRAM = genesis

# TODO
# introduce a target distrib that copies all relevant files to an archive or so
# introduce constants for DEBUG and RELEASE (latter bound to distrib)
# build into build folder with subfolders per arch/extension,
# put resulting binaries in bin folder

# --------------------------------
#   Compiler Options
# --------------------------------

# Debug & Profiling (comment out if not needed).
DBG     = -g -ggdb3 -pg -DDEBUG

# Warning flags.
WARN    = -Wall -Wextra -pedantic -pedantic-errors

# Compiler flags.
STDCC   = clang++
MPICC   = mpic++
CCFLAGS = -std=c++11 -I./src $(WARN) $(DBG)
LDFLAGS = -lm

# --------------------------------
#   File lists
# --------------------------------

# Source files are named .cc, headers .hh. If a source file
# ends in .inc.cc, it is not directly compiled, because we
# assume it is included somewhere and compilation is done there.
# For example, all implementation code of template files is
# outsourced to .inc.cc files.

SRCFILES := $(shell find ./src -type f -name "*.cc")
HDRFILES := $(shell find ./src -type f -name "*.hh")
OBJFILES  = $(patsubst %.cc,%.o,$(SRCFILES))
OBJFILES := $(OBJFILES:%.inc.o=)
DEPFILES := $(patsubst %.cc,%.d,$(SRCFILES))
ALLFILES := $(SRCFILES) $(HDRFILES)

# --------------------------------
#   Make rules
# --------------------------------

.PHONY: all clean dist check test todo

# Build the standard version of the program.
all: CC = ${STDCC}
all: $(PROGRAM)
	@echo "\n========== Done std  =========="

# Build an MPI version of the program.
mpi: CC = ${MPICC}
mpi: CCFLAGS += -DUSE_MPI
mpi: $(PROGRAM)
	@echo "\n========== Done mpi  =========="

# Link all objects to get the program.
$(PROGRAM): $(OBJFILES) $(HDRFILES)
	@echo "\n========== Linking   =========="
	@echo "Objects: $(OBJFILES)\n"
	$(CC) $(OBJFILES) -o $@ $(LDFLAGS)

# Compile all sources and create dependency files (-MMD -MP).
%.o: %.cc
	@echo "\n========== Compiling =========="
	@echo "File: $< > $@"
	$(CC) $(CCFLAGS) -MMD -MP -c $< -o $@

# Include the dependecies files.
# They are generated when compiling the sources and contain makefile-formatted
# information about which source needs which headers. This way, all dependant
# objects are recompiled when a header changes. Awesome!
-include $(DEPFILES)

# Remove all generated files.
clean:
	@echo "\n========== Cleaning  =========="
	-@$(RM) $(PROGRAM) $(OBJFILES) $(DEPFILES) core

# Extract todo's and fixme's.
todo:
	@echo "\n========== Todos     =========="
	-@$(RM) TODO
	-@for file in $(ALLFILES:Makefile=); do fgrep -Hni -e TODO -e FIXME $$file \
	| sed "s/[[:space:]]*\/[\/\*]*[[:space:]]*\(TODO\|FIXME\)[[:space:]]*"\
	"/      /gI" >> TODO; done; true
