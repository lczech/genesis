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

    For an alternative approach that seems to support Apple better, see
    https://github.com/xflouris/libpll-2/blob/54b9a3db5689cce6abed5af978e49c45a3b23b45/src/hardware.c
    and https://github.com/amkozlov/raxml-ng/blob/805318cef87bd5d67064efa299b5d1cf948367fd/src/util/sysutil.cpp

    For more hardware related information, see
    https://github.com/amkozlov/raxml-ng/blob/805318cef87bd5d67064efa299b5d1cf948367fd/src/util/sysutil.cpp
 */

/**
 * @brief
 *
 * @file
 * @ingroup main
 */

#include "genesis/utils/core/info.hpp"

#include "genesis/utils/core/options.hpp"
#include "genesis/utils/core/version.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>

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

#if defined( _WIN32 ) || defined(  _WIN64  ) || defined(_MSC_VER)
#    include <io.h>
#    include <intrin.h>
#    include <windows.h>
#elif defined(__GNUC__) || defined(__clang__)
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
#    warning "No cpuid intrinsic defined for compiler."
#endif

#if !(defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86))
#    warning "No cpuid intrinsic defined for processor architecture."
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
        initialized = true;
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

    // res += "Endianness:        " + std::string(
    //     info_is_little_endian() ? "little endian" : "big endian"
    // ) + "\n";
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

    void cpuid(int32_t out[4], int32_t eax, int32_t ecx)
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
                printf("Error Detecting Operating System.\n");
                printf("Defaulting to 32-bit OS.\n\n");
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

#else // defined( _WIN32 ) || defined(  _WIN64  )

    void cpuid(int32_t out[4], int32_t eax, int32_t ecx)
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

    #define _XCR_XFEATURE_ENABLED_MASK  0

    //  Detect 64-bit
    bool detect_OS_x64_(){
        //  We only support x64 on Linux.
        return true;
    }

#endif // defined( _WIN32 ) || defined(  _WIN64  )

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
    cpuid(cpuInfo, 1, 0);

    bool osUsesXSAVE_XRSTORE = (cpuInfo[2] & (1 << 27)) != 0;
    bool cpuAVXSuport = (cpuInfo[2] & (1 << 28)) != 0;

    if (osUsesXSAVE_XRSTORE && cpuAVXSuport)
    {
        uint64_t xcrFeatureMask = xgetbv(_XCR_XFEATURE_ENABLED_MASK);
        avxSupported = (xcrFeatureMask & 0x6) == 0x6;
    }

    return avxSupported;
}

bool detect_OS_AVX512_()
{
    if (!detect_OS_AVX_())
        return false;

    uint64_t xcrFeatureMask = xgetbv(_XCR_XFEATURE_ENABLED_MASK);
    return (xcrFeatureMask & 0xe6) == 0xe6;
}
std::string get_vendor_string_()
{
    int32_t CPUInfo[4];
    char name[13];

    cpuid(CPUInfo, 0, 0);
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

// -----------------------------------------------------------------------------
//     info_get_hardware
// -----------------------------------------------------------------------------

InfoHardware const& info_get_hardware()
{
    // No need to repeat.
    static InfoHardware result;
    static bool initialized = false;
    if( initialized ) {
        initialized = true;
        return result;
    }

    // Endianness
    static const uint16_t little_endian_test = 0x1000;
    result.is_little_endian = ( 0 == *reinterpret_cast< uint8_t const* >( &little_endian_test ));
    // static const uint16_t big_endian_test = 0x0001;
    // result.is_little_endian = ( 0 == *reinterpret_cast< uint8_t const* >( &big_endian_test ));

    // ---------------------------------------------------------
    //     Vendor and OS
    // ---------------------------------------------------------

    //  Vendor
    result.vendor_string = get_vendor_string_();
    if( result.vendor_string == "GenuineIntel" ){
        result.vendor_Intel = true;
    }else if( result.vendor_string == "AuthenticAMD" ){
        result.vendor_AMD = true;
    }
    result.cpu_model = get_cpu_model_();

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
    cpuid(info, 0, 0);
    int nIds = info[0];

    cpuid(info, 0x80000000, 0);
    uint32_t nExIds = info[0];

    if (nIds >= 0x00000001){
        cpuid(info, 0x00000001, 0);
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
    if (nIds >= 0x00000007){
        cpuid(info, 0x00000007, 0);
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

        cpuid(info, 0x00000007, 1);
        result.HW_AVX512_BF16      = (info[0] & ((int)1 <<  5)) != 0;
    }
    if (nExIds >= 0x80000001){
        cpuid(info, 0x80000001, 0);
        result.HW_x64   = (info[3] & ((int)1 << 29)) != 0;
        result.HW_ABM   = (info[2] & ((int)1 <<  5)) != 0;
        result.HW_SSE4a = (info[2] & ((int)1 <<  6)) != 0;
        result.HW_FMA4  = (info[2] & ((int)1 << 16)) != 0;
        result.HW_XOP   = (info[2] & ((int)1 << 11)) != 0;
        result.HW_PREFETCHW = (info[2] & ((int)1 << 8)) != 0;
    }

    return result;
}

// -----------------------------------------------------------------------------
//     info_print_hardware
// -----------------------------------------------------------------------------

std::string info_print_hardware()
{
    std::stringstream ss;
    auto print_ = [&]( char const* label, bool yes )
    {
        ss << label;
        ss << (yes ? "true" : "false") << "\n";
    };

    ss << "Hardware Features\n";
    ss << "=============================================\n\n";
    auto const& info_hardware = info_get_hardware();

    ss << "CPU Vendor:" << "\n";
    ss << "    Vendor      = " << info_hardware.vendor_string << "\n";
    ss << "    CPU model   = " << info_hardware.cpu_model << "\n";
    print_("    AMD         = ", info_hardware.vendor_AMD);
    print_("    Intel       = ", info_hardware.vendor_Intel);
    ss << "\n";

    ss << "OS Features:" << "\n";
    print_("    64-bit      = ", info_hardware.OS_x64);
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

    // ss << "Summary:" << "\n";
    // print_("    Safe to use AVX:     ", info_hardware.HW_AVX && info_hardware.OS_AVX);
    // print_("    Safe to use AVX512:  ", info_hardware.HW_AVX512_F && info_hardware.OS_AVX512);
    // ss << "\n";

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
