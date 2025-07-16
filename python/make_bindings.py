#!/usr/bin/env python3

# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2023-2025 Giannis Reppas and Lucas Czech
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
# Lucas Czech <lucas.czech@sund.ku.dk>
# University of Copenhagen, Globe Institute, Section for GeoGenetics
# Oster Voldgade 5-7, 1350 Copenhagen K, Denmark

import os, sys
import filecmp
import glob
import pathlib
import re
import shutil
import subprocess
from typing import List, Set, Union
# from distutils.sysconfig import get_python_inc

# Change to the genesis main directory, so that this script can be called from anywhere.
# All file paths below are then relative to the main directory.
os.chdir(pathlib.Path(__file__).parent.parent.absolute())
print("In", os.getcwd())

# ------------------------------------------------------------------------------
#   Settings
# ------------------------------------------------------------------------------

# For ease of development, and to avoid hard-coded paths,
# we assume binder to be symlinked from the python directory as `binder`.

# Paths to binder, pybind, and python
binder_executable = './python/binder'
pybind_source     = './external/pybind11'
# python_directory  = '/usr/include/python3.12'

# Other include directories. Those might depend on the submodule setup,
# and hence is a bit brittle, but let's keep it for now.
other_includes = ['./build/generated/include']

# Paths to genesis code
genesis_source   = './lib'
genesis_includes = ['./python', './lib']

# Do we want to generate the all_includes.hpp file?
# We have deactivated this for now, as it does not seem to play nice with binder,
# messing up the paths so that some binding files end up in "unknown".
# Until we have figured out why, we instead add headers manually one by one,
# so that we have control over what is breaking when.
# When deactivate, we instead compare the retrieved list with the existing file,
# and report any headers that are missing, so that they can be added manually.
make_all_includes_file = False

# Do we want to skip the first n levels of the directories?
# This is needed to get binder to recognize the correct relative paths.
# If we include the `genesis.hpp` all-including header, somehow binder
# does not find the proper include paths and puts all bindings in "unknown".
# To avoid this, we skip the directory levels that include those headers.
min_directory_level = 2

# Do we want to read each c++ file and find its included headers?
# This will include system headers etc, and not needed.
parse_for_headers = False

# Python module
python_module_name = 'genesis'
namespace_to_bind = '"genesis"'

# Files needed for binder
all_includes_file = './python/all_includes.hpp'
config_file       = './python/binder.cfg'
bindings_dir      = './python/bindings'

# Settings for binder
c_plus_plus_version = '17'
use_pybind_stl  = True
single_file     = False
binder_debug    = False
binder_annotate_includes  = False
binder_annotate_functions = True

# ------------------------------------------------------------------------------
#   Sync Directories
# ------------------------------------------------------------------------------

def sync_dirs(src: str, dst: str) -> None:
    """
    Synchronize directory `dst` to match directory `src`:

      1. Delete anything in dst that is not in src.
      2. Copy anything in src that is not in dst.
      3. Overwrite in dst any files whose content differs from src.
      4. Leave untouched any files with identical content (so their timestamps stay the same).

    Args:
        src: Path to the “master” source directory.
        dst: Path to the target directory to be updated.

    Raises:
        FileNotFoundError: if src or dst do not exist.
        NotADirectoryError: if src or dst are not directories.
    """
    if not os.path.exists(src):
        raise FileNotFoundError(f"Source directory not found: {src}")
    if not os.path.isdir(src):
        raise NotADirectoryError(f"Source is not a directory: {src}")
    if not os.path.exists(dst):
        raise FileNotFoundError(f"Destination directory not found: {dst}")
    if not os.path.isdir(dst):
        raise NotADirectoryError(f"Destination is not a directory: {dst}")

    # Compare the two directories
    dc = filecmp.dircmp(dst, src)

    # 1) Remove files/dirs in dst not in src
    for name in dc.left_only:
        path = os.path.join(dst, name)
        if os.path.isdir(path) and not os.path.islink(path):
            shutil.rmtree(path)
        else:
            os.remove(path)

    # 2) Copy new files/dirs from src to dst
    for name in dc.right_only:
        src_path = os.path.join(src, name)
        dst_path = os.path.join(dst, name)
        if os.path.isdir(src_path):
            shutil.copytree(src_path, dst_path)
        else:
            shutil.copy(src_path, dst_path)

    # 3) Overwrite files that differ
    for name in dc.diff_files:
        src_path = os.path.join(src, name)
        dst_path = os.path.join(dst, name)
        shutil.copy(src_path, dst_path)

    # 4) Recurse into common subdirectories
    for subdir in dc.common_dirs:
        sync_dirs(
            src=os.path.join(src, subdir),
            dst=os.path.join(dst, subdir)
        )

