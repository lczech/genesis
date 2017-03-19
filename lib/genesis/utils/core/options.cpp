/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief Implementation of Options members.
 *
 * @file
 * @ingroup main
 */

#include "genesis/utils/core/options.hpp"

#include "genesis/utils/core/version.hpp"

#include <chrono>
#include <cstdint>
#include <cstdio>

#if defined( _WIN32 ) || defined(  _WIN64  )
#   include <io.h>
#else
#   include <unistd.h>
#endif

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

#ifdef GENESIS_PTHREADS
#    include <thread>
#endif

namespace genesis {
namespace utils {

// =================================================================================================
//     Initialization
// =================================================================================================

Options::Options()
{

    #if defined( GENESIS_OPENMP )

        // Initialize threads to number of OpenMP threads, which might be set through the
        // `OMP_NUM_THREADS` environment variable.
        number_of_threads( omp_get_num_threads() );

    #elif defined( GENESIS_PTHREADS )

        // Initialize threads with actual number of cores.
        number_of_threads( std::thread::hardware_concurrency() );

    #else

        // Set to single threaded.
        number_of_threads( 1 );

    #endif

    // Initialize random seed with time.
    random_seed( std::chrono::system_clock::now().time_since_epoch().count() );
}

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
//     Number of Threads
// =================================================================================================

void Options::number_of_threads (const unsigned int number)
{
    number_of_threads_ = number;

    #if defined( GENESIS_OPENMP )

        // If we use OpenMp, set the thread number there, too.
        omp_set_num_threads( number );

    #endif
}

bool Options::using_pthreads() const
{
    #ifdef GENESIS_PTHREADS
        return true;
    #else
        return false;
    #endif
}

bool Options::using_openmp() const
{
    #ifdef GENESIS_OPENMP
        return true;
    #else
        return false;
    #endif
}

// =================================================================================================
//     Random Seed & Engine
// =================================================================================================

void Options::random_seed(const unsigned seed)
{
    random_seed_ = seed;
    random_engine_.seed( seed );
}

// =================================================================================================
//     Run Time Environment
// =================================================================================================

bool Options::stdin_is_terminal() const
{
    // Using http://stackoverflow.com/a/1312957/4184258
    #if defined( _WIN32 ) || defined(  _WIN64  )
        return _isatty( _fileno( stdin ));
    #else
        return isatty( fileno( stdin ));
    #endif
}

bool Options::stdout_is_terminal() const
{
    #if defined( _WIN32 ) || defined(  _WIN64  )
        return _isatty( _fileno( stdout ));
    #else
        return isatty( fileno( stdout ));
    #endif
}

bool Options::stderr_is_terminal() const
{
    #if defined( _WIN32 ) || defined(  _WIN64  )
        return _isatty( _fileno( stderr ));
    #else
        return isatty( fileno( stderr ));
    #endif
}

// =================================================================================================
//     Compile Time Environment
// =================================================================================================

bool Options::is_little_endian()
{
    static const uint16_t e = 0x1000;
    return 0 == *reinterpret_cast< uint8_t const* >( &e );
}

bool Options::is_big_endian()
{
    static const uint16_t e = 0x0001;
    return 0 == *reinterpret_cast< uint8_t const* >( &e );
}

std::string Options::platform()
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

std::string Options::compiler_family()
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

std::string Options::compiler_version()
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

std::string Options::cpp_version()
{
    #ifdef __cplusplus
        return std::to_string(__cplusplus);
    #else
        return "unknown";
    #endif
}

// =================================================================================================
//     Build Type
// =================================================================================================

bool Options::is_debug()
{
    #ifdef DEBUG
        return true;
    #else
        return false;
    #endif
}

bool Options::is_release()
{
    #ifdef NDEBUG
        return true;
    #else
        return false;
    #endif
}

std::string Options::build_type()
{
    #if defined( DEBUG )
        return "debug";
    #elif defined( NDEBUG )
        return "release";
    #else
        return "unknown";
    #endif
}

// =================================================================================================
//     Dump & Overview
// =================================================================================================

std::string Options::info() const
{
    std::string res = genesis_header();

    res += "\n";
    res += "Compile Time Options\n";
    res += "=============================================\n\n";
    res += "Platform:          " + platform() + "\n";
    res += "Compiler:          " + compiler_family() + " " + compiler_version() + "\n";
    res += "C++ version:       " + cpp_version() + "\n";
    res += "Build type:        " + build_type()  + "\n";
    res += "Endianness:        " + std::string( is_little_endian() ? "little endian" : "big endian" ) + "\n";
    res += "Using Pthreads:    " + std::string( using_pthreads() ? "true" : "false" ) + "\n";
    res += "Using OpenMP:      " + std::string( using_openmp() ? "true" : "false" ) + "\n";

    res += "\n";
    res += "Run Time Options\n";
    res += "=============================================\n\n";
    res += "Command line:      " + command_line_string() + "\n";
    res += "Number of threads: " + std::to_string( number_of_threads() ) + "\n";
    res += "Random seed:       " + std::to_string( random_seed_ ) + "\n";
    return res;
}

} // namespace utils
} // namespace genesis
