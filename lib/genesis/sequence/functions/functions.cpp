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

#include "genesis/sequence/functions/functions.hpp"

#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/sequence/printers/simple.hpp"

#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/text/style.hpp"
#include "genesis/utils/tools/char_lookup.hpp"

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

utils::Bitvector gap_sites( Sequence const& seq, std::string const& gap_chars )
{
    auto result = utils::Bitvector( seq.length() );

    // Init lookup array.
    auto lookup = utils::CharLookup<bool>( false );
    lookup.set_selection_upper_lower( gap_chars, true );

    for( size_t i = 0; i < seq.length(); ++i ) {
        result.set( i, lookup[ seq[ i ] ] );
    }
    return result;
}

utils::Bitvector gap_sites( SequenceSet const& set, std::string const& gap_chars )
{
    // Edge case.
    if( set.size() == 0 ) {
        return utils::Bitvector();
    }

    // Init result bitvector to all true. Then, for every site that is not a gap, reset to false.
    auto result = utils::Bitvector( set[0].length(), true );

    // Init lookup array.
    auto lookup = utils::CharLookup<bool>( false );
    lookup.set_selection_upper_lower( gap_chars, true );

    // Process all sequences in the set.
    for( auto const& seq : set ) {
        if( seq.length() != result.size() ) {
            throw std::runtime_error(
                "Cannot calculate gap_sites() if SequenceSet is not an alignment."
            );
        }

        // Process the sites of the sequence. If it is not a gap, set it to false in the bitvector.
        // This way, only sites that are all-gap will remain with value `true` in the end.
        for( size_t i = 0; i < seq.length(); ++i ) {
            if( ! lookup[ seq[ i ] ] ) {
                result.set( i, false );
            }
        }
    }

    return result;
}

bool validate_chars( SequenceSet const& set, std::string const& chars )
{
    // Init array to false, then set all necessary chars to true.
    auto lookup = utils::CharLookup<bool>( false );
    lookup.set_selection_upper_lower( chars, true );

    for( auto& s : set ) {
        for( auto& c : s ) {
            // get rid of this check and leave it to the parser/lexer/stream iterator
            if( c < 0 ) {
                return false;
            }
            assert( c >= 0 );
            if( ! lookup[ c ] ) {
                return false;
            }
        }
    }
    return true;
}

// -------------------------------------------------------------------------
//     Length and length checks
// -------------------------------------------------------------------------

size_t longest_sequence_length( SequenceSet const& set )
{
    size_t max = 0;
    for( auto const& seq : set ) {
        max = std::max( max, seq.length() );
    }
    return max;
}

size_t total_length( SequenceSet const& set )
{
    return std::accumulate( set.begin(), set.end(), 0,
        [] (size_t c, Sequence const& s) {
            return c + s.length();
        }
    );
}

bool is_alignment( SequenceSet const& set )
{
    if( set.size() == 0 ) {
        return true;
    }

    size_t length = set[0].length();
    for( auto& s : set ) {
        if( s.length() != length ) {
            return false;
        }
    }
    return true;
}

// =================================================================================================
//     Modifiers
// =================================================================================================

void remove_sites( Sequence& seq, utils::Bitvector sites )
{
    if( seq.length() != sites.size() ) {
        throw std::runtime_error(
            "Cannot remove sites from Sequence. "
            "Given Bitvector has not the same size as the Sequence."
        );
    }

    auto const num_sites = sites.size() - sites.count();
    std::string result;
    result.reserve( num_sites );

    for( size_t i = 0; i < sites.size(); ++i ) {
        if( ! sites[i] ) {
            result += seq[i];
        }
    }

    seq.sites( result );
}

void remove_sites( SequenceSet& set, utils::Bitvector sites )
{
    for( auto const& seq : set ) {
        if( seq.length() != sites.size() ) {
            throw std::runtime_error(
                "Cannot remove sites from SequenceSet. "
                "Given Bitvector has not the same size as the Sequences."
            );
        }
    }

    for( auto& seq : set ) {
        remove_sites( seq, sites );
    }
}

void remove_characters( Sequence& seq, std::string const& search )
{
    auto is_search_char = [&] ( char c ) {
        return search.find( c ) != std::string::npos;
    };

    auto& str = seq.sites();
    str.erase( std::remove_if( str.begin(), str.end(), is_search_char ), str.end() );
}

void remove_characters( SequenceSet& set, std::string const& search )
{
    for( auto& sequence : set ) {
        remove_characters( sequence, search );
    }
}

void remove_gaps( Sequence& seq, std::string const& gap_chars )
{
    remove_characters( seq, gap_chars );
}

void remove_gaps( SequenceSet& set, std::string const& gap_chars )
{
    remove_characters( set, gap_chars );
}

