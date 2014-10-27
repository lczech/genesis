# --------------------------------
#   Program
# --------------------------------

PROG = genesis

# --------------------------------
#   Compiler Options
# --------------------------------

# Debug & Profiling (comment out if not needed)
# TODO set log level
DBG     = -g -pg

# Warning flags
WARN    = -Wall -Wextra -pedantic-errors

# Compiler flags
STDCC   = g++
MPICC   = mpic++
CCFLAGS = -std=c++11 -O1 $(WARN) $(DBG)
LDFLAGS = -lm
#-lpll-sse3 -lm

# --------------------------------
#   Make rules
# --------------------------------

# Collect all files
SRCS = $(wildcard src/*.cc) $(wildcard src/*/*.cc)
OBJS = $(SRCS:.cc=.o)

# Target for non-mpi version
all: CC = ${STDCC}
all: $(PROG)
	@echo "\n========== Done std  =========="

# Target for mpi version
mpi: CC = ${MPICC}
mpi: CCFLAGS += -DUSE_MPI
mpi: $(PROG)
	@echo "\n========== Done mpi  =========="

# Target for Linking
$(PROG): $(OBJS)
	@echo "\n==========  Linking  =========="
	@echo "Objects: $(OBJS)\n"
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Target for Compiling
.cc.o:
	@echo "\n========== Compiling =========="
	@echo "File: $< > $@"
#	@echo "Sources: $(SRCS)"
	$(CC) $(CCFLAGS) -c $< -o $@

# Target for Cleaning
clean:
	@echo "\n========== Cleaning  =========="
	rm -fv $(OBJS) $(PROG)
