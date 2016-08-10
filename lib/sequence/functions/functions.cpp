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
#include "utils/core/logging.hpp"
#include "utils/text/string.hpp"
#include "utils/text/style.hpp"
#include "utils/tools/char_lookup.hpp"

#include <algorithm>
#include <array>
#include <assert.h>
#include <numeric>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

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
    std::map<char, size_t> sh;
    for( auto const& s : seq ) {
        ++sh[s];
    }
    return sh;
}

/**
 * @brief Get a histogram of the occurrences of particular sites, given a SequenceSet.
 *
 * This gives the raw counts of how often each site (character) appears in the whole set.
 * See base_frequencies() for the relative version of this function.
 */
std::map<char, size_t> site_histogram( SequenceSet const& set )
{
    std::map<char, size_t> sh;
    for( auto const& seq : set ) {
        for( auto const& s : seq ) {
            ++sh[s];
        }
    }
    return sh;
}

/**
 * @brief Local helper function that turns a site histogram into base frequencies.
 */
std::map<char, double> base_frequencies_accumulator(
    std::map<char, size_t> const& sitehistogram,
    std::string            const& plain_chars
) {
    // Calculate sum of raw counts of all chars given in plain_chars.
    size_t sum = 0;
    for( auto const& shp : sitehistogram ) {
        if( plain_chars.find( shp.first ) != std::string::npos ) {
            sum += shp.second;
        }
    }

    // Make relative.
    std::map<char, double> bf;
    for( auto const& pc : plain_chars ) {
        if( sitehistogram.count( pc )) {
            bf[pc] = static_cast<double>( sitehistogram.at(pc) ) / static_cast<double>( sum );
        }
    }
    return bf;
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
 * @brief Return the "gapyness" of the sequences, i.e., the proportion of gap chars
 * and other completely undetermined chars to the total length of all sequences.
 *
 * This function returns a value in the interval 0.0 (no gaps and undetermined chars at all)
 * and 1.0 (all chars are undetermined).
 * See `nucleic_acid_codes_undetermined()` and `amino_acid_codes_undetermined()` for presettings
 * of gap character that can be used here depending on the data set type.
 * The chars are treated case-insensitive.
 * In the special case that there are no sequences or sites, 0.0 is returned.
 */
double gapyness( SequenceSet const& set, std::string const& undetermined_chars )
{
    size_t gaps = count_chars( set, undetermined_chars );
    size_t len  = total_length( set );
    if( len == 0 ) {
        return 0.0;
    }

    double ret = static_cast<double>( gaps ) / static_cast<double>( len );
    assert( 0.0 <= ret && ret <= 1.0 );
    return ret;
}

/**
 * @brief Returns true iff all sequences only consist of the given `chars`.
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
 * @brief Return the total length (sum) of all sequences in the set.
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
 * @brief Return true iff all sequences in the set have the same length.
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
//     Print and Output
// =================================================================================================

// -------------------------------------------------------------------------
//     Helper Functions
// -------------------------------------------------------------------------

/**
 * @brief Local helper function for ostream and print to print one Sequence.
 *
 * See the print() functions for details about the parameters.
 */
void print_to_ostream(
    std::ostream&                      out,
    Sequence const&                    seq,
    std::map<char, std::string> const& colors,
    bool                               print_label,
    size_t                             length_limit,
    bool                               background
) {
    // Get the max number of sites to be printed.
    if( length_limit == 0 ) {
        length_limit = seq.length();
    }
    length_limit = std::min( length_limit, seq.length() );

    // Print label if needed.
    if( print_label ) {
        out << seq.label() << ": ";
    }

    // Print all chars of the sequence.
    for( size_t l = 0; l < length_limit; ++l ) {
        char s = seq[l];

        // If we want color, use it. Otherwise, print just the char.
        if( colors.size() > 0 ) {

            // We use map.at() here, which throws in case of invalid keys, so we don't have to.
            if( background ) {
                out << utils::Style( "black", colors.at(s) )( std::string( 1, s ) );
            } else {
                out << utils::Style( colors.at(s) )( std::string( 1, s ) );
            }

        } else {

            out << s;
        }
    }

    // Append ellipsis if needed.
    out << (seq.length() > length_limit ? " ...\n" : "\n");
}

/**
* @brief Local helper function for ostream and print to print a SequenceSet.
*
* See the print() functions for details about the parameters.
*/
void print_to_ostream(
    std::ostream&                      out,
    SequenceSet const&                 set,
    std::map<char, std::string> const& colors,
    bool                               print_label,
    size_t                             length_limit,
    size_t                             sequence_limit,
    bool                               background
) {
    // Get the max number of sequences to be printed.
    if( sequence_limit == 0 ) {
        sequence_limit = set.size();
    }
    sequence_limit = std::min( sequence_limit, set.size() );

    // Get longest label length.
    size_t label_len = 0;
    if( print_label ) {
        for( size_t i = 0; i < sequence_limit; ++i ) {
            label_len = std::max( label_len, set[i].label().size() );
        }
    }

    // Print sequences.
    for( size_t i = 0; i < sequence_limit; ++i ) {
        if( print_label ) {
            out << set[i].label() << ": " << std::string( label_len - set[i].label().size(), ' ' );
        }
        print_to_ostream( out, set[i], colors, false, length_limit, background );
    }

    // Append ellipsis if needed.
    if( set.size() > sequence_limit ) {
        out << "...\n";
    }

}

// -------------------------------------------------------------------------
//     Ostream
// -------------------------------------------------------------------------

/**
 * @brief Print a Sequence to an ostream in the form "label: sites".
 *
 * As this is meant for quickly having a look at the Sequence, only the first 100 sites are printed.
 * If you need all sites, use print().
 */
std::ostream& operator << ( std::ostream& out, Sequence    const& seq )
{
    print_to_ostream( out, seq, {}, true, 100, false );
    return out;
}

/**
 * @brief Print a SequenceSet to an ostream in the form "label: sites".
 *
 * As this is meant for quickly having a look at the SequenceSet, only the first 10 Sequences and
 * the first 100 sites of each are printed. If you need all sequences and sites, use print().
 */
std::ostream& operator << ( std::ostream& out, SequenceSet const& set )
{
    print_to_ostream( out, set, {}, true, 100, 10, false );
    return out;
}

// -------------------------------------------------------------------------
//     Print
// -------------------------------------------------------------------------

/**
 * @brief Return a Sequence in textual form.
 *
 * If the optional parameter `print_label` is true, a label is printed before the sequence in the
 * form "label: sites". Default is `true`.
 *
 * The optional parameter `length_limit` limites the output lenght to that many chars.
 * If set to 0, the whole Sequence is printed. Default is 100. This is useful to avoid line
 * wrapping. If the limit is lower than the acutal number of sites, ellipsis " ..." are appended.
 */
std::string print(
    Sequence const&                    seq,
    bool                               print_label,
    size_t                             length_limit
) {
    std::ostringstream stream;
    print_to_ostream( stream, seq, {}, print_label, length_limit, false );
    return stream.str();
}

/**
 * @brief Return a Sequence in textual form.
 *
 * See the Sequence version of this function for details. If the additional parameter
 * `sequence_limit` is set to a value other than 0, only this number of sequences are printed.
 * Default is 10. If the given limit is lower than the acutal number of sequences, ellipsis " ..."
 * are appended.
 */
std::string print(
    SequenceSet const&                 set,
    bool                               print_label,
    size_t                             length_limit,
    size_t                             sequence_limit
) {
    std::ostringstream stream;
    print_to_ostream( stream, set, {}, print_label, length_limit, sequence_limit, false );
    return stream.str();
}

// -------------------------------------------------------------------------
//     Print Color
// -------------------------------------------------------------------------

/**
 * @brief Return a string with the sites of the Sequence colored.
 *
 * This function returns a color view of the sites of the given Sequence, using utils::Style colors,
 * which can be displayed in a console/terminal. This is useful for visualizing the Sequence similar
 * to graphical alignment and sequence viewing tools.
 *
 * The function takes a map from sequences characters to their colors (see utils::Style for a list of
 * the available ones).
 * The presettings `nucleic_acid_text_colors()` and `amino_acid_text_colors()` for default sequence
 * types can be used as input for this parameter.
 * If the `colors` map does not contain a key for one of the chars in the sequence, the function
 * throws an `std::out_of_range` exception.
 *
 * The optional paramter `print_label` determines whether the sequence label is to be printed.
 * Default is `true`.
 *
 * The optional parameter `length_limit` limites the output lenght to that many chars.
 * If set to 0, the whole Sequence is used. Default is 100. This is useful to avoid line wrapping.
 * If the limit is lower than the acutal number of sites, ellipsis " ..." are appended.
 *
 * The parameter `background` can be used to control which part of the output is colored:
 * `true` (default) colors the text background and makes the foreground white, while `false` colors
 * the foreground of the text and leaves the background at its default.
 */
std::string print_color(
    Sequence const&                    seq,
    std::map<char, std::string> const& colors,
    bool                               print_label,
    size_t                             length_limit,
    bool                               background
) {
    std::ostringstream stream;
    print_to_ostream( stream, seq, colors, print_label, length_limit, background );
    return stream.str();
}

/**
 * @brief Return a string with the sites of a SequenceSet colored.
 *
 * See the Sequence version of this function for details.
 *
 * The additional parameter `sequence_limit` controls the number of sequences to be printed.
 * If set to 0, everything is printed. Default is 10. If this limit is lower than the actual number
 * of sequences, ellipsis " ..." are appended.
 *
 * Be aware that each character is colored separately, which results in a lot of formatted output.
 * This might slow down the terminal if too many sequences are printed at once.
 */
std::string print_color(
    SequenceSet const&                 set,
    std::map<char, std::string> const& colors,
    bool                               print_label,
    size_t                             length_limit,
    size_t                             sequence_limit,
    bool                               background
) {
    std::ostringstream stream;
    print_to_ostream( stream, set, colors, print_label, length_limit, sequence_limit, background );
    return stream.str();
}

// =============================================================================
//     Modifiers
// =============================================================================

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

// =============================================================================
//     Sequence Modifiers
// =============================================================================

/ **
 * @brief Calls remove_gaps() for every Sequence.
 * /
void SequenceSet::remove_gaps()
{
    for (Sequence* s : sequences) {
        s->remove_gaps();
    }
}

/ **
 * @brief Calls replace() for every Sequence.
 * /
void SequenceSet::replace(char search, char replace)
{
    for (Sequence* s : sequences) {
        s->replace(search, replace);
    }
}

// =============================================================================
//     Dump
// =============================================================================

/ **
 * @brief Gives a summary of the sequences names and their lengths for this set.
 * /
std::string SequenceSet::dump() const
{
    std::ostringstream out;
    for (Sequence* s : sequences) {
        out << s->label() << " [" << s->length() << "]\n";
    }
    return out.str();
}

// =============================================================================
//     Mutators
// =============================================================================

/ **
 * @brief Removes all occurences of `gap_char` from the sequence.
 * /
void Sequence::remove_gaps( std::string const& gap_chars )
{
    sites_.erase(std::remove(sites_.begin(), sites_.end(), gap_char), sites_.end());
}

void Sequence::compress_gaps( std::string const& gap_chars )
{
    ...
}

/ **
 * @brief Replaces all occurences of `search` by `replace`.
 * /
void Sequence::replace(char search, char replace)
{
    sites_ = utils::replace_all (sites_, std::string(1, search), std::string(1, replace));
}

*/

} // namespace sequence
} // namespace genesis
