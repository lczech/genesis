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

/*
    The implementation of the hardware feature detaction is based on FeatureDetector,
    see https://github.com/Mysticial/FeatureDetector which is published under CC0 1.0 Universal.
    We have adapted the code to be contained in the classes and functions here.

    For an alternative approach that seems to support Apple better,
    see https://github.com/xflouris/libpll-2/blob/master/src/hardware.c
    and https://github.com/amkozlov/raxml-ng/blob/master/src/util/sysutil.cpp

    For more hardware related information, see
    https://github.com/amkozlov/raxml-ng/blob/master/src/util/sysutil.cpp
 */

/**
 * @brief
 *
 * @file
 * @ingroup main
 */

#include "genesis/utils/core/info.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/core/options.hpp"
#include "genesis/utils/core/version.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <chrono>
#include <climits>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include <errno.h>
#include <stdio.h>

// These potentially need to be adopted for Windows.
// Untested, will deal with Windows later. Or never.
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// -----------------------------------------------------------------------------
//     OS Specific Includes
// -----------------------------------------------------------------------------

#if defined( __APPLE__ )
#    include <sys/sysctl.h>
#endif

#if defined(__linux__)
#    include <sched.h>
#endif

// For processor features and other intrinsics
#if defined( _WIN32 ) || defined( _WIN64  )|| defined( _MSC_VER )
#    include <io.h>
#    include <intrin.h>
#    include <windows.h>
#elif defined( __GNUC__ ) || defined( __clang__ )
#    ifndef __aarch64__
        // Both GCC and LLVM are missing the header guard for this header up until recent versions,
        // see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96238 and https://reviews.llvm.org/D91226
        // So we guard on our own here, which matters when genesis is compiled in a single unit.
#       ifndef GENESIS_INCLUDED_CPUID_H_
#           define GENESIS_INCLUDED_CPUID_H_
#           include <cpuid.h>
#       endif
#    endif
#    include <stdio.h>
#    include <sys/ioctl.h>
#    include <sys/types.h>
#    include <unistd.h>
#else
// #    warning "No cpuid intrinsic defined for compiler."
#endif

#if !(defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86))
// #    warning "No cpuid intrinsic defined for processor architecture."
#endif

// For current usage
#if defined( _WIN32 ) || defined( _WIN64  )|| defined( _MSC_VER )
#    include <TCHAR.h>
#    include <pdh.h>
#    include <psapi.h>
#    include <windows.h>
#elif defined( __APPLE__ )
#    include <mach/mach_error.h>
#    include <mach/mach_host.h>
#    include <mach/mach_init.h>
#    include <mach/mach_types.h>
#    include <mach/mach.h>
#    include <mach/task_info.h>
#    include <mach/vm_map.h>
#    include <mach/vm_statistics.h>
#    include <sys/resource.h>
#    include <sys/time.h>
#    include <sys/types.h>
// #    include <sys/sysinfo.h>
#elif defined( __GNUC__ ) || defined( __clang__ ) || defined(__linux__)
#    include <sys/types.h>
#    include <sys/sysinfo.h>
#    include <stdlib.h>
#    include <stdio.h>
#    include <string.h>
#    include <sys/times.h>
// #    include <sys/vtimes.h>
#endif

#ifdef GENESIS_OPENMP
#    include <omp.h>
#endif

namespace genesis {
namespace utils {

// =================================================================================================
//     Compiler Info
// =================================================================================================

// Some static checking to make sure that our CMake setup is correct.
#if defined( DEBUG ) && defined( NDEBUG )
    static_assert( false, "Cannot compile with both DEBUG and NDEBUG flags set." );
#endif

#if ! defined( DEBUG ) && ! defined( NDEBUG )
    static_assert( false, "Cannot compile with neiher DEBUG nor NDEBUG flag set." );
#endif

InfoCompiler const& info_get_compiler()
{
    // No need to repeat.
    static InfoCompiler result;
    static bool initialized = false;
    if( initialized ) {
        return result;
    }

    // Compile date and time
    result.compile_date_time = std::string( __DATE__ " " __TIME__ );

    // Build type
    #ifdef DEBUG
        result.is_debug = true;
    #else
        result.is_debug = false;
    #endif
    #ifdef NDEBUG
        result.is_release = true;
    #else
        result.is_release = false;
    #endif
    #if defined( DEBUG )
        result.build_type = "debug";
    #elif defined( NDEBUG )
        result.build_type = "release";
    #else
        result.build_type = "unknown";
    #endif

    // Platform
    #if defined _WIN64
        result.platform = "Win64";
    #elif defined _WIN32
        result.platform = "Win32";
    #elif defined __linux__
        result.platform = "Linux";
    #elif defined __APPLE__
        result.platform = "Apple";
    #elif defined __unix__
        result.platform = "Unix";
    #else
        result.platform = "Unknown";
    #endif

    // Compiler family
    #if defined(__clang__)
        result.compiler_family = "clang";
    #elif defined(__ICC) || defined(__INTEL_COMPILER)
        result.compiler_family = "icc";
    #elif defined(__GNUC__) || defined(__GNUG__)
        result.compiler_family = "gcc";
    #elif defined(__HP_cc) || defined(__HP_aCC)
        result.compiler_family = "hp";
    #elif defined(__IBMCPP__)
        result.compiler_family = "ilecpp";
    #elif defined(_MSC_VER)
        result.compiler_family = "msvc";
    #elif defined(__PGI)
        result.compiler_family = "pgcpp";
    #elif defined(__SUNPRO_CC)
        result.compiler_family = "sunpro";
    #else
        result.compiler_family = "unknown";
    #endif

    // Compiler version
    #if defined(__clang__)
        result.compiler_version = __clang_version__;
    #elif defined(__ICC) || defined(__INTEL_COMPILER)
        result.compiler_version = __INTEL_COMPILER;
    #elif defined(__GNUC__) || defined(__GNUG__)
        result.compiler_version =
            std::to_string(__GNUC__)            + "." +
            std::to_string(__GNUC_MINOR__)      + "." +
            std::to_string(__GNUC_PATCHLEVEL__)
        ;
    #elif defined(__HP_cc) || defined(__HP_aCC)
        result.compiler_version = "";
    #elif defined(__IBMCPP__)
        result.compiler_version = __IBMCPP__;
    #elif defined(_MSC_VER)
        result.compiler_version = _MSC_VER;
    #elif defined(__PGI)
        result.compiler_version = __PGI;
    #elif defined(__SUNPRO_CC)
        result.compiler_version = __SUNPRO_CC;
    #else
        result.compiler_version = "unknown";
    #endif

    // C++ version
    #ifdef __cplusplus
        result.cpp_version = std::to_string(__cplusplus);
    #elif defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
        result.cpp_version = std::to_string(_MSVC_LANG);
    #else
        result.cpp_version = "unknown";
    #endif

    // OpenMP
    #ifdef GENESIS_OPENMP
        result.with_openmp = true;
    #else
        result.with_openmp = false;
    #endif

    // AVX/AVX2
    #ifdef GENESIS_AVX
        result.with_avx = true;
    #else
        result.with_avx = false;
    #endif
    #ifdef GENESIS_AVX2
        result.with_avx2 = true;
    #else
        result.with_avx2 = false;
    #endif
    result.with_avx512 = false;

    initialized = true;
    return result;
}

std::unordered_map<std::string,std::string> const& info_preprocessor_definitions()
{
    // No need to repeat.
    static std::unordered_map<std::string,std::string> result;
    static bool initialized = false;
    if( initialized ) {
        return result;
    }

    // Now check all the macros that we want. There is likely no way to make these one-lines,
    // as having a preprocessor ifdef within a macro definition itself is not possible.
    #ifdef __cplusplus
        result.emplace( "__cplusplus", std::to_string( __cplusplus ));
    #endif
    #ifdef _MSVC_LANG
        result.emplace( "_MSVC_LANG", std::to_string( _MSVC_LANG ));
    #endif
    #ifdef __GNUC__
        result.emplace( "__GNUC__", std::to_string( __GNUC__ ));
    #endif
    #ifdef __clang__
        result.emplace( "__clang__", std::to_string( __clang__ ));
    #endif
    #ifdef _MSC_VER
        result.emplace( "_MSC_VER", std::to_string( _MSC_VER ));
    #endif
    #ifdef __APPLE__
        result.emplace( "__APPLE__", std::to_string( __APPLE__ ));
    #endif
    #ifdef __linux__
        result.emplace( "__linux__", std::to_string( __linux__ ));
    #endif
    #ifdef __unix__
        result.emplace( "__unix__", std::to_string( __unix__ ));
    #endif
    #ifdef _WIN32
        result.emplace( "_WIN32", std::to_string( _WIN32 ));
    #endif
    #ifdef _WIN64
        result.emplace( "_WIN64", std::to_string( _WIN64 ));
    #endif
    #ifdef __x86_64__
        result.emplace( "__x86_64__", std::to_string( __x86_64__ ));
    #endif
    #ifdef _M_X64
        result.emplace( "_M_X64", std::to_string( _M_X64 ));
    #endif
    #ifdef __i386
        result.emplace( "__i386", std::to_string( __i386 ));
    #endif
    #ifdef _M_IX86
        result.emplace( "_M_IX86", std::to_string( _M_IX86 ));
    #endif
    #ifdef __ARM_ARCH
        result.emplace( "__ARM_ARCH", std::to_string( __ARM_ARCH ));
    #endif
    #ifdef __arm__
        result.emplace( "__arm__", std::to_string( __arm__ ));
    #endif
    #ifdef __aarch64__
        result.emplace( "__aarch64__", std::to_string( __aarch64__ ));
    #endif

    initialized = true;
    return result;
}

std::string info_print_compiler()
{
    auto const& info_comp = info_get_compiler();

    std::string res;
    res += "Compiler Information\n";
    res += "=============================================\n\n";
    res += "Platform        = " + info_comp.platform + "\n";
    res += "Compiler        = " + info_comp.compiler_family + " " + info_comp.compiler_version + "\n";
    res += "C++ version     = " + info_comp.cpp_version + "\n";
    res += "Build type      = " + info_comp.build_type  + "\n";
    res += "With OpenMP     = " + std::string( info_comp.with_openmp ? "true" : "false" ) + "\n";
    res += "With AVX        = " + std::string( info_comp.with_avx    ? "true" : "false" ) + "\n";
    res += "With AVX2       = " + std::string( info_comp.with_avx2   ? "true" : "false" ) + "\n";

    return res;
}

// =================================================================================================
//     Hardware Info
// =================================================================================================

// -----------------------------------------------------------------------------
//     OS Specific Helper Functions
// -----------------------------------------------------------------------------

/*
    The implementation of the hardware feature detaction is based on FeatureDetector,
    see https://github.com/Mysticial/FeatureDetector which is published under CC0 1.0 Universal.
    We have adapted the code to be contained in the classes and functions here.
 */

#if defined( _WIN32 ) || defined(  _WIN64  )