void replace_characters( Sequence& seq, std::string const& search, char replacement )
{
    seq.sites() = utils::replace_all_chars( seq.sites(), search, replacement );
}

void replace_characters( SequenceSet& set, std::string const& search, char replacement )
{
    for( auto& sequence : set ) {
        replace_characters( sequence, search, replacement );
    }
}

void replace_u_with_t( Sequence& seq )
{
    for( auto& c : seq.sites() ) {
        if( c == 'U' ) {
            c = 'T';
        }
        if( c == 'u' ) {
            c = 't';
        }
    }
}

void replace_u_with_t( SequenceSet& set )
{
    for( auto& sequence : set ) {
        replace_u_with_t( sequence );
    }
}

void replace_t_with_u( Sequence& seq )
{
    for( auto& c : seq.sites() ) {
        if( c == 'T' ) {
            c = 'U';
        }
        if( c == 't' ) {
            c = 'u';
        }
    }
}

void replace_t_with_u( SequenceSet& set )
{
    for( auto& sequence : set ) {
        replace_t_with_u( sequence );
    }
}

void merge_duplicate_sequences(
    SequenceSet& set,
    MergeDuplicateSequencesCountPolicy count_policy,
    std::string const& counter_prefix
) {
    // Helper to keep track of which sequences (at position i in the set) occured how often.
    struct Duplicate
    {
        size_t index;
        size_t count;
    };

    // Find duplicates and count their occurences.
    // TODO this is a very memory intense step. find an algo that does not need to copy all sites...
    std::unordered_map< std::string, Duplicate > dup_map;
    size_t i = 0;
    while( i < set.size() ) {
        auto& seq = set[i];

        if( dup_map.count( seq.sites() ) == 0 ) {

            // If it is a new sequence, init the map entry and move to the next sequence.
            dup_map[ seq.sites() ].index = i;
            dup_map[ seq.sites() ].count = 1;
            ++i;

        } else {

            // If we already saw that sequence, increment its counter, and remove the sequence
            // from the set. Do not increment i - we deleted a sequence, so staying at the
            // position automatically "moves" to the next one.
            ++dup_map[ seq.sites() ].count;
            set.remove(i);
        }
    }

    // We do not need to relabel sequences.
    if( count_policy == MergeDuplicateSequencesCountPolicy::kDiscard ) {
        return;
    }

    // Relabel using the counts.
    for( size_t i = 0; i < set.size(); ++i ) {
        auto& seq = set[i];

        // The sequence needs to be in the map, as we added it in the previous step.
        // It also needs to have the same index, as we never changed that.
        assert( dup_map.count(seq.sites()) > 0 );
        assert( dup_map[ seq.sites() ].index == i );

        // Append the count to either the label or the metadata.
        auto count = dup_map[ seq.sites() ].count;
        if( count_policy == MergeDuplicateSequencesCountPolicy::kAppendToLabel ) {
            auto new_label = seq.label() + counter_prefix + std::to_string(count);
            seq.label( new_label );

        } else if( count_policy == MergeDuplicateSequencesCountPolicy::kAppendToMetadata ) {
            auto new_meta = seq.metadata() + counter_prefix + std::to_string(count);
            seq.metadata( new_meta );

        } else {

            // We already skipped the discard case, so this here should not happen.
            assert( false );
        }
    }
}

// =================================================================================================
//     Filters
// =================================================================================================

void filter_min_sequence_length( SequenceSet& set, size_t min_length )
{
    size_t index = 0;
    while( index < set.size() ) {
        if( set.at(index).length() < min_length ) {
            set.remove(index);
        } else {
            ++index;
        }
    }
}

void filter_max_sequence_length( SequenceSet& set, size_t max_length )
{
    size_t index = 0;
    while( index < set.size() ) {
        if( set.at(index).length() > max_length ) {
            set.remove(index);
        } else {
            ++index;
        }
    }
}

void filter_min_max_sequence_length( SequenceSet& set, size_t min_length, size_t max_length )
{
    size_t index = 0;
    while( index < set.size() ) {
        auto len = set.at(index).length();
        if( len < min_length || len > max_length ) {
            set.remove(index);
        } else {
            ++index;
        }
    }
}

// =================================================================================================
//     Print and Output
// =================================================================================================

// -------------------------------------------------------------------------
//     Ostream
// -------------------------------------------------------------------------

std::ostream& operator << ( std::ostream& out, Sequence    const& seq )
{
    auto printer = PrinterSimple();
    printer.length_limit( 100 );

    printer.print( out, seq );
    return out;
}

std::ostream& operator << ( std::ostream& out, SequenceSet const& set )
{
    auto printer = PrinterSimple();
    printer.length_limit( 100 );
    printer.sequence_limit( 10 );

    printer.print( out, set );
    return out;
}

} // namespace sequence
} // namespace genesis
