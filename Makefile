# ------------------------------------------------------------------------------
# This makefile wraps around cmake in order to make the use straight forward.
# A simple call to "make" suffices to build the whole of genesis.
# ------------------------------------------------------------------------------

# Run everything: first we need cmake, then make.
all: build/CMakeCache.txt make
	@echo "Done."
.PHONY: all

# Run cmake if not yet done or if CMakeLists.txt has changed.
build/CMakeCache.txt: CMakeLists.txt
	@echo "Running cmake..."
	@mkdir -p build
	@cd build && cmake ..

# Run make, but not before CMakeCache.txt is created.
# We first touch all inner cmake files so that their glob search for files is rerun.
# This ensures that all new files are compiled, even when doing incremental builds.
make: build/CMakeCache.txt
	@echo "Running make..."
	@touch lib/CMakeLists.txt
	@touch python/src/CMakeLists.txt
	@touch test/src/CMakeLists.txt
	$(MAKE) -s -C build
.PHONY: make

# Clean up all build targets.
clean:
	@echo "Running clean..."
	@rm -rf bin
	@rm -rf build
	@rm -rf python/module
	@rm -rf test/bin
.PHONY: clean