# ------------------------------------------------------------------------------
#   Collect Headers
# ------------------------------------------------------------------------------

def find_included_headers(file: str, include_pattern):
    collected: Set[str] = set()
    with open(file, 'r', encoding='utf-8', errors='ignore') as f:
        for line in f:
            m = include_pattern.match(line)
            if m:
                # the mathc is '<sys/time.h>' or '"my_lib/foo.hpp"'
                inc = m.group(1)
                if not inc.startswith(('<','"')) or not inc.endswith(('>', '"')):
                    print("Wrong include type in ", file, ":", inc)
                collected.add(inc)
    return collected

def collect_all_headers(
    directories: Union[str, List[str]],
    min_depth: int = 0
) -> List[str]:
    """
    Scans one or more directories recursively for C/C++ header and source files,
    but only uses files whose path depth is greater than `min_depth`,
    and returns a sorted list of unique headers:
      - Header files in the directories (.h, .hpp, .hxx, .hh)
      - Headers referenced by #include directives, preserving delimiters.

    Parameters:
        directories (str or List[str]): Path(s) to root directory(ies) to scan.
        min_depth (int): Minimum relative path depth (number of segments minus one)
                         required to include a file.

    Returns:
        List[str]: Sorted list of unique header paths/names, e.g. '<vector>' or '"my_lib/foo.hpp"'.
    """
    dirs_to_scan = [directories] if isinstance(directories, str) else directories

    header_exts = ('.h', '.hpp', '.hxx', '.hh')
    source_exts = ('.c', '.cpp', '.cc', '.cxx', '.c++', '.cp')
    # Capture the delimiter and the header name, preserving '<...>' or "...".
    include_pattern = re.compile(r'^\s*#\s*include\s*([<"][^">]+[>"])')

    collected: Set[str] = set()
    for directory in dirs_to_scan:
        for root, _, files in os.walk(directory):
            for fname in files:
                path = os.path.join(root, fname)
                rel_path = os.path.relpath(path, directory)
                ext = os.path.splitext(fname)[1].lower()

                # Skip files above or at min_depth
                parts = rel_path.split(os.sep)
                if len(parts) <= min_depth:
                    continue

                # Add actual header files (no delimiters)
                if ext in header_exts:
                    collected.add('"' + rel_path + '"')

                # Parse includes in both headers and sources.
                # This is neither needed nor working, as it also pulls
                # in headers that are in inactivate regions of the preprocessor.
                # But we leave it as an option here, just in case we want it later.
                if parse_for_headers and ext in header_exts + source_exts:
                    included = find_included_headers( path, include_pattern )
                    collected.update(included)

    # Sort headers, sending `<...>` entries before `"..."` entries,
    # so that system headers come first, and then genesis headers.
    # Then, strip the surrounding delimiters for the final result.
    sorted_headers = sorted(collected, key=lambda h: (h.startswith('"'), h))
    cleaned_headers = [h[1:-1] for h in sorted_headers]
    return cleaned_headers

