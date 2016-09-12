#ifndef GENESIS_SEQUENCE_PRINTERS_SIMPLE_H_
#define GENESIS_SEQUENCE_PRINTERS_SIMPLE_H_

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
         * @brief The label is printed on a line preceeding the Sequence sites.
         */
        kSameLine,

        /**
         * @brief The label is printed on the line where the Sequence sites start, separated
         * from them by ": ".
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

    void print(
        std::ostream&   out,
        Sequence const& seq
    ) const;

    void print(
        std::ostream&   out,
        SequenceSet const& set
    ) const;

    std::string print(
        Sequence const& seq
    ) const;

    std::string print(
        SequenceSet const& set
    ) const;

    std::string operator() (
        Sequence const& seq
    ) const;

    std::string operator() (
        SequenceSet const& set
    ) const;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    PrinterSimple& sequence_limit( size_t value );
    size_t         sequence_limit() const;

    PrinterSimple& line_length( size_t value );
    size_t         line_length() const;

    PrinterSimple& length_limit( size_t value );
    size_t         length_limit() const;

    PrinterSimple& color_map( std::map<char, std::string> value );
    std::map<char, std::string> color_map() const;

    PrinterSimple& color_mode( ColorMode value );
    ColorMode      color_mode() const;

    PrinterSimple& label_mode( LabelMode value );
    LabelMode      label_mode() const;

    // -------------------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------------------

private:

    void print_character(
        std::ostream&   out,
        char            site
    ) const;

    void print_sites(
        std::ostream&   out,
        Sequence const& seq
    ) const;

    void print_sequence(
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
