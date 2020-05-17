#ifndef GENESIS_SEQUENCE_FORMATS_FASTA_WRITER_H_
#define GENESIS_SEQUENCE_FORMATS_FASTA_WRITER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

class Sequence;
class SequenceSet;

// =================================================================================================
//     Fasta Writer
// =================================================================================================

/**
 * @brief Write Fasta data.
 *
 * This class provides simple facilities for writing Fasta data.
 *
 * Exemplary usage:
 *
 *     std::string outfile = "path/to/file.fasta";
 *     SequenceSet sequence_set;
 *
 *     FastaWriter().line_length( 100 ).write( sequence_set, utils::to_file( outfile ));
 *
 * See FastaReader for a description of the Fasta format.
 */
class FastaWriter
{
public:

    // ---------------------------------------------------------------------
    //     Typedefs and Enums
    // ---------------------------------------------------------------------

    /**
     * @brief Enumeration of types for how to write Sequence abundances.
     */
    enum class AbundanceNotation
    {
        /**
         * @brief Do not write abundances. This is the default.
         */
        kNone,

        /**
         * @brief Write abundances appaneded by an underscore: `abc_123`.
         */
        kUnderscore,

        /**
         * @brief Write abundances appended as a text of the form `abc;size=123`
         */
        kSize
    };

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    FastaWriter()  = default;
    ~FastaWriter() = default;

    FastaWriter( FastaWriter const& ) = default;
    FastaWriter( FastaWriter&& )      = default;

    FastaWriter& operator= ( FastaWriter const& ) = default;
    FastaWriter& operator= ( FastaWriter&& )      = default;

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

    /**
     * @brief Write a single Sequence to an output target, using the Fasta format.
     *
     * See the output target convenience functions utils::to_file(), utils::to_stream(), and
     * utils::to_string() for examples of how to obtain a suitable output target.
     */
    void write( Sequence const& sequence, std::shared_ptr<utils::BaseOutputTarget> target ) const;

    /**
     * @brief Write a SequenceSet to an output target, using the Fasta format.
     *
     * See the output target convenience functions utils::to_file(), utils::to_stream(), and
     * utils::to_string() for examples of how to obtain a suitable output target.
     */
    void write( SequenceSet const& sequence_set, std::shared_ptr<utils::BaseOutputTarget> target ) const;

    /**
     * @brief Write a single Sequence to an output stream in Fasta format.
     */
    void write_sequence( Sequence const& sequence, std::ostream& os ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    /**
     * @brief Set the line length, which determines after how many chars (Sequence sites) lines breaks
     * are inserted when writing the Fasta file.
     *
     * Default is `80`. If set to `0`, no breaks are inserted.
     * The functions returns the FastaWriter object to allow fluent interfaces.
     */
    FastaWriter& line_length( size_t value );

    /**
     * @brief Get the current line length.
     *
     * See the setter line_length( size_t ) for details.
     */
    size_t       line_length() const;

    /**
     * @brief Set the notation for how to write Sequence abundances.
     *
     * Default is to not write abundances. Use this setting to change that.
     */
    FastaWriter& abundance_notation( AbundanceNotation value );

    /**
     * @brief Get the current setting for how to write abundances.
     */
    AbundanceNotation abundance_notation() const;

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    size_t line_length_ = 80;
    AbundanceNotation abundance_notation_ = AbundanceNotation::kNone;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
