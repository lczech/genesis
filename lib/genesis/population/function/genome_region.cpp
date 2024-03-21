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

#include "genesis/population/function/genome_region.hpp"

#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/text/convert.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Comparion Operators
// =================================================================================================

bool operator ==( GenomeRegion const& a, GenomeRegion const& b )
{
    return a.chromosome == b.chromosome && a.start == b.start && a.end == b.end;
}

bool operator !=( GenomeRegion const& a, GenomeRegion const& b )
{
    return !( a == b );
}

// =================================================================================================
//     Parsing & Printing
// =================================================================================================

std::ostream& operator<<( std::ostream& os, GenomeRegion const& region )
{
    os << to_string( region );
    // os << region.chromosome << ": [" << region.start << ", " << region.end << ")";
    return os;
}

std::string to_string( GenomeRegion const& region )
{
    // Error cases. We need to check separately here, as we want to be able to treat the
    // start == end == 0 special case extra here, and just print out the chromosome in that case.
    if( region.chromosome.empty() ) {
        throw std::invalid_argument(
            "Invalid GenomeRegion with empty chromosome."
        );
    }
    if(( region.start == 0 ) != ( region.end == 0 )) {
        throw std::invalid_argument(
            "Invalid GenomeRegion with one of start and end equal to zero."
        );
    }
    if( region.start > region.end ) {
        throw std::invalid_argument(
            "Invalid GenomeRegion with start > end."
        );
    }

    // Special cases.
    if( region.start == 0 && region.end == 0 ) {
        return region.chromosome;
    } else if( region.start == region.end ) {
        return region.chromosome + ":" + std::to_string( region.start );
    }

    // General case.
    return
        region.chromosome + ":" +
        std::to_string( region.start ) + "-" +
        std::to_string( region.end )
    ;
}

GenomeRegion parse_genome_region( std::string const& region, bool zero_based, bool end_exclusive )
{
    GenomeRegion result;

    // Helper function to throw on error without copies of the same error message each time.
    auto throw_invalid_arg_ = [&](){
        throw std::invalid_argument( "Invalid genomic region string \"" + region + "\"" );
    };

    // Helper function to convert a string to a position, with a nicer error message.
    auto convert_str_to_pos_ = [&]( std::string const& str ){
        size_t res = 0;
        try {
            res = utils::convert_from_string<size_t>( str, true );
        } catch( ... ) {
            throw_invalid_arg_();
        }
        return res;
    };

    // Split by chromosome delimitier and store the chrom. Every string at least yields one split.
    auto const chr_split = utils::split( region, ":", false );
    assert( chr_split.size() > 0 );
    result.chromosome = chr_split[0];

    // Special cases where either everything is empty, or parts are.
    if( result.chromosome.empty() || result.chromosome == "-" || result.chromosome == ".." ) {
        throw_invalid_arg_();
    }

    // If there is a part after the `:`, use that for positions.
    if( chr_split.size() == 2 ) {

        // Try to split by "-", or if that does not work, try ".." instead.
        auto pos_split = utils::split( chr_split[1], "-", false );
        if( pos_split.size() == 1 ) {
            pos_split = utils::split_at( chr_split[1], "..", false );
        }

        // If there is no delimiter, use the number for both start and end.
        if( pos_split.size() == 1 ) {
            // Did neither find "-" nor "..". Use position as both start and end.
            if( pos_split[0].empty() ) {
                throw_invalid_arg_();
            }
            auto const pos = convert_str_to_pos_( pos_split[0] );
            result.start = pos;
            result.end   = pos;
        } else if( pos_split.size() == 2 ) {
            // Found a valid split by "-" or "..".
            // If either part is empty, error. Otherwise, convert.
            if( pos_split[0].empty() || pos_split[1].empty() ) {
                throw_invalid_arg_();
            }
            result.start = convert_str_to_pos_( pos_split[0] );
            result.end   = convert_str_to_pos_( pos_split[1] );
        } else {
            throw_invalid_arg_();
        }

        // Fix coordinates if needed.
        if( zero_based ) {
            ++result.start;
            ++result.end;
        }
        if( end_exclusive ) {
            if( result.end == 0 ) {
                throw_invalid_arg_();
            }
            --result.end;
        }

        // Validity check.
        if( ! result.specified() ) {
            throw_invalid_arg_();
        }
    } else if( chr_split.size() > 2 ) {
        // Multiple ":" found.
        throw_invalid_arg_();
    }
    return result;
}

GenomeRegionList parse_genome_regions(
    std::string const& regions, bool zero_based, bool end_exclusive
) {
    GenomeRegionList result;

    auto const region_list = utils::split( regions, ",", false );
    for( auto const& region : region_list ) {
        result.add( parse_genome_region( utils::trim( region ), zero_based, end_exclusive ));
    }

    return result;
}

// =================================================================================================
//     Region Coverage
// =================================================================================================

bool is_covered( GenomeRegion const& region, std::string const& chromosome, size_t position )
{
    if( region.start > region.end ) {
        throw std::runtime_error( "Invalid GenomeRegion with start > end" );
    }

    if( region.start > 0 && region.end > 0 ) {
        // With proper start and/or end, all has to match.
        auto const chr = chromosome == region.chromosome;
        auto const beg = position >= region.start;
        auto const end = position <= region.end;
        return chr && beg && end;
    } else if( region.start == 0 && region.end == 0 ) {
        // If both start and end are zero, we are just matching the chromosome.
        return chromosome == region.chromosome;
    }

    // Edge error case, could throw here as well.
    assert( region.start == 0 || region.end == 0 );
    return false;
}

} // namespace population
} // namespace genesis
