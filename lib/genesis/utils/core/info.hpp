#ifndef GENESIS_UTILS_CORE_INFO_H_
#define GENESIS_UTILS_CORE_INFO_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
 * @ingroup utils
 */

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Compile Time Environment
// =================================================================================================

/**
 * @brief Return whether the binary was compiled with build type `DEBUG`.
 */
bool info_is_debug();

/**
 * @brief Return whether the binary was compiled with build type `RELEASE`.
 */
bool info_is_release();

/**
 * @brief Return the build type that was used to compile the binary, i.e., "debug" or "release".
 */
std::string info_build_type();

/**
 * @brief Return whether the system uses little endian memory.
 */
bool info_is_little_endian();

/**
 * @brief Return whether the system uses big endian memory.
 */
bool info_is_big_endian();

/**
 * @brief Return the platform under which genesis was compiled.
 *
 * This can be either "Win32", "Linux", "Apple", "Unix" or "Unknown".
 */
std::string info_platform();

/**
 * @brief Return the compiler family (name) that was used to compile genesis.
 *
 * See compiler_version() to get the version of the compiler.
 */
std::string info_compiler_family();

/**
 * @brief Return the compiler version that was used to compile genesis.
 *
 * See compiler_family() to get the corresponding compiler name.
 */
std::string info_compiler_version();

/**
 * @brief Return the CPP version that was used to compile genesis.
 */
std::string info_cpp_version();

/**
 * @brief Return the date and time when genesis was compiled.
 *
 * Due to this using the preprocessor, the returned date and time are from when
 * the Options class was first compiled in a clean build process.
 */
std::string info_compile_date_time();

/**
 * @brief Return whether the binary was compiled using Pthreads.
 */
bool info_using_pthreads();

/**
 * @brief Return whether the binary was compiled using OpenMP.
 */
bool info_using_openmp();

/**
 * @brief Return whether the binary was compiled using zlib.
 */
bool info_using_zlib();

/**
 * @brief Return whether the binary was compiled with our optional htslib dependency.
 */
bool info_using_htslib();

// =================================================================================================
//     Run Time Environment
// =================================================================================================

/**
 * @brief Return true iff the standard input stream is a terminal, and false if not, i.e., if
 * it is a file or a pipe.
 */
bool info_stdin_is_terminal();

/**
 * @brief Return true iff the standard output stream is a terminal, and false if not, i.e., if
 * it is a file or a pipe.
 */
bool info_stdout_is_terminal();

/**
 * @brief Return true iff the standard error stream is a terminal, and false if not, i.e., if
 * it is a file or a pipe.
 */
bool info_stderr_is_terminal();

/**
 * @brief Return the width and height of the terminal that is used to run the program,
 * in number of columns and lines.
 */
std::pair<int, int> info_terminal_size();

// =================================================================================================
//     File Handling
// =================================================================================================

/**
 * @brief Return the maximum number of files (i.e., file descriptors)
 * that can be opened simultaneously in the current process.
 */
size_t info_max_file_count();

/**
 * @brief Return the number of files (i.e., file descriptors) that the current process
 * (the process calling this function) has opened at the moment.
 */
size_t info_current_file_count();

// =================================================================================================
//     Number of Threads
// =================================================================================================

/**
 * @brief Try to get whether hyperthreads are enabled in the current system.
 */
bool hyperthreads_enabled();

/**
 * @brief Make an educated guess on the number of threads to use for multi-threaded functionality.
 *
 * This function uses multiple sources and ways to try to guess a reasonable number of threads:
 *
 *  - If @p use_openmp is set (and genesis was compiled with OpenMP support) we obtain
 *    `omp_get_max_threads()` to get a number of threads from that. This itself considers for
 *    instance the environment variable `OMP_NUM_THREADS` as a source.
 *  - If @p use_slurm is set, we get a number of cores from the environment variable
 *    `SLURM_CPUS_PER_TASK`.
 *  - Lastly, we get the result of `std::thread::hardware_concurrency()` as another hint.
 *    If furthermore @p physical_cores is set, and hyperthreads are enabled, we divide that hardware
 *    concurrency number by two, in order to account for hyperthreads, resulting in the number
 *    of physical cores available on the system (ideally). This avoids core oversubscription that
 *    could otherwise be the result of using all threads instead of all physical cores.
 *
 * If the numbers disagree with each other, we prefer OpenMP over slurm over `std::thread`,
 * that is, we are going from most specific to least. Furthermore, if the OpenMP based guess yields
 * exactly the same number as the hardware concurrency, we also use the @p physical_cores setting,
 * as this result usually indicates that OpenMP left at its default, in which case we also want
 * to avoid core oversubscription due to hyperthreading.
 */
size_t guess_number_of_threads(
    bool use_openmp = true,
    bool use_slurm = true,
    bool physical_cores = true
);

} // namespace utils
} // namespace genesis

#endif // include guard
