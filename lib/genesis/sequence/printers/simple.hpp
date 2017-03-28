#ifndef GENESIS_SEQUENCE_PRINTERS_SIMPLE_H_
#define GENESIS_SEQUENCE_PRINTERS_SIMPLE_H_

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

#include <iosfwd>
#include <map>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Sequence;
class SequenceSet;

// =================================================================================================
//     Printer Simple
// =================================================================================================

/**
 * @brief Simple printer class for Sequence%s and SequenceSet%s.
 *
 * It provides to print() Sequence%s to strings and ostreams.
 *
 * By default, no colors are set, thus, Sequence%s are printed without color. You have to set the
 * @link color_map( std::map<char, std::string> const& ) color_map()@endlink first, for example using
 * the colors defined in nucleic_acid_text_colors() or amino_acid_text_colors().
 */
class PrinterSimple
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief Modes for how the Sequence sites are colored.
     *
     * In order for the coloring to work, a map from site characters to the name of the color
     * needs to be provided via color_map(). See there for details.
     *
     * The coloring is applied using escape sequences. This works on most modern terminals.
     * See utils::Style for details and for a list of the available color names.
     */
    enum class ColorMode {
        /**
         * @brief No color, even if a color_map() is provided.
         */
        kNone,

        /**
         * @brief Color the text foreground of the characters, leave the background at default.
         */
        kForeground,

        /**
         * @brief Color the text background of the characters, set the foreground to black.
         */
        kBackground
    };

    /**
     * @brief Modes for how to print Sequence labels.
     */
    enum class LabelMode {
        /**
         * @brief No label is printed.
         */
        kNone,

        /**
         * @brief The label is printed on the line where the Sequence sites start, separated
         * from them by ": ".
         */
        kSameLine,

        /**
         * @brief The label is printed on a line preceeding the Sequence sites.
         */
        kSeparateLine
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    PrinterSimple()  = default;
    ~PrinterSimple() = default;

    PrinterSimple( PrinterSimple const& ) = default;
    PrinterSimple( PrinterSimple&& )      = default;

    PrinterSimple& operator= ( PrinterSimple const& ) = default;
    PrinterSimple& operator= ( PrinterSimple&& )      = default;

    // -------------------------------------------------------------------------
    //     Print
    // -------------------------------------------------------------------------

    /**
     * @brief Print a single Sequence to a stream.
     */
    void print(
        std::ostream&   out,
        Sequence const& seq
    ) const;

    /**
     * @brief Print a SequenceSet to a stream.
     */
    void print(
        std::ostream&   out,
        SequenceSet const& set
    ) const;

    /**
     * @brief Return a string representing the print of a single Sequence.
     */
    std::string print(
        Sequence const& seq
    ) const;

    /**
     * @brief Return a string representing the print of a SequenceSet.
     */
    std::string print(
        SequenceSet const& set
    ) const;

    /**
     * @brief Return a string representing the print of a single Sequence.
     *
     * This is simply an alias for print();
     */
    std::string operator() (
        Sequence const& seq
    ) const;

    /**
     * @brief Return a string representing the print of a SequenceSet.
     *
     * This is simply an alias for print();
     */
    std::string operator() (
        SequenceSet const& set
    ) const;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Set the limit for how many Sequence%s to print.
     *
     * If this parameter is set to a value other than 0, only this number of Sequence%s are printed.
     * Default is 0, thus, all Sequences are printed. If the given limit is lower than the acutal number
     * of Sequences in the SequenceSet, ellipsis " ..." are appended.
     */
    PrinterSimple& sequence_limit( size_t value );

    /**
     * @brief Get the currently set limit for how many Sequence%s to print.
     *
     * See @link sequence_limit( size_t value ) the setter@endlink for details.
     */
    size_t         sequence_limit() const;

    /**
     * @brief Set the length of each line, i.e., when to wrap.
     *
     * If this parameter is set to a value other than 0, the Sequence is wrapped at this line length.
     * This also works in combination with length_limit().
     */
    PrinterSimple& line_length( size_t value );

    /**
     * @brief Get the currently set line length, i.e., when to wrap.
     *
     * See @link line_length( size_t value ) the setter@endlink for details.
     */
    size_t         line_length() const;

    /**
     * @brief Set the length limit for printing Sequence%s.
     *
     * This parameter limits the output length to the give number of chars. If set to 0, the whole
     * Sequence is printed (default). If the limit is lower than the acutal number of sites in the
     * Sequence, ellipsis " ..." are appended.
     */
    PrinterSimple& length_limit( size_t value );

    /**
     * @brief Get the currently set length limit.
     *
     * See @link length_limit( size_t value ) the setter@endlink for details.
     */
    size_t         length_limit() const;

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
    PrinterSimple& color_map( std::map<char, std::string> const& value );

    /**
     * @brief Get the currently set list of colors for each Sequence character.
     *
     * See @link color_map( std::map<char, std::string> const& ) the setter@endlink for details.
     */
    std::map<char, std::string> const& color_map() const;

    /**
     * @brief Set whether to use color in the background, foreground, or not at all.
     *
     * See @link PrinterSimple::ColorMode ColorMode@endlink for details.
     */
    PrinterSimple& color_mode( ColorMode value );

    /**
     * @brief Get the currently set color mode.
     *
     * See @link color_mode( PrinterSimple::ColorMode value ) the setter@endlink for details.
     */
    ColorMode      color_mode() const;

    /**
     * @brief Set the currently set @link PrinterSimple::LabelMode LabelMode@endlink.
     *
     * See @link PrinterSimple::LabelMode LabelMode@endlink for more information.
     */
    PrinterSimple& label_mode( LabelMode value );

    /**
     * @brief Get the currently set @link PrinterSimple::LabelMode LabelMode@endlink.
     *
     * See @link label_mode( PrinterSimple::LabelMode value ) the setter@endlink and
     * @link PrinterSimple::LabelMode LabelMode@endlink for more information.
     */
    LabelMode      label_mode() const;

    // -------------------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------------------

private:

    void print_character_(
        std::ostream&   out,
        char            site
    ) const;

    void print_sites_(
        std::ostream&   out,
        Sequence const& seq
    ) const;

    void print_sequence_(
        std::ostream&   out,
        Sequence const& seq,
        size_t          label_len = 0
    ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    size_t sequence_limit_ = 0;
    size_t line_length_    = 0;
    size_t length_limit_   = 0;

    std::map<char, std::string> color_map_;

    ColorMode color_mode_  = ColorMode::kBackground;
    LabelMode label_mode_  = LabelMode::kSameLine;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
