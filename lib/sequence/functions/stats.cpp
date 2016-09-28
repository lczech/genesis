/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "sequence/functions/stats.hpp"

#include "sequence/functions/functions.hpp"
#include "sequence/printers/simple.hpp"
#include "sequence/sequence_set.hpp"
#include "sequence/sequence.hpp"

#include "utils/core/logging.hpp"
#include "utils/text/string.hpp"
#include "utils/text/style.hpp"
#include "utils/tools/char_lookup.hpp"

#include <algorithm>
#include <array>
#include <assert.h>
#include <cctype>
#include <numeric>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Characteristics
// =================================================================================================

// -------------------------------------------------------------------------
//     Site Histogram  &  Base Frequencies
// -------------------------------------------------------------------------

/**
 * @brief Get a histogram of the occurrences of particular sites, given a Sequence.
 *
 * This gives the raw counts of how often each site (character) appears in the Sequence.
 * See base_frequencies() for the relative version of this function.
 */
std::map<char, size_t> site_histogram( Sequence const& seq )
{
    std::map<char, size_t> result;
    for( auto const& s : seq ) {
        ++result[s];
    }
    return result;
}

/**
 * @brief Get a histogram of the occurrences of particular sites, given a SequenceSet.
 *
 * This gives the raw counts of how often each site (character) appears in the whole set.
 * See base_frequencies() for the relative version of this function.
 */
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

/**
 * @brief Get the base frequencies of the sites in a Sequence given the base chars.
 *
 * This returns the relative proportions of the given `plain_chars` to each other. Typically,
 * the given chars come from either nucleic_acid_codes_plain() or amino_acid_codes_plain(),
 * depending on the dataset.
 *
 * It is necessary to select those chars on a per-dataset basis, as it is up to the user to define
 * the meaning of those chars.
 */
std::map<char, double> base_frequencies( Sequence const& seq, std::string const& plain_chars )
{
    auto const sh = site_histogram( seq );
    return base_frequencies_accumulator( sh, plain_chars );
}

/**
 * @brief Get the base frequencies of the sites in a SequenceSet given the base chars.
 *
 * See the Sequence implementation of this function for details.
 */
std::map<char, double> base_frequencies( SequenceSet const& set, std::string const& plain_chars )
{
    auto const sh = site_histogram( set );
    return base_frequencies_accumulator( sh, plain_chars );
}

// -------------------------------------------------------------------------
//     Char counting and validation
// -------------------------------------------------------------------------

/**
 * @brief Count the number of occurrences of the given `chars` within the sites of the SequenceSet.
 *
 * This function can be used to count e.g. gaps or ambiguous characters in sequences.
 * For presettings of usable chars, see the functions `nucleic_acid_codes_...` and
 * `amino_acid_codes_...`. The chars are treated case-insensitive.
 *
 * If `chars` contains invalid (non-standard ASCII) characters, an `std::invalid_argument`
 * exception is thrown.
 */
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

/**
 * @brief Return the "gapyness" of the Sequence%s, i.e., the proportion of gap chars
 * and other completely undetermined chars to the total length of all sequences.
 *
 * This function returns a value in the interval 0.0 (no gaps and undetermined chars at all)
 * and 1.0 (all chars are undetermined).
 * See `nucleic_acid_codes_undetermined()` and `amino_acid_codes_undetermined()` for presettings
 * of gap character that can be used here depending on the data set type.
 * The chars are treated case-insensitive.
 * In the special case that there are no sequences or sites, 0.0 is returned.
 */
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

} // namespace sequence
} // namespace genesis
