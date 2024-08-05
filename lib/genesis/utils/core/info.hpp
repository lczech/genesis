#ifndef GENESIS_UTILS_CORE_INFO_H_
#define GENESIS_UTILS_CORE_INFO_H_

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
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
#include <unordered_map>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Compiler Info
// =================================================================================================

struct InfoCompiler
{
    /**
     * @brief Date and time when genesis was compiled.
     *
     * Due to this using the preprocessor, the returned date and time are from when
     * the Options class was first compiled in a clean build process.
     */
    std::string compile_date_time;

    /**
    * @brief Binary was compiled with build type `DEBUG`.
    */
    bool is_debug;

    /**
    * @brief Binary was compiled with build type `RELEASE`.
    */
    bool is_release;

    /**
    * @brief Build type that was used to compile the binary, i.e., "debug" or "release".
    */
    std::string build_type;

    /**
    * @brief Platform under which genesis was compiled.
    *
    * This can be either "Win32", "Linux", "Apple", "Unix" or "Unknown".
    */
    std::string platform;

    /**
    * @brief Compiler family (name) that was used to compile genesis.
    *
    * See compiler_version to get the version of the compiler.
    */
    std::string compiler_family;

    /**
    * @brief Compiler version that was used to compile genesis.
    *
    * See compiler_family to get the corresponding compiler name.
    */
    std::string compiler_version;

    /**
    * @brief C++ version that was used to compile genesis.
    */
    std::string cpp_version;

    /**
     * @brief Compiled with OpenMP.
     */
    bool with_openmp;

    /**
     * @brief Compiled with AVX.
     *
     * Note that this only indicates if the AVX compiler flag was given, but it does not indicate
     * if AVX is actually available on the hardware and OS. See info_get_hardware() for that.
     */
    bool with_avx;

    /**
     * @brief Compiled with AVX2.
     *
     * Note that this only indicates if the AVX compiler flag was given, but it does not indicate
     * if AVX is actually available on the hardware and OS. See info_get_hardware() for that.
     */
    bool with_avx2;

    /**
     * @brief Compiled with AVX512.
     *
     * Note that this only indicates if the AVX compiler flag was given, but it does not indicate
     * if AVX is actually available on the hardware and OS. See info_get_hardware() for that.
     */
    bool with_avx512;

};

/**
 * @brief Return information about compiler settings and flags.
 */
InfoCompiler const& info_get_compiler();

/**
 * @brief Return a string map with some relevant preprocessor macros.
 *
 * This is mostly for debugging, to see how different systems compile the code.
 * We only add a set of important macros that we internally have use for as well.
 */
std::unordered_map<std::string,std::string> const& info_preprocessor_definitions();

/**
 * @brief Print information about compiler settings and flags to a string.
 */
std::string info_print_compiler();

// =================================================================================================
//     Hardware Info
// =================================================================================================

struct InfoHardware
{
    /**
     * @brief System uses little endian memory. If false, system uses big endian.
     */
    bool is_little_endian;

    /**
     * @brief Total amount of memory, in bytes.
     */
    size_t total_memory;

    // CPU
    bool vendor_AMD;
    bool vendor_Intel;
    std::string vendor_string;
    std::string cpu_model;
    size_t physical_core_count;
    bool with_hyperthreading;

    //  OS Features
    bool OS_x64;
    bool OS_AVX;
    bool OS_AVX512;

    //  Misc.
    bool HW_MMX;
    bool HW_x64;
    bool HW_ABM;
    bool HW_RDRAND;
    bool HW_RDSEED;
    bool HW_BMI1;
    bool HW_BMI2;
    bool HW_ADX;
    bool HW_MPX;
    bool HW_PREFETCHW;
    bool HW_PREFETCHWT1;
    bool HW_RDPID;

    //  SIMD: 128-bit
    bool HW_SSE;
    bool HW_SSE2;
    bool HW_SSE3;
    bool HW_SSSE3;
    bool HW_SSE41;
    bool HW_SSE42;
    bool HW_SSE4a;
    bool HW_AES;
    bool HW_SHA;

