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

#include "sequence/counts.hpp"

#include "sequence/sequence.hpp"
#include "sequence/sequence_set.hpp"
#include "utils/text/string.hpp"

#include <algorithm>
#include <assert.h>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace sequence {

// ================================================================================================
//     Constructors and Rule of Five
// ================================================================================================

SequenceCounts::SequenceCounts( std::string const& characters, size_t length )
    : num_seqs_( 0 )
{
    // Uppercase, sort, uniq the characters.
    characters_ = utils::to_upper_ascii( characters );
    std::sort( characters_.begin(), characters_.end() );
    characters_.erase( std::unique( characters_.begin(), characters_.end() ), characters_.end() );

    // Add characters to lookup table, set all other to a max value indicating that this char
    // is not part of the table.
    lookup_.set_all( characters_.size() );
    for( size_t i = 0; i < characters_.size(); ++i ) {
        auto c = characters_[i];
        lookup_.set_char_upper_lower( c, i );
    }

    // Use number of chars and number of sites to init data matrix.
    // We store the chars per row, and the sites per column (e.g., one column represents the 'A's
    // for all sites, while one row represents the possible chars for one site).
    // This way, we use cache locality when filling in data.
    counts_ = utils::Matrix< CountsIntType >( length, characters_.size() );
}

// ================================================================================================
//     Accesors
// ================================================================================================

/**
 * @brief Return the number of sites used for counting.
 *
 * This has to match the Sequence::length() property of the Sequence%s to be added for counting.
 */
size_t SequenceCounts::length() const
{
    return counts_.rows();
}

/**
 * @brief Return the character set that is used for counting.
 */
std::string SequenceCounts::characters() const
{
    assert( counts_.cols() == characters_.size() );
    return characters_;
}

/**
 * @brief Return the number of processed Sequence%s.
 */
SequenceCounts::CountsIntType SequenceCounts::added_sequences_count() const
{
    return num_seqs_;
}

/**
 * @brief Return the count for a specific character at a given site.
 *
 * If the charater is not part of the set of used characters, the function throws an exception.
 * See characters() to retrieve that set.
 */
SequenceCounts::CountsIntType SequenceCounts::count_at( size_t site_index, char character ) const
{
    if( site_index >= length() ) {
        throw std::runtime_error(
            "Invalid site index for retrieving count: " + std::to_string( site_index ) + "."
        );
    }

    auto char_idx = lookup_[ character ];
    if( char_idx == characters_.size() ) {
        throw std::runtime_error(
            "Invalid character for retrieving count: '" + std::string( 1, character ) + "'."
        );
    }

    return counts_( site_index, char_idx );
}

/**
 * @brief Return the count for a character and a site, given their indices.
 */
SequenceCounts::CountsIntType SequenceCounts::count_at(
    size_t site_index,
    size_t character_index
) const {
    if( site_index >= counts_.rows() ) {
        throw std::runtime_error(
            "Invalid site index for retrieving count: " + std::to_string( site_index ) + "."
        );
    }
    if( character_index > counts_.cols() ) {
        throw std::runtime_error(
            "Invalid character index for retrieving count: " + std::to_string( character_index ) + "."
        );
    }

    return counts_( site_index, character_index );
}

// ================================================================================================
//     Modifiers
// ================================================================================================

void SequenceCounts::add_sequence( Sequence const& sequence )
{
    add_sequence( sequence.sites() );
}

void SequenceCounts::add_sequence( std::string const& sites )
{
    if( num_seqs_ == std::numeric_limits< CountsIntType >::max() ) {
        throw std::runtime_error(
            "Cannot add Sequence to SequenceCounts as it might lead to an overflow in the counts."
        );
    }
    if( sites.size() != counts_.rows() ) {
        throw std::runtime_error(
            "Cannot add Sequence to SequenceCounts if it has different number of sites: Expected "
            + std::to_string( counts_.rows() ) + " sites, but sequence has"
            + std::to_string( sites.size() ) + " sites."
        );
    }

    for( size_t site_idx = 0; site_idx < sites.size(); ++site_idx ) {
        // Get the index of the char. If not found, this char is not to be counted, so continue.
        auto char_idx = lookup_[ static_cast< size_t >( sites[ site_idx ] ) ];
        if( char_idx == characters_.size() ) {
            continue;
        }

        // Increase the count at that index.
        ++counts_( site_idx, char_idx );
    }

    // We finished a sequence. Add to the counter.
    ++num_seqs_;
}

void SequenceCounts::add_sequences( SequenceSet const& sequences )
{
    for( auto const& seq : sequences ) {
        add_sequence( seq );
    }
}

} // namespace sequence
} // namespace genesis
