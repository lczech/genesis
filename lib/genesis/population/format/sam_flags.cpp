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
 * @ingroup population
 */

#ifdef GENESIS_HTSLIB

#include "genesis/population/format/sam_flags.hpp"

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <unordered_map>

extern "C" {
    #include <htslib/sam.h>
}

namespace genesis {
namespace population {

// =================================================================================================
//     Name to Value Map
// =================================================================================================

/**
 * @brief Map from sam flags to their numerical value, for different types of naming of the flags.
 */
static const std::unordered_map<std::string, int> sam_flag_name_to_int_ = {
    { "paired",        0x1   },
    { "properpair",    0x2   },
    { "unmap",         0x4   },
    { "unmapped",      0x4   },
    { "munmap",        0x8   },
    { "mateunmapped",  0x8   },
    { "reverse",       0x10  },
    { "mreverse",      0x20  },
    { "matereverse",   0x20  },
    { "read1",         0x40  },
    { "read2",         0x80  },
    { "secondary",     0x100 },
    { "qcfail",        0x200 },
    { "dup",           0x400 },
    { "duplicate",     0x400 },
    { "supplementary", 0x800 }
};

// =================================================================================================
//     Flag Helper Functions
// =================================================================================================

int string_to_sam_flag( std::string const& value )
{
    // Use the htslib internal parsing function, which takes care of numbers, and
    // their spelling of the flag names (just upper case, underscores between words).
    // See also parse_sam_flag?! not sure what it does or if its needed.
    int result = bam_str2flag( value.c_str() );
    if( result >= 0 ) {
        return result;
    }

    // We could check that the number is actually within the range of what can be specified
    // with the sam flags. But htslib does not do that either, and we can just ignore any
    // values that are larger than the flags.
    // if( result >= 0 && result < 0x1000 - 1 ) {
    //     return result;
    // }
    // if( result >= 0x1000 - 1 ) {
    //     throw std::invalid_argument(
    //         "Invalid sam flag value \"" + value + "\" that is higher than the maximum flag value"
    //     );
    // }

    // If the above fails, add our own more lenient parsing on top.
    // We allow comma, space, and plus as delimiters, because why not.
    auto const flags = utils::split( value, ",+| " );

    // Clean up all flags by removing non-alpha-numeric chars, and making it lower case.
    // Then check in the map to get the value.
    result = 0;
    for( auto const& flag : flags ) {
        auto cleaned_value = utils::to_lower_ascii( flag );
        utils::erase_if( cleaned_value, []( char const c ){
            return ! utils::is_alnum( c );
        });

        // Try the sam function first, which takes care of all numeric values.
        int const tmp = bam_str2flag( cleaned_value.c_str() );
        if( tmp >= 0 ) {
            result |= tmp;
            continue;
        }

        // If that did not work, we expect a name, and use our map to look it up.
        if( sam_flag_name_to_int_.count( cleaned_value ) == 0 ) {
            throw std::invalid_argument( "Invalid sam flag name \"" + flag + "\"" );
        }
        result |= sam_flag_name_to_int_.at( cleaned_value );
    }
    return result;
}

std::string sam_flag_to_string( int flags )
{
    char* str = bam_flag2str( flags );
    auto res = std::string( str );
    free( str );
    return res;
}

// =================================================================================================
//     Validity check
// =================================================================================================

static_assert(
    static_cast<int>( 0x1 ) == BAM_FPAIRED,
    "Definitions of BAM_FPAIRED in htslib and in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( 0x2 ) == BAM_FPROPER_PAIR,
    "Definitions of BAM_FPROPER_PAIR in htslib and in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( 0x4 ) == BAM_FUNMAP,
    "Definitions of BAM_FUNMAP in htslib and in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( 0x8 ) == BAM_FMUNMAP,
    "Definitions of BAM_FMUNMAP in htslib and in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( 0x10 ) == BAM_FREVERSE,
    "Definitions of BAM_FREVERSE in htslib and in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( 0x20 ) == BAM_FMREVERSE,
    "Definitions of BAM_FMREVERSE in htslib and in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( 0x40 ) == BAM_FREAD1,
    "Definitions of BAM_FREAD1 in htslib and in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( 0x80 ) == BAM_FREAD2,
    "Definitions of BAM_FREAD2 in htslib and in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( 0x100 ) == BAM_FSECONDARY,
    "Definitions of BAM_FSECONDARY in htslib and in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( 0x200 ) == BAM_FQCFAIL,
    "Definitions of BAM_FQCFAIL in htslib and in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( 0x400 ) == BAM_FDUP,
    "Definitions of BAM_FDUP in htslib and in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( 0x800 ) == BAM_FSUPPLEMENTARY,
    "Definitions of BAM_FSUPPLEMENTARY in htslib and in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);

// static_assert(
//     static_cast<int>( SamFlag::kPaired ) == BAM_FPAIRED,
//     "Definitions of BAM_FPAIRED in htslib and of SamFlag::kPaired in genesis differ. "
//     "Please submit a bug report at https://github.com/lczech/genesis/issues"
// );
// static_assert(
//     static_cast<int>( SamFlag::kProperPair ) == BAM_FPROPER_PAIR,
//     "Definitions of BAM_FPROPER_PAIR in htslib and of SamFlag::kProperPair in genesis differ. "
//     "Please submit a bug report at https://github.com/lczech/genesis/issues"
// );
// static_assert(
//     static_cast<int>( SamFlag::kUnmapped ) == BAM_FUNMAP,
//     "Definitions of BAM_FUNMAP in htslib and of SamFlag::kUnmapped in genesis differ. "
//     "Please submit a bug report at https://github.com/lczech/genesis/issues"
// );
// static_assert(
//     static_cast<int>( SamFlag::kMateUnmapped ) == BAM_FMUNMAP,
//     "Definitions of BAM_FMUNMAP in htslib and of SamFlag::kMateUnmapped in genesis differ. "
//     "Please submit a bug report at https://github.com/lczech/genesis/issues"
// );
// static_assert(
//     static_cast<int>( SamFlag::kReverse ) == BAM_FREVERSE,
//     "Definitions of BAM_FREVERSE in htslib and of SamFlag::kReverse in genesis differ. "
//     "Please submit a bug report at https://github.com/lczech/genesis/issues"
// );
// static_assert(
//     static_cast<int>( SamFlag::kMateReverse ) == BAM_FMREVERSE,
//     "Definitions of BAM_FMREVERSE in htslib and of SamFlag::kMateReverse in genesis differ. "
//     "Please submit a bug report at https://github.com/lczech/genesis/issues"
// );
// static_assert(
//     static_cast<int>( SamFlag::kRead1 ) == BAM_FREAD1,
//     "Definitions of BAM_FREAD1 in htslib and of SamFlag::kRead1 in genesis differ. "
//     "Please submit a bug report at https://github.com/lczech/genesis/issues"
// );
// static_assert(
//     static_cast<int>( SamFlag::kRead2 ) == BAM_FREAD2,
//     "Definitions of BAM_FREAD2 in htslib and of SamFlag::kRead2 in genesis differ. "
//     "Please submit a bug report at https://github.com/lczech/genesis/issues"
// );
// static_assert(
//     static_cast<int>( SamFlag::kSecondary ) == BAM_FSECONDARY,
//     "Definitions of BAM_FSECONDARY in htslib and of SamFlag::kSecondary in genesis differ. "
//     "Please submit a bug report at https://github.com/lczech/genesis/issues"
// );
// static_assert(
//     static_cast<int>( SamFlag::kQcFail ) == BAM_FQCFAIL,
//     "Definitions of BAM_FQCFAIL in htslib and of SamFlag::kQcFail in genesis differ. "
//     "Please submit a bug report at https://github.com/lczech/genesis/issues"
// );
// static_assert(
//     static_cast<int>( SamFlag::kDuplicate ) == BAM_FDUP,
//     "Definitions of BAM_FDUP in htslib and of SamFlag::kDuplicate in genesis differ. "
//     "Please submit a bug report at https://github.com/lczech/genesis/issues"
// );
// static_assert(
//     static_cast<int>( SamFlag::kSupplementary ) == BAM_FSUPPLEMENTARY,
//     "Definitions of BAM_FSUPPLEMENTARY in htslib and of SamFlag::kSupplementary in genesis differ. "
//     "Please submit a bug report at https://github.com/lczech/genesis/issues"
// );

} // namespace population
} // namespace genesis

#endif // htslib guard
