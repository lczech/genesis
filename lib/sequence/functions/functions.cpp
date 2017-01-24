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

#include "sequence/functions/functions.hpp"

#include "sequence/sequence_set.hpp"
#include "sequence/sequence.hpp"
#include "sequence/printers/simple.hpp"

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
//     Accessors
// =================================================================================================

/**
 * @brief Return a pointer to a sequence with a specific label, or `nullptr` iff not found.
 */
Sequence const* find_sequence( SequenceSet const& set, std::string const& label )
{
    for (Sequence const& s : set) {
        if (s.label() == label) {
            return &s;
        }
    }
    return nullptr;
}

// =================================================================================================
//     Characteristics
// =================================================================================================

/**
 * @brief Return a @link utils::Bitvector Bitvector@endlink that is `true` where the Sequence has
 * a gap and `false` where not.
 *
 * The `gap_chars` are used case-insensitively to determine what is considerted to be a gap.
 * By default, nucleic_acid_codes_undetermined() are used, but any other set of characters
 * is allowed.
 */
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

/**
 * @brief Return a @link utils::Bitvector Bitvector@endlink that is `true` where all Sequence%s in
 * the SequenceSet have a gap and `false` where not, that is, where at least on Sequence is not a
 * gap.
 *
 * The `gap_chars` are used case-insensitively to determine what is considerted to be a gap.
 * By default, nucleic_acid_codes_undetermined() are used, but any other set of characters
 * is allowed.
 */
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

/**
 * @brief Returns true iff all Sequence%s only consist of the given `chars`.
 *
 * For presettings of usable chars, see the functions `nucleic_acid_codes_...` and
 * `amino_acid_codes_...`. For example, to check whether the sequences are nucleic acids,
 * use `nucleic_acid_codes_all()`. The chars are treated case-insensitive.
 *
 * If `chars` contains invalid (non-standard ASCII) characters, an `std::invalid_argument`
 * exception is thrown.
 */
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

/**
 * @brief Return the total length (sum) of all Sequence%s in the SequenceSet.
 */
size_t total_length( SequenceSet const& set )
{
    return std::accumulate( set.begin(), set.end(), 0,
        [] (size_t c, Sequence const& s) {
            return c + s.length();
        }
    );
}

/**
 * @brief Return true iff all Sequence%s in the SequenceSet have the same length.
 */
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

/**
 * @brief Remove all sites from a Sequence where the given @link utils::Bitvector Bitvector@endlink
 * is `true`, and keep all others.
 *
 * The Bitvector needs to have the same size as the Sequence, otherwise an expection is thrown.
 *
 * This function is for example useful in combination with gap_sites().
 */
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

/**
 * @brief Remove all sites from all Sequence%s in a SequenceSet where the given
 * @link utils::Bitvector Bitvector@endlink is `true`, and keep all others.
 *
 * The Bitvector and all Sequences need to have the same size, otherwise an expection is thrown.
 * This check is done before any Sequence is changed. Thus, if the function throws for this reason,
 * the Sequences are left unchanged.
 *
 * This function is for example useful in combination with gap_sites().
 */
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

/**
 * @brief Remove all of the characters in `search` from the sites of the Sequence.
 */
void remove_characters( Sequence& seq, std::string const& search )
{
    auto is_search_char = [&] ( char c ) {
        return search.find( c ) != std::string::npos;
    };

    auto& str = seq.sites();
    str.erase( std::remove_if( str.begin(), str.end(), is_search_char ), str.end() );
}

/**
 * @brief Remove all of the characters in `search` from the sites of the Sequence%s in the
 * SequenceSet.
 */
void remove_characters( SequenceSet& set, std::string const& search )
{
    for( auto& sequence : set ) {
        remove_characters( sequence, search );
    }
}

/**
 * @brief Remove all gap characters from the sites of the Sequence.
 *
 * This function is an alias for remove_characters(), which by default uses the gap sites of
 * nucleic_acid_codes_undetermined().
 */
void remove_gaps( Sequence& seq, std::string const& gap_chars )
{
    remove_characters( seq, gap_chars );
}

/**
 * @brief Remove all gap characters from the sites of the Sequence%s in the SequenceSet.
 *
 * This function is an alias for remove_characters(), which by default uses the gap sites of
 * nucleic_acid_codes_undetermined().
 */
void remove_gaps( SequenceSet& set, std::string const& gap_chars )
{
    remove_characters( set, gap_chars );
}

/**
 * @brief Replace all occurences of the chars in `search` by the `replace` char, for all sites in
 * the given Sequence.
 *
 * The function is case sensitive. Thus, you need to use both cases for the search string if you
 * are unsure. The replace char is always used as is, independent of the case of the matching
 * search char.
 */