    void get_cpuid_(int32_t out[4], int32_t eax, int32_t ecx)
    {
        __cpuidex(out, eax, ecx);
    }

    __int64 xgetbv(unsigned int x)
    {
        return _xgetbv(x);
    }

    //  Detect 64-bit - Note that this snippet of code for detecting 64-bit has been copied from MSDN.
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    BOOL IsWow64()
    {
        BOOL bIsWow64 = FALSE;

        LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
            GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

        if (NULL != fnIsWow64Process)
        {
            if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
            {
                // printf("Error Detecting Operating System.\n");
                // printf("Defaulting to 32-bit OS.\n\n");
                bIsWow64 = FALSE;
            }
        }
        return bIsWow64;
    }

    bool detect_OS_x64_()
    {
        #ifdef _M_X64
            return true;
        #else
            return IsWow64() != 0;
        #endif
    }

#elif defined(__arm__) || defined(__aarch64__) || defined(__ARM_ARCH) // for M1/M2 chips

    void get_cpuid_( int32_t out[4], int32_t eax, int32_t ecx )
    {
        (void) eax;
        (void) ecx;

        // On ARM, for instance MacOS with M1/M2 chips, we do not detect any features at the moment.
        // That means we will just run vanilla code, but well, can't support everything.
        // Those processors seem to not have much support for SIMD anyway.
        out[0] = out[1] = out[2] = out[3] = 0;
    }

    uint64_t xgetbv(unsigned int index)
    {
        (void) index;
        return 0;
    }

    bool detect_OS_x64_()
    {
        // https://stackoverflow.com/a/41666292/4184258
        #ifdef __aarch64__
            return true;
        #else
            return false;
        #endif
    }

#else

    void get_cpuid_(int32_t out[4], int32_t eax, int32_t ecx)
    {
        __cpuid_count(eax, ecx, out[0], out[1], out[2], out[3]);
    }

    // Alternatiave from https://gist.github.com/hi2p-perim/7855506
    // void cpuid(int* cpuinfo, int info)
    // {
    //     __asm__ __volatile__(
    //         "xchg %%ebx, %%edi;"
    //         "cpuid;"
    //         "xchg %%ebx, %%edi;"
    //         :"=a" (cpuinfo[0]), "=D" (cpuinfo[1]), "=c" (cpuinfo[2]), "=d" (cpuinfo[3])
    //         :"0" (info)
    //     );
    // }

    uint64_t xgetbv(unsigned int index)
    {
        uint32_t eax, edx;
        __asm__ __volatile__("xgetbv" : "=a"(eax), "=d"(edx) : "c"(index));
        return ((uint64_t)edx << 32) | eax;
    }

    //  Detect 64-bit
    bool detect_OS_x64_() {
        //  We only support x64 on Linux.
        return true;
    }

#endif

#if defined(__linux__)

