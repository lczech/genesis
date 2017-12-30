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
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "genesis/sequence/functions/stats.hpp"

#include "genesis/sequence/counts.hpp"
#include "genesis/sequence/functions/functions.hpp"
#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/tools/char_lookup.hpp"

#include <cassert>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Characteristics
// =================================================================================================

// -------------------------------------------------------------------------
//     Site Histogram
// -------------------------------------------------------------------------

std::map<char, size_t> site_histogram( Sequence const& seq )
{
    std::map<char, size_t> result;
    for( auto const& s : seq ) {
        ++result[s];
    }
    return result;
}
std::map<char, size_t> site_histogram( SequenceSet const& set )
{
    std::map<char, size_t> result;
    for( auto const& seq : set ) {
        for( auto const& s : seq ) {
            ++result[s];
        }
    }
    return result;
}

// -------------------------------------------------------------------------
//     Base Frequencies
// -------------------------------------------------------------------------

/**
 * @brief Local helper function that turns a site histogram into base frequencies.
 */
std::map<char, double> base_frequencies_accumulator(
    std::map<char, size_t> const& sitehistogram,
    std::string            const& plain_chars
) {
    // Prepare result (do it here to facilitate copy elision).
    std::map<char, double> result;

    // Calculate sum of raw counts of all chars given in plain_chars.
    size_t sum = 0;
    for( auto const& shp : sitehistogram ) {
        if( plain_chars.find( shp.first ) != std::string::npos ) {
            sum += shp.second;
        }
    }

    // Make relative.
    for( auto const& pc : plain_chars ) {
        if( sitehistogram.count( pc )) {
            result[pc] = static_cast<double>( sitehistogram.at(pc) ) / static_cast<double>( sum );
        }
    }
    return result;
}

std::map<char, double> base_frequencies( Sequence const& seq, std::string const& plain_chars )
{
    auto const sh = site_histogram( seq );
    return base_frequencies_accumulator( sh, plain_chars );
}

std::map<char, double> base_frequencies( SequenceSet const& set, std::string const& plain_chars )
{
    auto const sh = site_histogram( set );
    return base_frequencies_accumulator( sh, plain_chars );
}

// -------------------------------------------------------------------------
//     Char counting and validation
// -------------------------------------------------------------------------

size_t count_chars( SequenceSet const& set, std::string const& chars )
{
    // Init array to false, then set all necessary chars to true.
    auto lookup = utils::CharLookup<bool>( false );
    lookup.set_selection_upper_lower( chars, true );

    size_t counter = 0;
    for( auto& s : set ) {
        for( auto& c : s ) {
            // get rid of this check and leave it to the parser/lexer/stream iterator
            if( c < 0 ) {
                continue;
            }
            assert( c >= 0 );
            if( lookup[ c ] ) {
                ++counter;
            }
        }
    }
    return counter;
}

// -------------------------------------------------------------------------
//     Gap Counting
// -------------------------------------------------------------------------

double gapyness( SequenceSet const& set, std::string const& gap_chars )
{
    size_t gaps = count_chars( set, gap_chars );
    size_t len  = total_length( set );
    if( len == 0 ) {
        return 0.0;
    }

    double ret = static_cast<double>( gaps ) / static_cast<double>( len );
    assert( 0.0 <= ret && ret <= 1.0 );
    return ret;
}

size_t gap_site_count( SiteCounts const& counts )
{
    size_t res = 0;
    for( size_t site_idx = 0; site_idx < counts.length(); ++site_idx ) {
        bool all_gap_site = true;

        for( size_t char_idx = 0; char_idx < counts.characters().size(); ++char_idx ) {
            all_gap_site &= ( counts.count_at( char_idx, site_idx ) == 0 );
        }

        if( all_gap_site ) {
            ++res;
        }
    }
    return res;
}

} // namespace sequence
} // namespace genesis