void replace_characters( Sequence& seq, std::string const& search, char replacement )
{
    seq.sites() = utils::replace_all_chars( seq.sites(), search, replacement );
}

/**
 * @brief Replace all occurences of the chars in `search` by the `replace` char, for all sites in
 * the Sequence%s in the given SequenceSet.
 *
 * The function is case sensitive. Thus, you need to use both cases for the search string if you
 * are unsure. The replace char is always used as is, independent of the case of the matching
 * search char.
 */
void replace_characters( SequenceSet& set, std::string const& search, char replacement )
{
    for( auto& sequence : set ) {
        replace_characters( sequence, search, replacement );
    }
}

/**
 * @brief Replace all occurrences of `U` by `T` in the sites of the Sequence.
 *
 * This is a small helper function for sequences with nucleic acid codes. It is case sensitive,
 * that is, lower case `u` is replaced by lower case `t`, and upper case `U` by upper case `T`.
 */
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

/**
 * @brief Replace all occurrences of `U` by `T` in the sites of all Sequence%s in the SequenceSet.
 *
 * This is a small helper function for sequences with nucleic acid codes. It is case sensitive,
 * that is, lower case `u` is replaced by lower case `t`, and upper case `U` by upper case `T`.
 */
void replace_u_with_t( SequenceSet& set )
{
    for( auto& sequence : set ) {
        replace_u_with_t( sequence );
    }
}

/**
 * @brief Replace all occurrences of `T` by `U` in the sites of the Sequence.
 *
 * This is a small helper function for sequences with nucleic acid codes. It is case sensitive,
 * that is, lower case `t` is replaced by lower case `u`, and upper case `T` by upper case `U`.
 */
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

/**
 * @brief Replace all occurrences of `T` by `U` in the sites of all Sequence%s in the SequenceSet.
 *
 * This is a small helper function for sequences with nucleic acid codes. It is case sensitive,
 * that is, lower case `t` is replaced by lower case `u`, and upper case `T` by upper case `U`.
 */
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

// void remove_list( SequenceSet& set, std::vector<std::string> const& labels, bool invert )
// {
//     // create a set of all labels for fast lookup.
//     std::unordered_set<std::string> lmap(labels.begin(), labels.end());
//
//     // iterate and move elements from it to re
//     auto it = set.begin();
//     auto re = set.begin();
//
//     // this works similar to std::remove (http://www.cplusplus.com/reference/algorithm/remove/)
//     while (it != set.end()) {
//         // if the label is (not) in the map, move it to the re position, otherwise delete it.
//         if ( (!invert && lmap.count( it->label() )  > 0) ||
//              ( invert && lmap.count( it->label() ) == 0)
//         ) {
//             // delete *it;
//         } else {
//             *re = std::move(*it);
//             ++re;
//         }
//         ++it;
//     }
//
//     // delete the tail of the vector.
//     // set.erase(re, sequences.end());
// }

// =================================================================================================
//     Filters
// =================================================================================================

/**
 * @brief Remove all Sequence%s from the SequenceSet whose @link Sequence::length() length@endlink
 * is below the given `min_length`.
 *
 * See also filter_max_sequence_length() and filter_min_max_sequence_length().
 */
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

/**
 * @brief Remove all Sequence%s from the SequenceSet whose @link Sequence::length() length@endlink
 * is above the given `max_length`.
 *
 * See also filter_min_sequence_length() and filter_min_max_sequence_length().
 */
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

/**
 * @brief Remove all Sequence%s from the SequenceSet whose @link Sequence::length() length@endlink
 * is not inbetween the `min_length` and `max_length`.
 *
 * This function has the same effect as calling filter_min_sequence_length() and
 * filter_max_sequence_length(), but does it in one iteration over the SequenceSet.
 */
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

/**
 * @brief Print a Sequence to an ostream in the form "label: sites".
 *
 * As this is meant for quickly having a look at the Sequence, only the first 100 sites are printed.
 * If you need all sites, or more settings like color, use SimplePrinter.
 */
std::ostream& operator << ( std::ostream& out, Sequence    const& seq )
{
    auto printer = PrinterSimple();
    printer.length_limit( 100 );

    printer.print( out, seq );
    return out;
}

/**
 * @brief Print a SequenceSet to an ostream in the form "label: sites".
 *
 * As this is meant for quickly having a look at the SequenceSet, only the first 10 Sequences and
 * the first 100 sites of each are printed. If you need all sequences and sites,
 * or more settings like color, use SimplePrinter.
 */
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
