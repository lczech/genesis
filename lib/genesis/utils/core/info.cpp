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
 * @brief
 *
 * @file
 * @ingroup main
 */

#include "genesis/utils/core/info.hpp"

#include "genesis/utils/core/version.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <thread>

#include <errno.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#if defined( _WIN32 ) || defined(  _WIN64  )
#    include <io.h>
#    include <windows.h>
#else
#    ifndef __aarch64__
        // Both GCC and LLVM are missing the header guard for this header up until recent versions,
        // see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96238 and https://reviews.llvm.org/D91226
        // So we guard on our own here, which matters when genesis is compiled in a single unit.
        // See also genesis/utils/core/options.cpp for the current other use case of this.
#       ifndef GENESIS_INCLUDED_CPUID_H_
#           define GENESIS_INCLUDED_CPUID_H_
#           include <cpuid.h>
#       endif
#    endif
#    include <stdio.h>
#    include <sys/ioctl.h>
#    include <sys/types.h>
#    include <unistd.h>
#endif

#ifdef GENESIS_OPENMP
#    include <omp.h>
#endif

namespace genesis {
namespace utils {

// =================================================================================================
//     Compile Time Environment
// =================================================================================================

// Some static checking to make sure that our CMake setup is correct.
#if defined( DEBUG ) && defined( NDEBUG )
    static_assert( false, "Cannot compile with both DEBUG and NDEBUG flags set." );
#endif

#if ! defined( DEBUG ) && ! defined( NDEBUG )
    static_assert( false, "Cannot compile with neiher DEBUG nor NDEBUG flag set." );
#endif

bool info_is_debug()
{
    #ifdef DEBUG
        return true;
    #else
        return false;
    #endif
}

bool info_is_release()
{
    #ifdef NDEBUG
        return true;
    #else
        return false;
    #endif
}

std::string info_build_type()
{
    #if defined( DEBUG )
        return "debug";
    #elif defined( NDEBUG )
        return "release";
    #else
        return "unknown";
    #endif
}

bool info_is_little_endian()
{
    static const uint16_t e = 0x1000;
    return 0 == *reinterpret_cast< uint8_t const* >( &e );
}

bool info_is_big_endian()
{
    static const uint16_t e = 0x0001;
    return 0 == *reinterpret_cast< uint8_t const* >( &e );
}

std::string info_platform()
{
    #if defined _WIN64
        return "Win64";
    #elif defined _WIN32
        return "Win32";
    #elif defined __linux__
        return "Linux";
    #elif defined __APPLE__
        return "Apple";
    #elif defined __unix__
        return "Unix";
    #else
        return "Unknown";
    #endif
}

std::string info_compiler_family()
{
    #if defined(__clang__)
        return "clang";
    #elif defined(__ICC) || defined(__INTEL_COMPILER)
        return "icc";
    #elif defined(__GNUC__) || defined(__GNUG__)
        return "gcc";
    #elif defined(__HP_cc) || defined(__HP_aCC)
        return "hp";
    #elif defined(__IBMCPP__)
        return "ilecpp";
    #elif defined(_MSC_VER)
        return "msvc";
    #elif defined(__PGI)
        return "pgcpp";
    #elif defined(__SUNPRO_CC)
        return "sunpro";
    #else
        return "unknown";
    #endif
}

std::string info_compiler_version()
{
    #if defined(__clang__)
        return __clang_version__;
    #elif defined(__ICC) || defined(__INTEL_COMPILER)
        return __INTEL_COMPILER;
    #elif defined(__GNUC__) || defined(__GNUG__)
        return std::to_string(__GNUC__)            + "." +
               std::to_string(__GNUC_MINOR__)      + "." +
               std::to_string(__GNUC_PATCHLEVEL__)
        ;
    #elif defined(__HP_cc) || defined(__HP_aCC)
        return "";
    #elif defined(__IBMCPP__)
        return __IBMCPP__;
    #elif defined(_MSC_VER)
        return _MSC_VER;
    #elif defined(__PGI)
        return __PGI;
    #elif defined(__SUNPRO_CC)
        return __SUNPRO_CC;
    #else
        return "unknown";
    #endif
}

std::string info_cpp_version()
{
    #ifdef __cplusplus
        return std::to_string(__cplusplus);
    #else
        return "unknown";
    #endif
}

std::string info_compile_date_time()
{
    return std::string( __DATE__ " " __TIME__ );
}

bool info_using_openmp()
{
    #ifdef GENESIS_OPENMP
        return true;
    #else
        return false;
    #endif
}

bool info_using_zlib()
{
    #ifdef GENESIS_ZLIB
        return true;
    #else
        return false;
    #endif
}

bool info_using_htslib()
{
    #ifdef GENESIS_HTSLIB
        return true;
    #else
        return false;
    #endif
}

// =================================================================================================
//     Run Time Environment
// =================================================================================================

size_t info_get_pid()
{
    // The return type `pid_t` of getpid() is an `int`, guaranteed to be non-negative:
    // https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_node/libc_554.html
    // https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap03.html#tag_03_300
    // We hence simply convert to `size_t` here.
    auto const pid = ::getpid();
    assert( pid > 0 );
    return static_cast<size_t>( pid );
}

bool info_stdin_is_terminal()
{
    // Using http://stackoverflow.com/a/1312957/4184258
    #if defined( _WIN32 ) || defined(  _WIN64  )
        return _isatty( _fileno( stdin ));
    #else
        return isatty( fileno( stdin ));
    #endif
}

bool info_stdout_is_terminal()
{
    #if defined( _WIN32 ) || defined(  _WIN64  )
        return _isatty( _fileno( stdout ));
    #else
        return isatty( fileno( stdout ));
    #endif
}

bool info_stderr_is_terminal()
{
    #if defined( _WIN32 ) || defined(  _WIN64  )
        return _isatty( _fileno( stderr ));
    #else
        return isatty( fileno( stderr ));
    #endif
}

std::pair<int, int> info_terminal_size()
{
    #if defined( _WIN32 ) || defined(  _WIN64  )

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        int cols, rows;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        return { cols, rows };

    #else

        struct winsize w;
        ioctl( STDOUT_FILENO, TIOCGWINSZ, &w );
        return { w.ws_col, w.ws_row };

    #endif
}

// =================================================================================================
//     File Handling
// =================================================================================================

size_t info_max_file_count()
{
    // https://www.man7.org/linux/man-pages/man2/getdtablesize.2.html
    return getdtablesize();

    // Alternative way. Not needed, as the above function internally calls the rlimits anyway.
    // https://man7.org/linux/man-pages/man2/getrlimit.2.html
    // struct rlimit rlimits;
    // getrlimit(RLIMIT_NOFILE, &rlimits);
    // rlimits.rlim_cur // soft limit
    // rlimits.rlim_max // hard limit (ceiling for rlim_cur)
}

size_t info_current_file_count()
{
    // Adapted from https://web.archive.org/web/20150326011742/
    // http://blog.lobstertechnology.com/2005/08/22/determine-the-number-of-open-files/

    // We loop over all possible file descriptor numbers...
    auto const max_fd_cnt = info_max_file_count();

    // ... and check if they are currently in use. If errno returns anything other
    // than EBADF 'file descriptor is bad', it increments a count.
    size_t fd_counter = 0;
    struct stat stats;
    for( size_t i = 0; i <= max_fd_cnt; ++i ) {
        errno = 0;
        fstat( i, &stats );
        if( errno != EBADF ) {
            ++fd_counter;
        }
    }
    return fd_counter;
}

// =================================================================================================
//     Number of Threads
// =================================================================================================

size_t guess_number_of_threads( bool use_openmp, bool use_slurm, bool physical_cores )
{
    // Dummy to avoid compiler warnings.
    (void) use_openmp;

    // Default to 1 thread. Will be overwritten later.
    size_t guess = 1;

    // Initialize threads with actual number of cores.
    // The function might return 0 if no number could be determined, in which case we default to 1.
    auto const hw_concur = std::thread::hardware_concurrency();
    if( hw_concur > 0 ) {
        guess = static_cast<size_t>( hw_concur );
    }

    // Now take hyperthreads into account.
    auto const threads_per_core = info_hyperthreads_enabled() ? 2 : 1;
    auto const hw_cores = hw_concur / threads_per_core;
    if( hw_cores > 0 && physical_cores ) {
        guess = static_cast<size_t>( hw_cores );
    }

    // Now try slurm, if specified.
    if( use_slurm ) {
        auto const slurm_cpus = info_number_of_threads_slurm();
        if( slurm_cpus > 0 ) {
            guess = static_cast<size_t>( slurm_cpus );
        }
    }

    // Lastly, try OpenMP, if specified.
    if( use_openmp ) {
        auto const openmp_threads = info_number_of_threads_openmp();
        if( openmp_threads > 0 ) {
            guess = static_cast<size_t>( openmp_threads );
        }
        if( openmp_threads == hw_concur && hw_cores > 0 && physical_cores ) {
            guess = static_cast<size_t>( hw_cores );
        }
    }

    assert( guess > 0 );
    return guess;
}

bool info_hyperthreads_enabled()
{
    // Get CPU info.
    int32_t info[4];
    #ifdef __aarch64__
        (void)info;
        return false;
    #elif defined( _WIN32 )
        __cpuid( info, 1 );
    #else
        __cpuid_count( 1, 0, info[0], info[1], info[2], info[3] );
    #endif

    return (bool) (info[3] & (0x1 << 28));
}

size_t info_number_of_threads_openmp()
{
    size_t openmp_threads = 0;

    #if defined( GENESIS_OPENMP )

        // Use number of OpenMP threads, which might be set through the `OMP_NUM_THREADS`
        // environment variable. If there was an error there, fix it.
        openmp_threads = static_cast<size_t>( std::max( omp_get_max_threads(), 0 ));

    #else

        // Use the env variable directly if we don't have access to the OpenMP functions.
        auto const openmp_ptr = std::getenv( "OMP_NUM_THREADS" );
        if( openmp_ptr ) {
            openmp_threads = std::atoi( openmp_ptr );
        }

    #endif

    return openmp_threads;
}

size_t info_number_of_threads_slurm()
{
    size_t slurm_cpus = 0;
    auto const slurm_ptr = std::getenv( "SLURM_CPUS_PER_TASK" );
    if( slurm_ptr ) {
        slurm_cpus = std::atoi( slurm_ptr );

    }
    return slurm_cpus;
}

} // namespace utils
} // namespace genesis