    static std::string get_cpu_info_linux_( std::string const& key )
    {
        std::string value = "(not found)";
        std::ifstream fs("/proc/cpuinfo");
        if( fs.good() ) {
            std::string line;
            while( !fs.eof() ) {
                std::getline(fs, line, '\n');
                if( strncmp(line.c_str(), key.c_str(), key.length()) == 0 ) {
                    size_t offset = key.length();
                    while(
                        ( isspace(line[offset]) || line[offset] == ':' ) &&
                        ( offset < line.length() )
                    ) {
                        ++offset;
                    }
                    value = line.c_str() + offset;
                    break;
                }
            }
        }

        return value;
    }

#endif

// -----------------------------------------------------------------------------
//     OS Feature Helper Functions
// -----------------------------------------------------------------------------

bool detect_OS_AVX_()
{
    //  Copied from: http://stackoverflow.com/a/22521619/922184
    bool avxSupported = false;

    int32_t cpuInfo[4];
    get_cpuid_(cpuInfo, 1, 0);

    bool osUsesXSAVE_XRSTORE = (cpuInfo[2] & (1 << 27)) != 0;
    bool cpuAVXSuport = (cpuInfo[2] & (1 << 28)) != 0;

    if( osUsesXSAVE_XRSTORE && cpuAVXSuport ) {
        auto const _XCR_XFEATURE_ENABLED_MASK = 0;
        uint64_t xcrFeatureMask = xgetbv(_XCR_XFEATURE_ENABLED_MASK);
        avxSupported = (xcrFeatureMask & 0x6) == 0x6;
    }

    return avxSupported;
}

bool detect_OS_AVX512_()
{
    if( !detect_OS_AVX_() ) {
        return false;
    }

auto const _XCR_XFEATURE_ENABLED_MASK = 0;
    uint64_t xcrFeatureMask = xgetbv(_XCR_XFEATURE_ENABLED_MASK);
    return (xcrFeatureMask & 0xe6) == 0xe6;
}

std::string get_vendor_string_()
{
    int32_t CPUInfo[4];
    char name[13];

    get_cpuid_(CPUInfo, 0, 0);
    memcpy(name + 0, &CPUInfo[1], 4);
    memcpy(name + 4, &CPUInfo[3], 4);
    memcpy(name + 8, &CPUInfo[2], 4);
    name[12] = '\0';

    return name;
}

std::string get_cpu_model_()
{
    std::string model = "unknown CPU model";
    #if defined(__linux__)
        model = get_cpu_info_linux_("model name");
    #elif defined(__APPLE__)
        char str[256];
        size_t len = 256;
        if( sysctlbyname( "machdep.cpu.brand_string", &str, &len, NULL, 0 ) == 0 ) {
            model = str;
        }
    #endif
    return model;
}

#if defined(__linux__)

