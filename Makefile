# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2016 Lucas Czech
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Contact:
# Lucas Czech <lucas.czech@h-its.org>
# Exelixis Lab, Heidelberg Institute for Theoretical Studies
# Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany

# --------------------------------------------------------------------------------------------------
#     This makefile wraps around cmake in order to make the use straight forward.
#     A simple call to "make" suffices to build the whole of genesis.
#
#     This script is mainly intended for fast development, as it 'misuses' cmake
#     directly as a build system instead of a build system _generator_.
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
update:
	@echo "Running make with new files..."
	@touch lib/CMakeLists.txt
	@touch apps/CMakeLists.txt
	@touch python/src/CMakeLists.txt
	@touch test/src/CMakeLists.txt
	$(MAKE) -s -C build
.PHONY: update

# Clean up all build targets.
clean:
	@echo "Running clean..."
	@rm -rf bin
	@rm -rf build
	@rm -rf python/module
	@rm -rf test/bin
.PHONY: clean
