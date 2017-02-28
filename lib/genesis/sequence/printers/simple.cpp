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

#include "genesis/sequence/printers/simple.hpp"

#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/text/style.hpp"

#include <algorithm>
#include <cassert>
#include <sstream>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Print
// =================================================================================================

/**
 * @brief Print a single Sequence to a stream.
 */
void PrinterSimple::print(
    std::ostream&   out,
    Sequence const& seq
) const {
    print_sequence( out, seq );
}

/**
 * @brief Print a SequenceSet to a stream.
 */
void PrinterSimple::print(
    std::ostream&   out,
    SequenceSet const& set
) const {
    // How many lines to print.
    size_t sequence_limit = ( sequence_limit_ > 0
        ? std::min( sequence_limit_, set.size() )
        : set.size()
    );

    // Get longest label length.
    size_t label_len = 0;
    if( label_mode_ == LabelMode::kSameLine ) {
        for( size_t i = 0; i < sequence_limit; ++i ) {
            label_len = std::max( label_len, set[i].label().size() );
        }
    }

    // Print sequences.
    for( size_t i = 0; i < sequence_limit; ++i ) {
        print_sequence( out, set[i], label_len );
    }

    // Append ellipsis if needed.
    if( set.size() > sequence_limit ) {
        out << "...\n";
    }
}

/**
 * @brief Return a string representing the print of a single Sequence.
 */
std::string PrinterSimple::print(
    Sequence const& seq
) const {
    std::ostringstream res;
    print( res, seq );
    return res.str();
}

/**
 * @brief Return a string representing the print of a SequenceSet.
 */
std::string PrinterSimple::print(
    SequenceSet const& set
) const {
    std::ostringstream res;
    print( res, set );
    return res.str();
}

/**
 * @brief Return a string representing the print of a single Sequence.
 *
 * This is simply an alias for print();
 */
std::string PrinterSimple::operator() (
    Sequence const& seq
) const {
    return print(seq);
}

/**
 * @brief Return a string representing the print of a SequenceSet.
 *
 * This is simply an alias for print();
 */
std::string PrinterSimple::operator() (
    SequenceSet const& set
) const {
    return print(set);
}

// =================================================================================================
//     Internal Functions
// =================================================================================================

void PrinterSimple::print_character(
    std::ostream&   out,
    char            site
) const {
    // Print plain if we use that mode or if there is no color in the map for this char.
    if( color_mode_ == ColorMode::kNone || color_map_.count( site ) == 0 ) {
        out << site;
    } else {
        auto color = color_map_.at( site );

        // Choose style.
        if( color_mode_ == ColorMode::kForeground ) {
            out << utils::Style( color)( std::string( 1, site ) );

        } else if( color_mode_ == ColorMode::kBackground ) {
            out << utils::Style( "black", color )( std::string( 1, site ) );

        } else {

            // Invalid color mode. There are only the ones used above.
            // In debug mode, this terminates; in release, it simply uses no color.
            out << site;
            assert( false );
        }
    }
}

void PrinterSimple::print_sites(
    std::ostream&   out,
    Sequence const& seq
) const {
    // Get the max number of sites to be printed.
    auto length_limit = ( length_limit_ > 0
        ? std::min( length_limit_, seq.length() )
        : seq.length()
    );

    // Print all chars of the sequence.
    for( size_t l = 0; l < length_limit; ++l ) {
        // Print new line if the length is > 0 and we are at a multiple of the length.
        if( l > 0 && line_length_ > 0 && l % line_length_ == 0 ) {
            out << "\n";
        }

        print_character( out, seq[l] );
    }

    // Append ellipsis if needed.
    out << ( seq.length() > length_limit ? " ...\n" : "\n" );
}

void PrinterSimple::print_sequence(
    std::ostream&   out,
    Sequence const& seq,
    size_t          label_len
) const {
    // Print label if needed.
     if( label_mode_ == LabelMode::kSeparateLine ) {
        out << seq.label() << "\n";

    } else if( label_mode_ == LabelMode::kSameLine ) {
        out << seq.label() << ": ";
        if( label_len > 0 && seq.label().size() > label_len ) {
            out << std::string( label_len - seq.label().size(), ' ' );
        }
    }

    print_sites( out, seq );
}

