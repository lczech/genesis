#ifndef GENESIS_SEQUENCE_FORMATS_PHYLIP_WRITER_H_
#define GENESIS_SEQUENCE_FORMATS_PHYLIP_WRITER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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

#include "genesis/utils/io/output_target.hpp"

#include <iosfwd>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class SequenceSet;

// =================================================================================================
//     Phylip Writer
// =================================================================================================

class PhylipWriter
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    PhylipWriter()  = default;
    ~PhylipWriter() = default;

    PhylipWriter( PhylipWriter const& ) = default;
    PhylipWriter( PhylipWriter&& )      = default;

    PhylipWriter& operator= ( PhylipWriter const& ) = default;
    PhylipWriter& operator= ( PhylipWriter&& )      = default;

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

    // void to_stream_interleaved ( SequenceSet const& sequence_set, std::ostream& os ) const;
    // void to_stream_sequential  ( SequenceSet const& sequence_set, std::ostream& os ) const;

    /**
     * @brief Write a SequenceSet to an output target, using the Phylip format.
     *
     * See the output target convenience functions utils::to_file(), utils::to_stream(), and
     * utils::to_string() for examples of how to obtain a suitable output target.
     */
    void write( SequenceSet const& sequence_set, std::shared_ptr<utils::BaseOutputTarget> target ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    /**
     * @brief Set the length of the label in front of the sequences.
     *
     * Phylip has the weird property that labels are written in front of sequences and do not need
     * to have a delimiter, but instead are simply the first `n` characters of the string. This value
     * determines after how many chars the label ends and the actual sequence begins.
     *
     * If set to 0 (default), a relaxed version of Phylip is used, where the sequence begin is
     * automatically detected. Labels can then be of arbitrary lengths, as long as they do not contain
     * white spaces. After the label, a space is appended.
     *
     * If set to a value greater than 0, the label will be cut off after that many chars. For shorter
     * labels, the remaining number is filled with spaces.
     * The functions returns the PhylipWriter object to allow fluent interfaces.
     */
    PhylipWriter& label_length( size_t value );

    /**
     * @brief Return the currently set label length.
     *
     * See the setter label_length( size_t ) for details.
     */
    size_t label_length() const;

    /**
     * @brief Set the line length, which determines after how many chars (Sequence sites) lines breaks
     * are inserted when writing the Phylip file.
     *
     * Default is `80`. If set to `0`, no breaks are inserted.
     * The functions returns the PhylipWriter object to allow fluent interfaces.
     */
    PhylipWriter& line_length( size_t value );

    /**
     * @brief Get the current line length.
     *
     * See the setter line_length( size_t ) for details.
     */
    size_t line_length() const;

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    size_t label_length_ = 0;
    size_t line_length_  = 80;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