    size_t get_memtotal_linux_()
    {
        // Using http://stackoverflow.com/a/1312957/4184258
        // And https://github.com/amkozlov/raxml-ng/blob/master/src/util/sysutil.cpp

        struct sysinfo memInfo;
        if( sysinfo( &memInfo )) {
            return 0;
        }
        return static_cast<size_t>( memInfo.totalram ) * static_cast<size_t>( memInfo.mem_unit );
    }

#endif

size_t get_memtotal_()
{
    // Adapted from https://github.com/amkozlov/raxml-ng/blob/master/src/util/sysutil.cpp
    // which itself is seems to be adapted from vsearch.

    #if defined( _WIN32 ) || defined( _WIN64  )|| defined( _MSC_VER )

        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        return memInfo.ullTotalPhys;

    #elif defined(_SC_PHYS_PAGES) && defined(_SC_PAGESIZE)

        long phys_pages = sysconf(_SC_PHYS_PAGES);
        long pagesize = sysconf(_SC_PAGESIZE);

        if ((phys_pages == -1) || (pagesize == -1)) {
            #if defined(__linux__)
                return get_memtotal_linux_();
            #else
                return 0;
            #endif
        }

        // sysconf(3) notes that pagesize * phys_pages can overflow, such as when long is
        // 32-bits and there's more than 4GB RAM. Since we target LP64 systems like
        // x86_64 linux, this will not arise in practice on the intended platform.
        if (pagesize > LONG_MAX / phys_pages) {
            return LONG_MAX;
        } else {
            return static_cast<size_t>( pagesize ) * static_cast<size_t>( phys_pages );
        }

    #elif defined(__APPLE__)

        int mib[] = { CTL_HW, HW_MEMSIZE };
        int64_t ram = 0;
        size_t length = sizeof(ram);
        if (-1 == sysctl(mib, 2, &ram, &length, NULL, 0)) {
            return 0;
        }
        return ram;

    #elif defined(__linux__)
        // Generic solution
        return get_memtotal_linux_();
    #else
        return 0;
    #endif
}

// -----------------------------------------------------------------------------
//     info_get_hardware
// -----------------------------------------------------------------------------

InfoHardware const& info_get_hardware()
{
    // No need to repeat.
    static InfoHardware result;
    static bool initialized = false;
    if( initialized ) {
        return result;
    }

    // Endianness
    static const uint16_t little_endian_test = 0x1000;
    result.is_little_endian = ( 0 == *reinterpret_cast< uint8_t const* >( &little_endian_test ));
    // static const uint16_t big_endian_test = 0x0001;
    // result.is_little_endian = ( 0 == *reinterpret_cast< uint8_t const* >( &big_endian_test ));

    // Memory
    result.total_memory = get_memtotal_();

    // ---------------------------------------------------------
    //     Vendor and OS
    // ---------------------------------------------------------

    //  Vendor
    result.vendor_string = get_vendor_string_();
    if( result.vendor_string == "GenuineIntel" ) {
        result.vendor_Intel = true;
    } else if( result.vendor_string == "AuthenticAMD" ) {
        result.vendor_AMD = true;
    }
    result.cpu_model = get_cpu_model_();
    result.physical_core_count = info_physical_core_count();
    result.with_hyperthreading = info_hyperthreads_enabled();

    //  OS Features
    result.OS_x64 = detect_OS_x64_();
    result.OS_AVX = detect_OS_AVX_();
    result.OS_AVX512 = detect_OS_AVX512_();

    // ---------------------------------------------------------
    //     Detect Features
    // ---------------------------------------------------------

    // Not used, but kept here for future reference if needed:
    // Windows Visual Studio detect AVX: https://insufficientlycomplicated.wordpress.com/2011/11/07/detecting-intel-advanced-vector-extensions-avx-in-visual-studio/
    // See also https://stackoverflow.com/a/28441544/4184258

    // Now run the feature detection

    int32_t info[4];
    get_cpuid_( info, 0, 0 );
    int nIds = info[0];

    get_cpuid_( info, 0x80000000, 0 );
    uint32_t nExIds = info[0];

    if( nIds >= 0x00000001 ) {
        get_cpuid_( info, 0x00000001, 0 );
        result.HW_MMX    = (info[3] & ((int)1 << 23)) != 0;
        result.HW_SSE    = (info[3] & ((int)1 << 25)) != 0;
        result.HW_SSE2   = (info[3] & ((int)1 << 26)) != 0;
        result.HW_SSE3   = (info[2] & ((int)1 <<  0)) != 0;

        result.HW_SSSE3  = (info[2] & ((int)1 <<  9)) != 0;
        result.HW_SSE41  = (info[2] & ((int)1 << 19)) != 0;
        result.HW_SSE42  = (info[2] & ((int)1 << 20)) != 0;
        result.HW_AES    = (info[2] & ((int)1 << 25)) != 0;

        result.HW_AVX    = (info[2] & ((int)1 << 28)) != 0;
        result.HW_FMA3   = (info[2] & ((int)1 << 12)) != 0;

        result.HW_RDRAND = (info[2] & ((int)1 << 30)) != 0;
    }
    if( nIds >= 0x00000007 ) {
        get_cpuid_( info, 0x00000007, 0 );
        result.HW_AVX2         = (info[1] & ((int)1 <<  5)) != 0;

        result.HW_BMI1         = (info[1] & ((int)1 <<  3)) != 0;
        result.HW_BMI2         = (info[1] & ((int)1 <<  8)) != 0;
        result.HW_ADX          = (info[1] & ((int)1 << 19)) != 0;
        result.HW_MPX          = (info[1] & ((int)1 << 14)) != 0;
        result.HW_SHA          = (info[1] & ((int)1 << 29)) != 0;
        result.HW_RDSEED       = (info[1] & ((int)1 << 18)) != 0;
        result.HW_PREFETCHWT1  = (info[2] & ((int)1 <<  0)) != 0;
        result.HW_RDPID        = (info[2] & ((int)1 << 22)) != 0;

        result.HW_AVX512_F     = (info[1] & ((int)1 << 16)) != 0;
        result.HW_AVX512_CD    = (info[1] & ((int)1 << 28)) != 0;
        result.HW_AVX512_PF    = (info[1] & ((int)1 << 26)) != 0;
        result.HW_AVX512_ER    = (info[1] & ((int)1 << 27)) != 0;

        result.HW_AVX512_VL    = (info[1] & ((int)1 << 31)) != 0;
        result.HW_AVX512_BW    = (info[1] & ((int)1 << 30)) != 0;
        result.HW_AVX512_DQ    = (info[1] & ((int)1 << 17)) != 0;

        result.HW_AVX512_IFMA  = (info[1] & ((int)1 << 21)) != 0;
        result.HW_AVX512_VBMI  = (info[2] & ((int)1 <<  1)) != 0;

        result.HW_AVX512_VPOPCNTDQ = (info[2] & ((int)1 << 14)) != 0;
        result.HW_AVX512_4VNNIW    = (info[3] & ((int)1 <<  2)) != 0;
        result.HW_AVX512_4FMAPS    = (info[3] & ((int)1 <<  3)) != 0;

        result.HW_AVX512_VNNI      = (info[2] & ((int)1 << 11)) != 0;

        result.HW_AVX512_VBMI2     = (info[2] & ((int)1 <<  6)) != 0;
        result.HW_GFNI             = (info[2] & ((int)1 <<  8)) != 0;
        result.HW_VAES             = (info[2] & ((int)1 <<  9)) != 0;
        result.HW_AVX512_VPCLMUL   = (info[2] & ((int)1 << 10)) != 0;
        result.HW_AVX512_BITALG    = (info[2] & ((int)1 << 12)) != 0;

        get_cpuid_( info, 0x00000007, 1 );
        result.HW_AVX512_BF16      = (info[0] & ((int)1 <<  5)) != 0;
    }
    if( nExIds >= 0x80000001 ) {
        get_cpuid_( info, 0x80000001, 0 );
        result.HW_x64   = (info[3] & ((int)1 << 29)) != 0;
        result.HW_ABM   = (info[2] & ((int)1 <<  5)) != 0;
        result.HW_SSE4a = (info[2] & ((int)1 <<  6)) != 0;
        result.HW_FMA4  = (info[2] & ((int)1 << 16)) != 0;
        result.HW_XOP   = (info[2] & ((int)1 << 11)) != 0;
        result.HW_PREFETCHW = (info[2] & ((int)1 << 8)) != 0;
    }

    initialized = true;
    return result;
}

// -----------------------------------------------------------------------------
//     info_print_hardware
// -----------------------------------------------------------------------------

std::string info_print_hardware( bool full )
{
    auto const& info_hardware = info_get_hardware();

    std::stringstream ss;
    auto print_ = [&]( char const* label, bool yes )
    {
        ss << label;
        ss << (yes ? "true" : "false") << "\n";
    };

    ss << "Hardware Features\n";
    ss << "=============================================\n\n";

    ss << "Memory:" << "\n";
    ss << "    Memory        = " << to_string_byte_format( info_hardware.total_memory ) << "\n";
    print_("    64-bit        = ", info_hardware.OS_x64);
    print_("    Little endian = ", info_hardware.is_little_endian);
    ss << "\n";

    ss << "CPU Vendor:" << "\n";
    ss << "    Vendor        = " << info_hardware.vendor_string << "\n";
    ss << "    CPU model     = " << info_hardware.cpu_model << "\n";
    print_("    AMD           = ", info_hardware.vendor_AMD);
    print_("    Intel         = ", info_hardware.vendor_Intel);
    ss << "    Cores         = " << info_hardware.physical_core_count << "\n";
    print_("    Hyperthreads  = ", info_hardware.with_hyperthreading);
    ss << "\n";

    if( full ) {
        ss << "OS Features:" << "\n";
        print_("    OS AVX      = ", info_hardware.OS_AVX);
        print_("    OS AVX512   = ", info_hardware.OS_AVX512);
        ss << "\n";

        ss << "Hardware Features:" << "\n";
        print_("    MMX         = ", info_hardware.HW_MMX);
        print_("    x64         = ", info_hardware.HW_x64);
        print_("    ABM         = ", info_hardware.HW_ABM);
        print_("    RDRAND      = ", info_hardware.HW_RDRAND);
        print_("    RDSEED      = ", info_hardware.HW_RDSEED);
        print_("    BMI1        = ", info_hardware.HW_BMI1);
        print_("    BMI2        = ", info_hardware.HW_BMI2);
        print_("    ADX         = ", info_hardware.HW_ADX);
        print_("    MPX         = ", info_hardware.HW_MPX);
        print_("    PREFETCHW   = ", info_hardware.HW_PREFETCHW);
        print_("    PREFETCHWT1 = ", info_hardware.HW_PREFETCHWT1);
        print_("    RDPID       = ", info_hardware.HW_RDPID);
        print_("    GFNI        = ", info_hardware.HW_GFNI);
        print_("    VAES        = ", info_hardware.HW_VAES);
        ss << "\n";

        ss << "SIMD: 128-bit" << "\n";
        print_("    SSE         = ", info_hardware.HW_SSE);
        print_("    SSE2        = ", info_hardware.HW_SSE2);
        print_("    SSE3        = ", info_hardware.HW_SSE3);
        print_("    SSSE3       = ", info_hardware.HW_SSSE3);
        print_("    SSE4a       = ", info_hardware.HW_SSE4a);
        print_("    SSE4.1      = ", info_hardware.HW_SSE41);
        print_("    SSE4.2      = ", info_hardware.HW_SSE42);
        print_("    AES-NI      = ", info_hardware.HW_AES);
        print_("    SHA         = ", info_hardware.HW_SHA);
        ss << "\n";

        ss << "SIMD: 256-bit" << "\n";
        print_("    AVX         = ", info_hardware.HW_AVX);
        print_("    XOP         = ", info_hardware.HW_XOP);
        print_("    FMA3        = ", info_hardware.HW_FMA3);
        print_("    FMA4        = ", info_hardware.HW_FMA4);
        print_("    AVX2        = ", info_hardware.HW_AVX2);
        ss << "\n";

        ss << "SIMD: 512-bit" << "\n";
        print_("    AVX512-F         = ", info_hardware.HW_AVX512_F);
        print_("    AVX512-CD        = ", info_hardware.HW_AVX512_CD);
        print_("    AVX512-PF        = ", info_hardware.HW_AVX512_PF);
        print_("    AVX512-ER        = ", info_hardware.HW_AVX512_ER);
        print_("    AVX512-VL        = ", info_hardware.HW_AVX512_VL);
        print_("    AVX512-BW        = ", info_hardware.HW_AVX512_BW);
        print_("    AVX512-DQ        = ", info_hardware.HW_AVX512_DQ);
        print_("    AVX512-IFMA      = ", info_hardware.HW_AVX512_IFMA);
        print_("    AVX512-VBMI      = ", info_hardware.HW_AVX512_VBMI);
        print_("    AVX512-VPOPCNTDQ = ", info_hardware.HW_AVX512_VPOPCNTDQ);
        print_("    AVX512-4VNNIW    = ", info_hardware.HW_AVX512_4VNNIW);
        print_("    AVX512-4FMAPS    = ", info_hardware.HW_AVX512_4FMAPS);
        print_("    AVX512-VBMI2     = ", info_hardware.HW_AVX512_VBMI2);
        print_("    AVX512-VPCLMUL   = ", info_hardware.HW_AVX512_VPCLMUL);
        print_("    AVX512-VNNI      = ", info_hardware.HW_AVX512_VNNI);
        print_("    AVX512-BITALG    = ", info_hardware.HW_AVX512_BITALG);
        print_("    AVX512-BF16      = ", info_hardware.HW_AVX512_BF16);
        ss << "\n";
    }

    ss << "SIMD Summary:" << "\n";
    print_("    Safe to use AVX:     ", info_use_avx());
    print_("    Safe to use AVX2:    ", info_use_avx2());
    print_("    Safe to use AVX512:  ", info_use_avx512());
    ss << "\n";

    return ss.str();
}

bool info_use_avx()
{
    // Initialized on first use.
    static const bool avx_is_safe_ =
        info_get_compiler().with_avx &&
        info_get_hardware().HW_AVX &&
        info_get_hardware().OS_AVX
    ;
    return avx_is_safe_;
}

bool info_use_avx2()
{
    // Initialized on first use.
    static const bool avx2_is_safe_ =
        info_get_compiler().with_avx2 &&
        info_get_hardware().HW_AVX2 &&
        info_get_hardware().OS_AVX
    ;
    return avx2_is_safe_;
}

bool info_use_avx512()
{
    // Initialized on first use.
    static const bool avx512_is_safe_ =
        info_get_compiler().with_avx512 &&
        info_get_hardware().HW_AVX512_F &&
        info_get_hardware().OS_AVX512
    ;
    return avx512_is_safe_;
}

// =================================================================================================
//     Number of Threads
// =================================================================================================

unsigned int info_task_cpu_cores( bool physical = false )
{
    // Adapted from https://github.com/amkozlov/raxml-ng/blob/master/src/util/sysutil.cpp
    auto ncores = std::thread::hardware_concurrency();

    #if defined(__linux__)
        cpu_set_t mask;
        if (sched_getaffinity(0, sizeof(cpu_set_t), &mask) != -1) {
            ncores = CPU_COUNT(&mask);
        }
    #endif

    if (physical) {
        auto const threads_per_core = info_hyperthreads_enabled() ? 2 : 1;
        ncores /= threads_per_core;
    }

    return ncores;
}

long info_online_cpu_cores()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

size_t read_id_from_file_( std::string const& filename )
{
    std::ifstream f(filename);
    if( f.good() ) {
        size_t id;
        f >> id;
        return id;
    } else {
        throw std::runtime_error("couldn't open sys files");
    }
}

size_t get_numa_node_id_( std::string const& cpu_path )
{
    // Adapted from https://github.com/amkozlov/raxml-ng/blob/master/src/util/sysutil.cpp
    // This is ugly, but should be reliable -> please blame Linux kernel developers & Intel!
    std::string node_path = cpu_path + "../node";
    for (size_t i = 0; i < 1000; ++i) {
        if( is_dir(node_path + std::to_string(i)) ) {
            return i;
        }
    }

    // Fallback solution: return socket_id which is often identical to numa id
    return read_id_from_file_(cpu_path + "physical_package_id");
}

size_t get_core_id_( std::string const& cpu_path )
{
    return read_id_from_file_(cpu_path + "core_id");
}

int get_physical_core_count_( size_t n_cpu )
{
    // Adapted from https://github.com/amkozlov/raxml-ng/blob/master/src/util/sysutil.cpp
    #if defined(__linux__)
        std::unordered_set<size_t> cores;
        for (size_t i = 0; i < n_cpu; ++i) {
            std::string cpu_path = "/sys/devices/system/cpu/cpu" + std::to_string(i) + "/topology/";
            size_t core_id = get_core_id_(cpu_path);
            size_t node_id = get_numa_node_id_(cpu_path);
            size_t uniq_core_id = (node_id << 16) + core_id;
            cores.insert(uniq_core_id);
        }
        return cores.size();
    #else
        (void) n_cpu;
        return 0;
    #endif
}

size_t info_physical_core_count()
{
    auto const hw_cores = std::thread::hardware_concurrency();
    try {
        // Try to get the physical cores, might fail and throw.
        auto const phys_cores = get_physical_core_count_( hw_cores );
        if( phys_cores > 0 ) {
            return phys_cores;
        }
    } catch ( ... ) {
        // Do nothing, just continue with the alternative.
    }

    // If the above did not work, use the fallback instead.
    auto const threads_per_core = info_hyperthreads_enabled() ? 2 : 1;
    return hw_cores / threads_per_core;
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

size_t guess_number_of_threads( bool use_openmp, bool use_slurm, bool physical_cores )
{
    // Dummy to avoid compiler warnings.
    (void) use_openmp;

    // Default to 1 thread. Will be overwritten later.
    size_t guess = 1;

    // Initialize threads with actual number of cores.
    // The function might return 0 if no number could be determined, in which case we default to 1.
    if( physical_cores ) {
        auto const phys_cores = info_physical_core_count();
        if( phys_cores > 0 ) {
            guess = phys_cores;
        }
    } else {
        auto const hw_concur = std::thread::hardware_concurrency();
        if( hw_concur > 0 ) {
            guess = static_cast<size_t>( hw_concur );
        }
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

        // By default, OpenMP uses something like hardware_concurrency, which might include
        // hyperthreads, and hence mess up this setup. So we catch that special case.
        auto const hw_concur = std::thread::hardware_concurrency();
        if( openmp_threads > 0 && openmp_threads == hw_concur && physical_cores ) {
            // We do not use the OpenMP threads for the guess in that case.
            auto const threads_per_core = info_hyperthreads_enabled() ? 2 : 1;
            guess = hw_concur / threads_per_core;
        } else if( openmp_threads > 0 ) {
            // Here, OpenMP is set to some non-zero number that is not just the hardware_concurrency
            guess = static_cast<size_t>( openmp_threads );
        }
    }

    assert( guess > 0 );
    return guess;
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
//     Current Resource Usage
// =================================================================================================

// -------------------------------------------------------------------------
//     Open File Counts
// -------------------------------------------------------------------------

size_t info_process_max_file_count()
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

size_t info_process_current_file_count()
{
    // Adapted from https://web.archive.org/web/20150326011742/
    // http://blog.lobstertechnology.com/2005/08/22/determine-the-number-of-open-files/

    // We loop over all possible file descriptor numbers...
    auto const max_fd_cnt = info_process_max_file_count();

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

#if defined( _WIN32 ) || defined( _WIN64  )|| defined( _MSC_VER )

    // -------------------------------------------------------------------------
    //     Current Mem/CPU - Windows
    // -------------------------------------------------------------------------

    size_t info_process_current_memory_usage()
    {
        // https://stackoverflow.com/q/63166/4184258
        // Beware: Completely untested, no error checking.
        // Kept here mostly for future reference.

        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
        return pmc.WorkingSetSize;
    }

    size_t info_system_current_memory_usage()
    {
        // https://stackoverflow.com/q/63166/4184258
        // Beware: Completely untested, no error checking.
        // Kept here mostly for future reference.

        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        assert( memInfo.ullTotalPhys >= memInfo.ullAvailPhys );
        return memInfo.ullTotalPhys - memInfo.ullAvailPhys;
    }

    size_t info_system_current_memory_available()
    {
        // Based on https://stackoverflow.com/q/63166/4184258
        // Beware: Completely untested, no error checking.
        // Kept here mostly for future reference.

        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        return memInfo.ullAvailPhys;
    }

    double info_process_current_cpu_usage( bool all_cores, bool percent )
    {
        // https://stackoverflow.com/q/63166/4184258
        // Beware: Completely untested, no error checking.
        // Kept here mostly for future reference.

        static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
        static int num_processors;
        static HANDLE self;
        static bool initialized = false;

        if( ! initialized ) {
            SYSTEM_INFO sysInfo;
            FILETIME ftime, fsys, fuser;

            GetSystemInfo(&sysInfo);
            num_processors = sysInfo.dwNumberOfProcessors;

            GetSystemTimeAsFileTime(&ftime);
            memcpy(&lastCPU, &ftime, sizeof(FILETIME));

            self = GetCurrentProcess();
            GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
            memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
            memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));

            initialized = true;
            return 0.0;
        }

        FILETIME ftime, fsys, fuser;
        ULARGE_INTEGER now, sys, user;
        double result;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&now, &ftime, sizeof(FILETIME));

        GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
        memcpy(&sys, &fsys, sizeof(FILETIME));
        memcpy(&user, &fuser, sizeof(FILETIME));
        result = (sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart);
        result /= (now.QuadPart - lastCPU.QuadPart);
        if( ! all_cores ) {
            result /= num_processors;
        }
        if( percent ) {
            result *= 100;
        }

        lastCPU = now;
        lastUserCPU = user;
        lastSysCPU = sys;

        return result;
    }