    //  SIMD: 256-bit
    bool HW_AVX;
    bool HW_XOP;
    bool HW_FMA3;
    bool HW_FMA4;
    bool HW_AVX2;

    //  SIMD: 512-bit
    bool HW_AVX512_F;
    bool HW_AVX512_CD;

    //  Knights Landing
    bool HW_AVX512_PF;
    bool HW_AVX512_ER;

    //  Skylake Purley
    bool HW_AVX512_VL;
    bool HW_AVX512_BW;
    bool HW_AVX512_DQ;

    //  Cannon Lake
    bool HW_AVX512_IFMA;
    bool HW_AVX512_VBMI;

    //  Knights Mill
    bool HW_AVX512_VPOPCNTDQ;
    bool HW_AVX512_4VNNIW;
    bool HW_AVX512_4FMAPS;

    //  Cascade Lake
    bool HW_AVX512_VNNI;

    //  Cooper Lake
    bool HW_AVX512_BF16;

    //  Ice Lake
    bool HW_AVX512_VBMI2;
    bool HW_GFNI;
    bool HW_VAES;
    bool HW_AVX512_VPCLMUL;
    bool HW_AVX512_BITALG;

};

/**
 * @brief Return information about hardware features.
 */
InfoHardware const& info_get_hardware();

/**
 * @brief Print information about hardware features to a string.
 *
 * This lists hardware features such as memory, CPU model, and potentially CPU features.
 * If @p full is given, each CPU feature that we test for is listed. Otherwise, by default,
 * we just print a summary of the features that are important to us, such as AVX.
 */
std::string info_print_hardware( bool full = false );

/**
 * @brief Assess if it is safe to use AVX features.
 *
 * This is the case if the hardware supports AVX, the OS has it activated, and the compiler had
 * the respective flag set.
 */
bool info_use_avx();

/**
 * @brief Assess if it is safe to use AVX2 features.
 *
 * This is the case if the hardware supports AVX2, the OS has it activated, and the compiler had
 * the respective flag set.
 */
bool info_use_avx2();

/**
 * @brief Assess if it is safe to use AVX512 features.
 *
 * This is the case if the hardware supports AVX512, the OS has it activated, and the compiler had
 * the respective flag set.
 */
bool info_use_avx512();

// =================================================================================================
//     Number of Threads
// =================================================================================================

/**
 * @brief Get the number of CPU cores.
 *
 * This tries to get the physical core count, and if that fails, it uses
 * std::thread::hardware_concurrency() along with info_hyperthreads_enabled() to determine
 * a best guess at the number of physical cores.
 */
size_t info_physical_core_count();

/**
 * @brief Try to get whether hyperthreads are enabled in the current system.
 */
bool info_hyperthreads_enabled();

/**
 * @brief Get the number of threads as indicated by the OpenMP environment.
 *
 * If genesis was compiled with OpenMP support (that is, if `GENESIS_OPENMP` is defined, which is
 * done via the CMake script), we use `omp_get_max_threads()` to get a number of threads from that.
 * If however genesis was compiled without OpenMP support, we instead consider the environment
 * variable `OMP_NUM_THREADS` as a source for the OpenMP threads.
 *
 * If unsuccessful, 0 is returned.
 */
size_t info_number_of_threads_openmp();

/**
 * @brief Get the number of threads as indicated by the SLURM environment.
 *
 * This uses the environment variable `SLURM_CPUS_PER_TASK`. If unsuccessful, 0 is returned.
 */
size_t info_number_of_threads_slurm();

