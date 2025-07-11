#!/usr/bin/env python3

import os
import shutil
import sys
import subprocess
import pathlib


# Overall script settings
binder_executable = '/usr/local/bin/binder'
binder_source = '/home/giannis/Desktop/binder/source'
python_module_name = 'mylibgenesis'
bindings_dir = './genesis_bindings'
namespace_to_bind = '""'
config_file = './config_file.cfg'
all_includes_file = './all_includes.hpp'
use_pybind_stl = True
single_file = False
c_plus_plus_version = '11'
genesis_directory = './../lib'
pybind_source = '/usr/include/pybind11'
python_directory = '/usr/include/python3.10'
genesis_library_file = '/home/giannis/Desktop/genesis/bin/libgenesis.so'

def clear_bindings_directory():
    for filename in os.listdir(bindings_dir):
        file_path = os.path.join(bindings_dir, filename)
        try:
            if os.path.isfile(file_path) or os.path.islink(file_path):
                os.unlink(file_path)
            elif os.path.isdir(file_path):
                shutil.rmtree(file_path)
        except Exception as e:
            print('Failed to delete %s. Reason: %s' % (file_path, e))

def make_bindings_code():
    binder_command = ""
    binder_command += binder_executable + " --root-module " + python_module_name + " --prefix " + \
                        bindings_dir + " --bind=" + namespace_to_bind + " --config " + \
                        config_file + " " + all_includes_file + " "
    if use_pybind_stl:
        binder_command += "--include-pybind11-stl "
    if single_file:
        binder_command += "--single-file "
    binder_command += "-- -std=c++" + c_plus_plus_version + " -I" + genesis_directory + \
                        " -I" + pybind_source + " -I" + python_directory + " -DNDEBUG"

    os.system(binder_command)

    sources_to_compile = []
    if single_file:
        sources_to_compile.append(python_module_name + '.cpp' )
    else:
        with open(f'{bindings_dir}/{python_module_name}.sources', 'r') as fh:
            for line in fh:
                sources_to_compile.append(line.strip())

    return sources_to_compile


def compile_sources(sources_to_compile):
    lines_to_write = []
    lines_to_write.append(f'project({python_module_name})')
    lines_to_write.append(f'cmake_minimum_required(VERSION 3.10)')
    lines_to_write.append(f'set(CMAKE_CXX_STANDARD ' + c_plus_plus_version + ')')
    lines_to_write.append(f'set(CMAKE_CXX_STANDARD_REQUIRED True)')

    for include_dir in [binder_source, genesis_directory, pybind_source, python_directory]:
        lines_to_write.append(f'include_directories({include_dir})')
    lines_to_write.append('set_property(GLOBAL PROPERTY POSITION_INDEPENDENT_CODE ON)')  # -fPIC
    lines_to_write.append('add_definitions(-DNDEBUG)')

    lines_to_write.append(f'add_library({python_module_name} SHARED')
    for source in sources_to_compile:
        lines_to_write.append(f'\t{source}')
    lines_to_write.append(')')
    lines_to_write.append(f'target_link_libraries({python_module_name} PUBLIC {genesis_library_file} )')

    lines_to_write.append(f'set_target_properties({python_module_name} PROPERTIES PREFIX "")')
    lines_to_write.append(f'set_target_properties({python_module_name} PROPERTIES SUFFIX ".so")')

    with open('CMakeLists.txt', 'w') as f:
        for line in lines_to_write:
            f.write(f'{line}\n')

    # Done making CMakeLists.txt
    cmake_script_path = pathlib.Path().resolve()
    os.chdir(bindings_dir)

    # fast, a lot of memory
    cmake_command = 'cmake ' + str(cmake_script_path) + ' -G Ninja'
    subprocess.call(cmake_command.split())
    subprocess.call('ninja')

    # very slow, less memory
    #cmake_command = 'cmake ' + str(cmake_script_path)
    #subprocess.call(cmake_command.split())
    #subprocess.call('make')


def main():
    clear_bindings_directory()
    sources_to_compile = make_bindings_code()
    compile_sources(sources_to_compile)


if __name__ == '__main__':
    main()