    double info_system_current_cpu_usage( bool all_cores, bool percent )
    {
        // Adapted from https://stackoverflow.com/q/63166/4184258
        // Beware: Completely untested, no error checking.
        // Kept here mostly for future reference.

        static PDH_HQUERY cpuQuery;
        static PDH_HCOUNTER cpuTotal;
        static int num_processors;
        static bool initialized = false;

        if( ! initialized ) {
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            num_processors = sysInfo.dwNumberOfProcessors;

            PdhOpenQuery(NULL, NULL, &cpuQuery);
            // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values
            // with PdhGetFormattedCounterArray()
            PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
            PdhCollectQueryData(cpuQuery);

            initialized = true;
        }

        PDH_FMT_COUNTERVALUE counterVal;
        PdhCollectQueryData(cpuQuery);
        PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);

        auto result = counterVal.doubleValue;
        if( !all_cores ) {
            result /= num_processors;
        }
        if( !percent ) {
            result /= 100.0;
        }
        return result;
    }

#elif defined( __APPLE__ )

    // -------------------------------------------------------------------------
    //     Current Mem/CPU - Apple
    // -------------------------------------------------------------------------

    size_t info_process_current_memory_usage()
    {
        // https://stackoverflow.com/q/63166/4184258

        struct task_basic_info t_info;
        mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

        auto const ret = task_info(
            mach_task_self(), TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count
        );
        if( KERN_SUCCESS != ret ) {
            return 0;
        }
        // resident size is in t_info.resident_size;
        // virtual size is in t_info.virtual_size;
        return t_info.resident_size;
    }

