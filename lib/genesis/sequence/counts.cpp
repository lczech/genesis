/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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

#include "genesis/sequence/counts.hpp"

#include "genesis/sequence/functions/codes.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/sequence/sequence_set.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace sequence {

// ================================================================================================
//     Constructors and Rule of Five
// ================================================================================================

SiteCounts::SiteCounts( std::string const& characters, size_t length )
    : num_seqs_( 0 )
{
    // Uppercase, sort, uniq the characters.
    characters_ = normalize_code_alphabet( characters );

    // Add characters to lookup table, set all other to a max value indicating that this char
    // is not part of the table.
    lookup_.set_all( static_cast<unsigned char>( characters_.size() ));
    for( size_t i = 0; i < characters_.size(); ++i ) {
        auto c = characters_[i];
        lookup_.set_char_upper_lower( c, static_cast<unsigned char>(i) );
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

size_t SiteCounts::length() const
{
    return counts_.rows();
}

std::string SiteCounts::characters() const
{
    assert( counts_.cols() == characters_.size() );
    return characters_;
}

SiteCounts::CountsIntType SiteCounts::added_sequences_count() const
{
    return num_seqs_;
}

SiteCounts::CountsIntType SiteCounts::count_of( char character, size_t site_index ) const
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

SiteCounts::CountsIntType SiteCounts::count_at(
    size_t character_index,
    size_t site_index
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

void SiteCounts::add_sequence( Sequence const& sequence, bool use_abundance )
{
    if( use_abundance ) {
        add_sequence( sequence.sites(), static_cast<CountsIntType>( sequence.abundance() ));
    } else {
        add_sequence( sequence.sites(), 1 );
    }
}

void SiteCounts::add_sequence( std::string const& sites, CountsIntType weight )
{
    if( num_seqs_ >= std::numeric_limits< CountsIntType >::max() - weight ) {
        throw std::runtime_error(
            "Cannot add Sequence to SiteCounts as it might lead to an overflow in the counts."
        );
    }
    if( sites.size() != counts_.rows() ) {
        throw std::runtime_error(
            "Cannot add Sequence to SiteCounts if it has different number of sites: Expected "
            + std::to_string( counts_.rows() ) + " sites, but sequence has "
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
        counts_( site_idx, char_idx ) += weight;
    }

    // We finished a sequence. Add to the counter.
    num_seqs_ += weight;
}

void SiteCounts::add_sequences( SequenceSet const& sequences, bool use_abundances )
{
    for( auto const& seq : sequences ) {
        add_sequence( seq, use_abundances );
    }
}

void SiteCounts::clear()
{
    characters_ = "";
    lookup_.set_all( 0 );
    counts_ = utils::Matrix< CountsIntType >();
    num_seqs_ = 0;
}

void SiteCounts::clear_counts()
{
    for( auto& e : counts_ ) {
        e = 0;
    }
    num_seqs_ = 0;
}

} // namespace sequence
} // namespace genesis