// =================================================================================================
//     Properties
// =================================================================================================

/**
 * @brief Set the limit for how many Sequence%s to print.
 *
 * If this parameter is set to a value other than 0, only this number of Sequence%s are printed.
 * Default is 0, thus, all Sequences are printed. If the given limit is lower than the acutal number
 * of Sequences in the SequenceSet, ellipsis " ..." are appended.
 */
PrinterSimple& PrinterSimple::sequence_limit( size_t value )
{
    sequence_limit_ = value;
    return *this;
}

/**
 * @brief Get the currently set limit for how many Sequence%s to print.
 *
 * See @link sequence_limit( size_t value ) the setter@endlink for details.
 */
size_t PrinterSimple::sequence_limit() const
{
    return sequence_limit_;
}

/**
 * @brief Set the length of each link, i.e., when to wrap.
 *
 * If this parameter is set to a value other than 0, the Sequence is wrapped at this line length.
 * This also works in combination with length_limit().
 */
PrinterSimple& PrinterSimple::line_length( size_t value )
{
    line_length_ = value;
    return *this;
}

/**
 * @brief Get the currently set line length, i.e., when to wrap.
 *
 * See @link line_length( size_t value ) the setter@endlink for details.
 */
size_t PrinterSimple::line_length() const
{
    return line_length_;
}

/**
 * @brief Set the length limit for printing Sequence%s.
 *
 * This parameter limits the output length to the give number of chars. If set to 0, the whole
 * Sequence is printed (default). If the limit is lower than the acutal number of sites in the
 * Sequence, ellipsis " ..." are appended.
 */
PrinterSimple& PrinterSimple::length_limit( size_t value )
{
    length_limit_ = value;
    return *this;
}

/**
 * @brief Get the currently set length limit.
 *
 * See @link length_limit( size_t value ) the setter@endlink for details.
 */
size_t PrinterSimple::length_limit() const
{
    return length_limit_;
}

/**
 * @brief Set the list of colors to use for the Sequence charaters.
 *
 * The provided map sets the color name for each Sequence character.
 * The presettings `nucleic_acid_text_colors()` and `amino_acid_text_colors()` for default sequence
 * types can be used as input for this parameter.
 * If the `colors` map does not contain a key for one of the chars in the sequence, the char is
 * printed without color.
 *
 * See @link color_mode( PrinterSimple::ColorMode value ) color_mode()@endlink for more details.
 * See utils::Style for details and for a list of the available color names.
 */
PrinterSimple& PrinterSimple::color_map( std::map<char, std::string> value )
{
    color_map_ = value;
    return *this;
}

/**
 * @brief Get the currently set list of colors for each Sequence character.
 *
 * See @link color_map( std::map<char, std::string> value ) the setter@endlink for details.
 */
std::map<char, std::string> PrinterSimple::color_map() const
{
    return color_map_;
}

/**
 * @brief Set whether to use color in the background, foreground, or not at all.
 *
 * See @link PrinterSimple::ColorMode ColorMode@endlink for details.
 */
PrinterSimple& PrinterSimple::color_mode( PrinterSimple::ColorMode value )
{
    color_mode_ = value;
    return *this;
}

/**
 * @brief Get the currently set color mode.
 *
 * See @link color_mode( PrinterSimple::ColorMode value ) the setter@endlink for details.
 */
PrinterSimple::ColorMode PrinterSimple::color_mode() const
{
    return color_mode_;
}

/**
 * @brief Set the currently set @link PrinterSimple::LabelMode LabelMode@endlink.
 *
 * See @link PrinterSimple::LabelMode LabelMode@endlink for more information.
 */
PrinterSimple& PrinterSimple::label_mode( PrinterSimple::LabelMode value )
{
    label_mode_ = value;
    return *this;
}

/**
 * @brief Get the currently set @link PrinterSimple::LabelMode LabelMode@endlink.
 *
 * See @link label_mode( PrinterSimple::LabelMode value ) the setter@endlink and
 * @link PrinterSimple::LabelMode LabelMode@endlink for more information.
 */
PrinterSimple::LabelMode PrinterSimple::label_mode() const
{
    return label_mode_;
}

} // namespace sequence
} // namespace genesis