    size_t info_system_current_memory_usage()
    {
        // Adapted from https://stackoverflow.com/a/1911863/4184258

        vm_size_t page_size;
        mach_port_t mach_port;
        mach_msg_type_number_t count;
        vm_statistics64_data_t vm_stats;

        mach_port = mach_host_self();
        count = sizeof(vm_stats) / sizeof(natural_t);
        if(
            KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
            KERN_SUCCESS == host_statistics64(
                mach_port, HOST_VM_INFO, (host_info64_t)&vm_stats, &count
            )
        ) {
            // Used memory has several types of relevant pages that we need to take into account.
            auto const relevant_sum =
                static_cast<size_t>( vm_stats.active_count ) +
                static_cast<size_t>( vm_stats.inactive_count ) +
                static_cast<size_t>( vm_stats.wire_count )
            ;
            return relevant_sum * static_cast<size_t>( page_size );
        }

        return 0;
    }

    size_t info_system_current_memory_available()
    {
        // Adapted from https://stackoverflow.com/a/1911863/4184258

        vm_size_t page_size;
        mach_port_t mach_port;
        mach_msg_type_number_t count;
        vm_statistics64_data_t vm_stats;

        mach_port = mach_host_self();
        count = sizeof(vm_stats) / sizeof(natural_t);
        if(
            KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
            KERN_SUCCESS == host_statistics64(
                mach_port, HOST_VM_INFO, (host_info64_t)&vm_stats, &count
            )
        ) {
            return static_cast<size_t>( vm_stats.free_count ) * static_cast<size_t>( page_size );
        }
        return 0;
    }

    size_t info_process_number_of_processors_()
    {
        // This is kind of redundant from the other functions...
        // Well, but we keep it this way for now for compatibility with the original code.

        host_basic_info_data_t hostInfo;
        mach_msg_type_number_t hostCount = HOST_BASIC_INFO_COUNT;
        auto const hi = host_info(
            mach_host_self(), HOST_BASIC_INFO, (host_info_t)&hostInfo, &hostCount
        );
        if( hi != KERN_SUCCESS ) {
            return 0;
        }
        return hostInfo.avail_cpus;
    }

