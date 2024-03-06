/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief Implementation of Options members.
 *
 * @file
 * @ingroup main
 */

#include "genesis/utils/core/options.hpp"

#include "genesis/utils/core/info.hpp"
#include "genesis/utils/core/version.hpp"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <stdexcept>

#if defined( _WIN32 ) || defined(  _WIN64  )
#   include <io.h>
#   include <windows.h>
#else
#   ifndef __aarch64__
        // Both GCC and LLVM are missing the header guard for this header up until recent versions,
        // see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96238 and https://reviews.llvm.org/D91226
        // So we guard on our own here, which matters when genesis is compiled in a single unit.
        // See also genesis/utils/core/info.cpp for the current other use case of this.
#       ifndef GENESIS_INCLUDED_CPUID_H_
#           define GENESIS_INCLUDED_CPUID_H_
#           include <cpuid.h>
#       endif
#   endif
#   include <stdio.h>
#   include <sys/ioctl.h>
#   include <unistd.h>
#endif

namespace genesis {
namespace utils {

// =================================================================================================
//     Command Line
// =================================================================================================

std::string Options::command_line_string () const
{
    std::string ret = "";
    for (size_t i = 0; i < command_line_.size(); ++i) {
        std::string a = command_line_[i];
        ret += (i==0 ? "" : " ") + a;
    }
    return ret;
}

void Options::command_line( int const argc, char const* const* argv )
{
    // Store all arguments in the array.
    command_line_.clear();
    for (int i = 0; i < argc; i++) {
        command_line_.push_back(argv[i]);
    }
}

// =================================================================================================
//     Multi-Threading
// =================================================================================================

void Options::init_global_thread_pool()
{
    // Automatic guessing of the correct number of threads.
    // We then reduce it by one to account for the main thread doing work as well.
    auto const num_threads = guess_number_of_threads();
    assert( num_threads > 0 );
    init_global_thread_pool( num_threads - 1 );
}

void Options::init_global_thread_pool( size_t num_threads )
{
    if( thread_pool_ ) {
        throw std::runtime_error(
            "Global thread pool has already been initialized."
        );
    }
    thread_pool_ = std::make_shared<utils::ThreadPool>( num_threads );
}

std::shared_ptr<ThreadPool> Options::global_thread_pool() const
{
    if( ! thread_pool_ ) {
        throw std::runtime_error(
            "Global thread pool has not been initialized. Call init_global_thread_pool() first."
        );
    }
    return thread_pool_;
}

// =================================================================================================
//     Dump & Overview
// =================================================================================================

std::string Options::info() const
{
    std::string res;
    res += genesis_header() + "\n";
    res += info_compile_time() + "\n";
    res += info_run_time() + "\n";
    return res;
}

std::string Options::info_compile_time() const
{
    std::string res;
    res += "Compile Time Options\n";
    res += "=============================================\n\n";
    res += "Platform:          " + info_platform() + "\n";
    res += "Compiler:          " + info_compiler_family() + " " + info_compiler_version() + "\n";
    res += "C++ version:       " + info_cpp_version() + "\n";
    res += "Build type:        " + info_build_type()  + "\n";
    res += "Endianness:        " + std::string(
        info_is_little_endian() ? "little endian" : "big endian"
    ) + "\n";
    res += "Using OpenMP:      " + std::string( info_using_openmp() ? "true" : "false" ) + "\n";
    return res;
}

std::string Options::info_run_time() const
{
    std::string res;
    res += "Run Time Options\n";
    res += "=============================================\n\n";
    auto const cli_str = command_line_string();
    res += "Command line:      " + ( cli_str.size() > 0 ? cli_str : "(not available)" ) + "\n";
    size_t num_threads = 1;
    if( thread_pool_ ) {
        // Need to account for the main thread doing work as well.
        // We init the global thread pool with one fewer, so we need to add it back here.
        num_threads = thread_pool_->size() + 1;
    }
    res += "Number of threads: " + std::to_string( num_threads ) + "\n";
    res += "Random seed:       " + std::to_string( random_seed_ ) + "\n";
    return res;
}

} // namespace utils
} // namespace genesis
