# --------------------------------------------------------------------------------------------------
#     This makefile wraps around cmake in order to make the use straight forward.
#     A simple call to "make" suffices to build the whole of genesis.
#
#     This script is mainly intended for fast development, as it 'misuses' cmake
#     directly as a build system instead of a build system generator.
# --------------------------------------------------------------------------------------------------

# Run everything by specifying the cmake file and the build command as dependencies.
all: build/CMakeCache.txt build
	@echo "Done."
.PHONY: all

# Run cmake if not yet done or if CMakeLists.txt has changed.
build/CMakeCache.txt: CMakeLists.txt
	@echo "Running cmake..."
	@mkdir -p build
	@cd build && cmake ..

# Run make. State cmake as dependency, to ensure correct order.
build: build/CMakeCache.txt
	@echo "Running make..."
	$(MAKE) -s -C build
.PHONY: build

# Special make that also includes new files.
# We first touch all inner cmake files so that their glob search for files is rerun.
# This ensures that all new files are compiled, even when doing incremental builds.
new_files:
	@echo "Running make with new files..."
	@touch lib/CMakeLists.txt
	@touch python/src/CMakeLists.txt
	@touch test/src/CMakeLists.txt
	$(MAKE) -s -C build
.PHONY: new_files

# Clean up all build targets.
clean:
	@echo "Running clean..."
	@rm -rf bin
	@rm -rf build
	@rm -rf python/module
	@rm -rf test/bin
.PHONY: clean