    double info_process_current_cpu_usage( bool all_cores, bool percent )
    {
        // No implementation from stack overflow :-(
        // Hopefully, this one works though, provided by ChatGPT.

        static uint64_t last_total_time = 0;
        static struct timeval last_time;
        static int num_processors = 0;
        static bool initialized = false;

        struct task_thread_times_info thread_times;
        mach_msg_type_number_t count = TASK_THREAD_TIMES_INFO_COUNT;

        if( !initialized ) {
            gettimeofday(&last_time, NULL);
            auto const ti = task_info(
                mach_task_self(), TASK_THREAD_TIMES_INFO, (task_info_t)&thread_times, &count
            );
            if( ti != KERN_SUCCESS ) {
                return 0.0;
            }
            last_total_time =
                thread_times.user_time.seconds * 1000000 +
                thread_times.user_time.microseconds +
                thread_times.system_time.seconds * 1000000 +
                thread_times.system_time.microseconds
            ;

            // Get the number of processors
            num_processors = info_process_number_of_processors_();

            initialized = true;
            return 0.0;
        }

        struct timeval current_time;
        gettimeofday(&current_time, NULL);
        auto const ti = task_info(
            mach_task_self(), TASK_THREAD_TIMES_INFO, (task_info_t)&thread_times, &count
        );
        if( ti != KERN_SUCCESS ) {
            return 0.0;
        }
        uint64_t total_time =
            thread_times.user_time.seconds * 1000000 +
            thread_times.user_time.microseconds +
            thread_times.system_time.seconds * 1000000 +
            thread_times.system_time.microseconds
        ;
        uint64_t elapsed_time =
            (current_time.tv_sec - last_time.tv_sec) * 1000000 +
            (current_time.tv_usec - last_time.tv_usec)
        ;
        double result = (total_time - last_total_time) / static_cast<double>(elapsed_time);
        if( all_cores ) {
            result *= num_processors;
        }
        if( percent ) {
            result *= 100;
        }

        last_time = current_time;
        last_total_time = total_time;

        return result;
    }

    double info_system_current_cpu_usage( bool all_cores, bool percent )
    {
        // Adapted from https://stackoverflow.com/a/49996245/4184258

        static size_t _previousTotalTicks = 0;
        static size_t _previousIdleTicks = 0;
        static int num_processors = 0;
        static bool initialized = false;

        if( ! initialized ) {
            num_processors = info_process_number_of_processors_();
            initialized = true;
            return 0.0;
        }

        auto calculate_CPU_load_ = [&](size_t idleTicks, size_t totalTicks)
        {
            size_t totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
            size_t idleTicksSinceLastTime  = idleTicks  - _previousIdleTicks;
            _previousTotalTicks = totalTicks;
            _previousIdleTicks  = idleTicks;

            if( totalTicksSinceLastTime == 0 ) {
                return 1.0;
            }
            return 1.0 - static_cast<double>( idleTicksSinceLastTime ) / totalTicksSinceLastTime;
        };

        // Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in between
        // You'll need to call this at regular intervals, since it measures the load between
        // the previous call and the current one.
        host_cpu_load_info_data_t cpuinfo;
        mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
        if(
            host_statistics(
                mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuinfo, &count
            ) != KERN_SUCCESS
        ) {
            return 0.0;
        }

        size_t totalTicks = 0;
        for( int i = 0; i < CPU_STATE_MAX; i++ ) {
            totalTicks += cpuinfo.cpu_ticks[i];
        }
        auto result = calculate_CPU_load_( cpuinfo.cpu_ticks[CPU_STATE_IDLE], totalTicks );
        if( all_cores ) {
            result *= num_processors;
        }
        if( percent ) {
            result *= 100.0;
        }
        return result;
    }

#elif defined( __GNUC__ ) || defined( __clang__ ) || defined(__linux__)

    // -------------------------------------------------------------------------
    //     Current Mem/CPU - Linux
    // -------------------------------------------------------------------------

    size_t parse_proc_line_kb_( char* line )
    {
        // Helper function to parse "/proc/self/status", see
        // https://stackoverflow.com/q/63166/4184258
        // Adapted from there to add error checking.


        // Check if the line is too short to contain " kB" and a value
        auto length = strlen(line);
        if( length < 5 ) {
            return 0;
        }

        // Need to remove new line from end if present
        if( line[length - 1] == '\n' ) {
            line[length - 1] = '\0';
            --length;
        }

        // Check that the string ends with " kB" (case-insensitive)
        if( strncasecmp( &line[length - 3], " kB", 3 ) != 0 ) {
            return 0;
        }

        // Null-terminate the string to remove the " kB"
        line[length - 3] = '\0';

        // Find the first digit in the string
        char* p = line;
        while( *p && !isdigit(*p) ) {
            p++;
        }

        // Check if we found a digit, then convert the numeric part of the string to an integer
        if( *p == '\0' ) {
            return 0;
        }
        return atoi(p);
    }

    size_t info_process_current_memory_usage()
    {
        // https://stackoverflow.com/q/63166/4184258

        //Note: this value is in KB!
        FILE* file = fopen("/proc/self/status", "r");
        size_t result = 0;
        char line[128];
        if( !file ) {
            return 0;
        }

        while( fgets( line, 128, file ) != NULL ) {
            if( strncmp( line, "VmRSS:", 6 ) == 0 ) {
                result = parse_proc_line_kb_(line);
                break;
            }
        }
        fclose(file);
        return result * 1024;
    }

    std::unordered_map<std::string, size_t> get_proc_meminfo_lines_()
    {
        std::unordered_map<std::string, size_t> meminfo;
        std::ifstream file("/proc/meminfo");
        std::string line;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string key;
            size_t value;
            std::string unit;
            iss >> key >> value >> unit;
            key.pop_back(); // Remove trailing ':'
            meminfo[key] = value * 1024; // Convert kB to bytes
        }