/**
 * @brief Make an educated guess on the number of threads to use for multi-threaded functionality.
 *
 * This function uses multiple sources and ways to try to guess a reasonable number of threads:
 *
 *  - If @p use_openmp is set, we use the `omp_get_max_threads()` function or the `OMP_NUM_THREADS`
 *    environment variable to get a number of threads, using the info_number_of_threads_openmp()
 *    function. For details, see there.
 *  - If @p use_slurm is set, we get a number of cores from the environment variable
 *    `SLURM_CPUS_PER_TASK`.
 *  - Lastly, we get the result of `std::thread::hardware_concurrency()` as another hint.
 *    If furthermore @p physical_cores is set, and hyperthreads are enabled, we divide that hardware
 *    concurrency number by two, in order to account for hyperthreads, resulting in the number
 *    of physical cores available on the system (ideally). This avoids core oversubscription that
 *    could otherwise be the result of using all threads instead of all physical cores.
 *
 * If the numbers disagree with each other, we prefer OpenMP over slurm over `hardware_concurrency()`,
 * that is, we are going from most specific to least. Furthermore, if the OpenMP based guess yields
 * exactly the same number as the hardware concurrency, we also use the @p physical_cores setting,
 * as this result usually indicates that OpenMP left at its default, in which case we also want
 * to avoid core oversubscription due to hyperthreading.
 *
 * The function is guaranteed to return a non-zero value, meaning that at least the main thread
 * is always accounted for. This is important for Options::get().init_global_thread_pool(), which
 * will account for that, and initialize the number of additional threads to one fewer than this.
 */
size_t guess_number_of_threads(
    bool use_openmp = true,
    bool use_slurm = true,
    bool physical_cores = true
);

// =================================================================================================
//     Run Time Environment
// =================================================================================================

/**
 * @brief Get the process ID of the current process.
 *
 * This is a POSIX system call, using `getpid()`. See https://man7.org/linux/man-pages/man2/getpid.2.html
 * and https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/getpid.2.html
 * for details. We wrap this here to avoid having the system headers included. Furthermore, we
 * convert to a size_t for convenience, although typically, the pid is an `int` (guaranteed to be
 * non-negative for an actual process).
 */
size_t info_get_pid();

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
//     Current Resource Usage
// =================================================================================================

/**
 * @brief Return the maximum number of files (i.e., file descriptors)
 * that can be opened simultaneously in the current process.
 */
size_t info_process_max_file_count();

/**
 * @brief Return the number of files (i.e., file descriptors) that the current process
 * (the process calling this function) has opened at the moment.
 */
size_t info_process_current_file_count();

/**
 * @brief Return the memory currently used by the current process, in bytes.
 */
size_t info_process_current_memory_usage();

/**
 * @brief Return the memory currently used across all running processes, in bytes.
 */
size_t info_system_current_memory_usage();

/**
 * @brief Return the memory currently available in the system, in bytes.
 */
size_t info_system_current_memory_available();

/**
 * @brief Return the CPU usage of the current process.
 *
 * The first time the function is called, it is being initialized, and will return 0.0, as there is
 * no usage to be reported yet. Any subsequent call with then report the usage since the last call.
 *
 * By default, this reports the total usage across all cores. That is, for multi-core systems,
 * this can report usage up to the number of cores. For instance, two cores fully running on a
 * 4-core system would report 200% usage (using return value 200.0 by default, or 2.0 if @p percent
 * is set to `false`). If however @p all_cores is set to `false`, this is instead divided by the
 * number of cores, and so would return 50% usage (using return value 50.0 or 0.5, depending on
 * @p percent) instead.
 */
double info_process_current_cpu_usage( bool all_cores = true, bool percent = true );

/**
 * @brief Return the CPU usage of the system, across all cores.
 *
 * @see See info_process_current_cpu_usage() for details on the parameters.
 */
double info_system_current_cpu_usage( bool all_cores = true, bool percent = true );

// =================================================================================================
//     Total Resource Usage
// =================================================================================================

/**
 * @brief Get the peak used memory, in bytes.
 */
size_t info_process_peak_memory_usage();

/**
 * @brief Get the currently used cpu run time, similar to the Unix `time` command.
 *
 * Time is returned in seconds, with the first result the user time, and the second the system time.
 */
std::pair<double, double> info_process_total_cpu_time();

/**
 * @brief Get energy consumption of the program so far, in Wh.
 */
double info_process_total_energy_consumption();

/**
 * @brief Print usage information to a string.
 */
std::string info_process_print_total_usage();

} // namespace utils
} // namespace genesis

#endif // include guard
