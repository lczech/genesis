/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/functions/genome_region.hpp"

#include "genesis/utils/text/convert.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Genome Region
// =================================================================================================

std::ostream& operator<<( std::ostream& os, GenomeRegion const& region )
{
    os << to_string( region );
    // os << region.chromosome << ": [" << region.start << ", " << region.end << ")";
    return os;
}

std::string to_string( GenomeRegion const& region )
{
    if( region.start == 0 && region.end == 0 ) {
        return region.chromosome;
    } else if( region.start == region.end ) {
        return region.chromosome + ":" + std::to_string( region.start );
    } else {
        return
            region.chromosome + ":" +
            std::to_string( region.start ) + "-" +
            std::to_string( region.end )
        ;
    }
}

bool operator ==( GenomeRegion const& a, GenomeRegion const& b )
{
    return a.chromosome == b.chromosome && a.start == b.start && a.end == b.end;
}

GenomeRegion parse_genome_region( std::string const& region )
{
    GenomeRegion result;

    // Helper function to throw on error without copies of the same error message each time.
    auto throw_invalid_arg_ = [&](){
        throw std::invalid_argument( "Invalid genomic region string '" + region + "'" );
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
            auto const pos = utils::convert_from_string<size_t>( pos_split[0], true );
            result.start = pos;
            result.end   = pos;
        } else if( pos_split.size() == 2 ) {
            // Found a valid split by "-" or "..".
            // If either part is empty, error. Otherwise, convert.
            if( pos_split[0].empty() || pos_split[1].empty() ) {
                throw_invalid_arg_();
            }
            result.start = utils::convert_from_string<size_t>( pos_split[0], true );
            result.end   = utils::convert_from_string<size_t>( pos_split[1], true );
        } else {
            throw_invalid_arg_();
        }

        // Validity check.
        if( result.start > result.end ) {
            throw_invalid_arg_();
        }
    } else if( chr_split.size() > 2 ) {
        // Multiple ":" found.
        throw_invalid_arg_();
    }
    return result;
}

} // namespace population
} // namespace genesis