def find_missing_includes(headers: List[str], file_path: str) -> List[str]:
    """
    Check which strings from `headers` are absent in the file at `file_path`.

    Args:
        headers: List of substrings to search for.
        file_path: Path to the text file to scan.

    Returns:
        A list of strings from `headers` that were NOT found in the file.
    """
    # Read the entire file into one big string
    try:
        text = pathlib.Path(file_path).read_text(encoding='utf-8')
    except FileNotFoundError:
        raise FileNotFoundError(f"File not found: {file_path}")
    except Exception as e:
        raise RuntimeError(f"Error reading {file_path}: {e}")

    # Collect any needle not present in the file text
    missing = [s for s in headers if s not in text]
    return missing

def make_all_includes():
    # Collect _all_ headers of genesis and all that are included there.
    all_includes = collect_all_headers([genesis_source], min_directory_level)

    # Write out the headers. Binder wants angular brackets for all of them.
    with open(all_includes_file, 'w') as fh:
        for include in all_includes:
            fh.write(f'#include <{include}>\n')

def compare_all_includes():
    all_includes = collect_all_headers([genesis_source], min_directory_level)
    missing = find_missing_includes( all_includes, all_includes_file )
    if missing:
        print('\033[91m' + "Missing headers in", all_includes_file + '\033[0m')
        for m in missing:
            print('\033[91m' + f'#include <{m}>' + '\033[0m')

# ------------------------------------------------------------------------------
#   Make Bindings
# ------------------------------------------------------------------------------

def write_binder_version(output_path: str) -> None:
    # Capture the output of `binder --version`
    try:
        result = subprocess.run(
            [binder_executable, "--version"],
            capture_output=True,
            text=True,
            check=True
        )
        version_str = result.stdout.strip()
    except subprocess.CalledProcessError as e:
        version_str = f"Error running 'binder --version': {e}"
    pathlib.Path(output_path).write_text(version_str + "\n", encoding="utf-8")

def make_bindings_code(target_dir):
    # Build the binder command
    command = [f'{binder_executable}']

    # Basics
    command += ['--root-module', f'{python_module_name}']
    command += ['--prefix', f'{target_dir}']
    command += ['--bind', f'{namespace_to_bind}']
    command += ['--config', f'{config_file}']

    # Conditional extra arguments
    if use_pybind_stl:
        command += ['--include-pybind11-stl']
    if single_file:
        command += ['--single-file']
    if binder_annotate_includes:
        command += ['--annotate-includes']
    if binder_annotate_functions:
        command += ['--annotate-functions']
    if binder_debug:
        command += ['--trace', '-v']

    # Final binder args, and Clang args
    command += [f'{all_includes_file}']
    command += ['--']
    command += [f'-std=c++{c_plus_plus_version}']
    command += ['-DNDEBUG']
    # command += ['-v']

    # We need to include this directory as well,
    # so that the all_includes header can be parsed properly.
    for genesis_include in genesis_includes:
        command += [f'-I{genesis_include}']

    # Add other includes of libraries needed in our headers.
    for other_include in other_includes:
        command += [f'-I{other_include}']

    # Add the basic includes for pybind last as well.
    command += [f'-I{pybind_source}']
    # command += [f'-I{python_directory}']

    # Execute binder!
    print('Binder command:', ' '.join(command))
    subprocess.check_call(command)

def generate_bindings():
    # Some prep, version, dirs, etc.
    # Binder does not create the directory for us...

    # Make the bindings code, in a fresh new temp directory.
    temp_dir = pathlib.Path(bindings_dir).parent / (pathlib.Path(bindings_dir).name + "_tmp")
    os.makedirs(temp_dir, exist_ok=True)
    make_bindings_code( temp_dir )
    write_binder_version( os.path.join( temp_dir, "binder_version.txt" ))

    # Now sync the generated files to our actual target dir,
    # making sure to not touch time stamps of files that have not changed.
    # That will make incremental compiling much faster down the line!
    os.makedirs(bindings_dir, exist_ok=True)
    sync_dirs( temp_dir, bindings_dir )
    shutil.rmtree(temp_dir, ignore_errors=True)

# ------------------------------------------------------------------------------
#   Main
# ------------------------------------------------------------------------------

def main():
    if make_all_includes_file:
        make_all_includes()
    else:
        compare_all_includes()
    generate_bindings()

if __name__ == '__main__':
    main()
