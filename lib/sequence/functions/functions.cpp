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
//     Labels
// =================================================================================================

/**
 * @brief Return true iff all labels of the Sequence%s in the SequenceSet are unique.
 *
 * The optional parameter `case_sensitive` controls how labels are compared. Default is `true`,
 * that is, Sequence%s are compared case-sensitively.
 */
bool has_unique_labels( SequenceSet const& set, bool case_sensitive )
{
    std::unordered_set< std::string > labels;
    std::string label;

    for( auto const& seq : set ) {
        if( case_sensitive ) {
            label = seq.label();
        } else {
            label = utils::to_lower( seq.label() );
        }

        if( labels.count( label ) > 0 ) {
            return false;
        } else {
            labels.insert( label );
        }
    }
    return true;
}

/**
 * @brief Check whether a given string is a valid label for a Sequence.
 *
 * While we can work with any form of label (as long as it is a string), most file formats and
 * consequently most programs that read them restrict the set of valid characters for labels of
 * sequences. We thus provide this function, which uses the most common interpretation of valid
 * labels.
 *
 * A label is valid if its characters have a graphical representation (i.e., isgraph() is true) and
 * if non of these characters occurs:
 *
 *     :,();[]'
 *
 * Thus, all whitespaces, control characters, and the listed special characters are invalid.
 * See sanitize_label() for a function that replaces all invalid characters of the label by
 * underscores.
 */
bool is_valid_label( std::string const& label )
{
    std::string invalid_chars = ":,();[]'";
    for( auto c : label ) {
        if( ! isgraph(c) || invalid_chars.find( c ) != std::string::npos ) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Check whether a Sequence has a valid label.
 *
 * This might be important for printing the Sequence to a file that needs to be read by other
 * applications. See is_valid_label() for details on what is considered a valid label.
 * See sanitize_label() for a function that replaces all invalid characters of the label by
 * underscores.
 */
bool has_valid_label(  Sequence const& seq )
{
    return is_valid_label( seq.label() );
}

/**
 * @brief Check whether all Sequence%s in a SequenceSet have valid labels.
 *
 * This might be important for printing the Sequences to a file that needs to be read by other
 * applications. See is_valid_label() for details on what is considered a valid label.
 * See sanitize_labels() for a function that replaces all invalid characters of the labels by
 * underscores.
 */
bool has_valid_labels( SequenceSet const& set )
{
    for( auto const& seq : set ) {
        if( ! has_valid_label( seq )) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Sanitize a label by replacing all invalid characters with underscores.
 *
 * This might be important for printing the Sequences to a file that needs to be read by other
 * applications. See is_valid_label() for details on what is considered a valid label.
 */
std::string sanitize_label( std::string const& label )
{
    std::string result;
    std::string invalid_chars = ":,();[]'";
    for( auto c : label ) {
        if( ! isgraph(c) || invalid_chars.find( c ) != std::string::npos ) {
            result += "_";
        } else {
            result += c;
        }
    }
    return result;
}

/**
 * @brief Sanitize a label by replacing all invalid characters with underscores.
 *
 * This might be important for printing the Sequences to a file that needs to be read by other
 * applications. See is_valid_label() for details on what is considered a valid label.
 */
void sanitize_label( Sequence& seq )
{
    seq.label( sanitize_label( seq.label() ));
}

/**
 * @brief Sanitize the labels of all Sequence%s in the SequenceSet by replacing all invalid
 * characters with underscores.
 *
 * This might be important for printing the Sequences to a file that needs to be read by other
 * applications. See is_valid_label() for details on what is considered a valid label.
 */
void sanitize_labels( SequenceSet& set )
{
    for( auto& seq : set ) {
        sanitize_label( seq );
    }
}

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
 * This is a small helper function for sequences with nucleic acid codes. It is case insensitive,
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
 * This is a small helper function for sequences with nucleic acid codes. It is case insensitive,
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
 * This is a small helper function for sequences with nucleic acid codes. It is case insensitive,
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
 * This is a small helper function for sequences with nucleic acid codes. It is case insensitive,
 * that is, lower case `t` is replaced by lower case `u`, and upper case `T` by upper case `U`.
 */
void replace_t_with_u( SequenceSet& set )
{
    for( auto& sequence : set ) {
        replace_t_with_u( sequence );
    }
}

/*
/ **
 * @brief Remove and delete all those sequences from a SequenceSet whose labels are in the given
 * list. If `invert` is set to true, it does the same inverted: it removes all except those in the
 * list.
 * /
void remove_list(SequenceSet& set, std::vector<std::string> const& labels, bool invert)
{
    // create a set of all labels for fast lookup.
    std::unordered_set<std::string> lmap(labels.begin(), labels.end());

    // iterate and move elements from it to re
    std::vector<Sequence*>::iterator it = sequences.begin();
    std::vector<Sequence*>::iterator re = sequences.begin();

    // this works similar to std::remove (http://www.cplusplus.com/reference/algorithm/remove/)
    while (it != sequences.end()) {
        // if the label is (not) in the map, move it to the re position, otherwise delete it.
        if ( (!invert && lmap.count((*it)->label())  > 0) ||
             ( invert && lmap.count((*it)->label()) == 0)
        ) {
            delete *it;
        } else {
            *re = std::move(*it);
            ++re;
        }
        ++it;
    }

    // delete the tail of the vector.
    sequences.erase(re, sequences.end());
}

*/

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
            set.remove_at(index);
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
            set.remove_at(index);
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
            set.remove_at(index);
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