        return meminfo;
    }

    size_t info_system_current_memory_helper_( bool available )
    {
        // Use a more comprehensive approach based on proc files, that also accounts for
        // buffered and cached memory pages.
        try {
            // We purposefully do not make meminfo const here, so that any failing
            // lookups instead simply add the element as 0, which is an okay fallback.
            size_t mem_avail = 0;
            auto meminfo = get_proc_meminfo_lines_();
            if( meminfo.count( "MemAvailable" ) > 0 ) {
                mem_avail = meminfo["MemAvailable"];
            } else {
                // If MemAvailable is not found, fallback to calculating it manually
                size_t memFree = meminfo["MemFree"];
                size_t buffers = meminfo["Buffers"];
                size_t cached  = meminfo["Cached"];
                mem_avail = memFree + buffers + cached;
            }
            if( available ) {
                return mem_avail;
            } else if( meminfo.count( "MemTotal" ) != 0 ) {
                // If this is not available, we use the below fallback instead.
                return meminfo["MemTotal"] - mem_avail;
            }
        } catch( ... ) {
            // Do nothing here, just follow up with the below fallback.
        }

        // If the above does not work, we use this as another fallback, which however
        // only reports the completely free memory, disregarind buffers and caches.
        // Adapted from https://stackoverflow.com/q/63166/4184258
        struct sysinfo memInfo;
        if( sysinfo( &memInfo )) {
            return 0;
        }
        auto const mem_unit = static_cast<size_t>( memInfo.mem_unit );
        if( available ) {
            return static_cast<size_t>( memInfo.freeram ) * mem_unit;
        } else {
            return static_cast<size_t>( memInfo.totalram - memInfo.freeram ) * mem_unit;
        }
        return 0;
    }

    size_t info_system_current_memory_usage()
    {
        return info_system_current_memory_helper_( false );
    }

    size_t info_system_current_memory_available()
    {
        return info_system_current_memory_helper_( true );
    }

    size_t info_process_number_of_processors_()
    {
        // This is kind of redundant from the other functions...
        // Well, but we keep it this way for now for compatibility with the original code.

        size_t num_processors = 0;
        FILE* file = fopen("/proc/cpuinfo", "r");
        if( ! file ) {
            return 0;
        }
        char line[128];
        while( fgets(line, 128, file) != NULL) {
            if( strncmp(line, "processor", 9) == 0 ) {
                ++num_processors;
            }
        }
        fclose(file);
        return num_processors;
    }

    double info_process_current_cpu_usage( bool all_cores, bool percent )
    {
        // https://stackoverflow.com/q/63166/4184258

        static clock_t lastCPU, lastSysCPU, lastUserCPU;
        static int num_processors;
        static bool initialized = false;

        if( ! initialized ) {
            struct tms timeSample;
            lastCPU = times(&timeSample);
            lastSysCPU = timeSample.tms_stime;
            lastUserCPU = timeSample.tms_utime;
            num_processors = info_process_number_of_processors_();

            initialized = true;
            return 0.0;
        }

        struct tms timeSample;
        clock_t now;
        double result;

        now = times(&timeSample);
        if(
            now <= lastCPU ||
            timeSample.tms_stime < lastSysCPU ||
            timeSample.tms_utime < lastUserCPU
        ) {
            //Overflow detection. Just skip this value.
            result = 0.0;
        } else {
            result = (timeSample.tms_stime - lastSysCPU) + (timeSample.tms_utime - lastUserCPU);
            result /= (now - lastCPU);
            if( ! all_cores ) {
                result /= num_processors;
            }
            if( percent ) {
                result *= 100;
            }
        }
        lastCPU = now;
        lastSysCPU = timeSample.tms_stime;
        lastUserCPU = timeSample.tms_utime;

        return result;
    }

    double info_system_current_cpu_usage( bool all_cores, bool percent )
    {
        // Adapted from https://stackoverflow.com/q/63166/4184258

        static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;
        static int num_processors;
        static bool initialized = false;

        if( ! initialized ) {
            FILE* file = fopen("/proc/stat", "r");
            if( ! file ) {
                return 0.0;
            }

            auto const scanned = fscanf(
                file, "cpu %llu %llu %llu %llu",
                &lastTotalUser, &lastTotalUserLow, &lastTotalSys, &lastTotalIdle
            );
            fclose(file);
            num_processors = info_process_number_of_processors_();
            if( scanned != 4 ) {
                return 0.0;
            }

            initialized = true;
            return 0.0;
        }

        double result;
        FILE* file;
        unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

        file = fopen("/proc/stat", "r");
        if( ! file ) {
            return 0.0;
        }
        auto const scanned = fscanf(
            file, "cpu %llu %llu %llu %llu",
            &totalUser, &totalUserLow, &totalSys, &totalIdle
        );
        fclose(file);
        if( scanned != 4 ) {
            return 0.0;
        }

        if(
            totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
            totalSys < lastTotalSys || totalIdle < lastTotalIdle
        ) {
            //Overflow detection. Just skip this value.
            result = 0.0;
        } else {
            total =
                (totalUser    - lastTotalUser) +
                (totalUserLow - lastTotalUserLow) +
                (totalSys     - lastTotalSys)
            ;
            result = total;
            total += (totalIdle - lastTotalIdle);
            result /= total;
            if( all_cores ) {
                result *= num_processors;
            }
            if( percent ) {
                result *= 100;
            }
        }

        lastTotalUser = totalUser;
        lastTotalUserLow = totalUserLow;
        lastTotalSys = totalSys;
        lastTotalIdle = totalIdle;

        return result;
    }

#else

    // -------------------------------------------------------------------------
    //     Current Mem/CPU - Default
    // -------------------------------------------------------------------------

    size_t info_process_current_memory_usage()
    {
        return 0;
    }

    size_t info_system_current_memory_usage()
    {
        return 0;
    }

    size_t info_system_current_memory_available()
    {
        return 0;
    }

    double info_process_current_cpu_usage( bool all_cores, bool percent )
    {
        (void) all_cores;
        (void) percent;
        return 0.0;
    }

    double info_system_current_cpu_usage( bool all_cores, bool percent )
    {
        (void) all_cores;
        (void) percent;
        return 0.0;
    }

#endif

// =================================================================================================
//     Total Resource Usage
// =================================================================================================

size_t info_process_peak_memory_usage()
{
    // Adapted from https://github.com/amkozlov/raxml-ng/blob/master/src/util/sysutil.cpp

    struct rusage r_usage;
    getrusage(RUSAGE_SELF, &r_usage);

    #if defined __APPLE__
        // Mac: ru_maxrss gives the size in bytes
        return static_cast<size_t>( r_usage.ru_maxrss );
    #else
        // Linux: ru_maxrss gives the size in kilobytes
        return static_cast<size_t>( r_usage.ru_maxrss * 1024 );
    #endif
}

std::pair<double, double> info_process_total_cpu_time()
{
    // https://www.gnu.org/software/libc/manual/html_node/Resource-Usage.html
    struct rusage r_usage;
    getrusage(RUSAGE_SELF, &r_usage);

    // Calculate user and system time.
    auto const u_tmr = r_usage.ru_utime.tv_sec * 1.0 + (double)r_usage.ru_utime.tv_usec * 1.0e-6;
    auto const s_tmr = r_usage.ru_stime.tv_sec * 1.0 + r_usage.ru_stime.tv_usec * 1.0e-6;

    return std::make_pair( u_tmr, s_tmr );
}

double info_process_total_energy_consumption()
{
    // Adapted from https://github.com/amkozlov/raxml-ng/blob/master/src/util/sysutil.cpp

    try {
        double energy = 0.0;
        auto const basepath = "/sys/class/powercap/intel-rapl/intel-rapl:";

        // Sum up over all energy files
        size_t const max_packages = 32;
        for( size_t i = 0; i < max_packages; ++i ) {
            double pkg_energy;
            auto const fname = basepath + std::to_string(i) + "/energy_uj";
            if( !file_is_readable(fname) ) {
                break;
            }
            std::ifstream fs(fname);
            fs >> pkg_energy;
            energy += pkg_energy;
        }

        // Prepare output
        energy /= 1e6; // convert to Joules
        energy /= 3600; // convert to Wh
        return energy;
    } catch ( ... ) {
        return 0.0;
    }
    return 0.0;
}

std::string info_process_print_total_usage()
{
    // Get data.
    auto const time = info_process_total_cpu_time();
    auto const memory = info_process_peak_memory_usage();
    auto const energy = info_process_total_energy_consumption();

    // Print everything.
    std::stringstream ss;
    ss << std::setprecision(3) << std::fixed;
    ss << "Time:   " << time.first << "s (user)\n";
    ss << "Time:   " << time.second << "s (sys)\n";
    ss << "Memory: " << to_string_byte_format( memory ) << "\n";
    ss << "Energy: " << energy << "Wh\n";

    return ss.str();
}

} // namespace utils
} // namespace genesis
