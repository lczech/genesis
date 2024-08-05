#ifndef GENESIS_SEQUENCE_FORMATS_FASTQ_WRITER_H_
#define GENESIS_SEQUENCE_FORMATS_FASTQ_WRITER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
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

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

    #include <string_view>

#endif

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Sequence;
class SequenceSet;

// =================================================================================================
//     Fastq Writer
// =================================================================================================

/**
 * @brief Write Fastq data.
 *
 * This class provides simple facilities for writing Fastq data.
 *
 * Exemplary usage:
 *
 *     std::string outfile = "path/to/file.fastq";
 *     SequenceSet sequence_set;
 *
 *     FastqWriter().write( sequence_set, utils::to_file( outfile ));
 *
 * See FastqReader for a description of the Fastq format.
 * Sequences are written to Fastq format, using the Sanger encoding (phred score plus offset 33),
 * see also quality_encode_from_phred_score().
 *
 * We expect Sequence%s to have a phred quality score by default, and throw an exception otherwise.
 * To change this behaviour, and instead fill Sequence%s without quality scores with a constant
 * quality score value, use fill_missing_quality( unsigned char ).
 */
class FastqWriter
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    FastqWriter()  = default;
    ~FastqWriter() = default;

    FastqWriter( FastqWriter const& ) = default;
    FastqWriter( FastqWriter&& )      = default;

    FastqWriter& operator= ( FastqWriter const& ) = default;
    FastqWriter& operator= ( FastqWriter&& )      = default;

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

    /**
     * @brief Write a single Sequence to an output target, using the Fastq format.
     *
     * See the output target convenience functions utils::to_file(), utils::to_stream(), and
     * utils::to_string() for examples of how to obtain a suitable output target.
     */
    void write(
        Sequence const& sequence,
        std::shared_ptr<utils::BaseOutputTarget> target
    ) const;

    /**
     * @brief Write a single Sequence to an output target, using the Fastq format.
     *
     * This overload additionally takes the quality string as input, for cases where this is not
     * stored in the sequence itself. The provided quality string has to be either of the same length
     * as the sequece itself, or empty, in which case this function behaves according to
     * fill_missing_quality(). If the sequence itself already has a phred score, an exception is thrown.
     *
     * See the output target convenience functions utils::to_file(), utils::to_stream(), and
     * utils::to_string() for examples of how to obtain a suitable output target.
     */
    void write(
        Sequence const& sequence,
        std::string const& quality,
        std::shared_ptr<utils::BaseOutputTarget> target
    ) const;

    /**
     * @brief Write a SequenceSet to an output target, using the Fastq format.
     *
     * See the output target convenience functions utils::to_file(), utils::to_stream(), and
     * utils::to_string() for examples of how to obtain a suitable output target.
     */
    void write(
        SequenceSet const& sequence_set,
        std::shared_ptr<utils::BaseOutputTarget> target
    ) const;

    #if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

    /**
     * @brief Write a sequence in form of std::string_view instances on the data.
     *
     * This can for instance come from FastqInputViewStream. Note that the @p quality string is
     * just taken as-is, that is, we assume that it is already in the desired ASCII encoding.
     * If left empty, and fill_missing_quality() is `true`, it is instead filled with a dummy code.
     */
    void write(
        std::string_view const& label,
        std::string_view const& sites,
        std::string_view const& quality,
        std::shared_ptr<utils::BaseOutputTarget> target
    ) const;

    #endif

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    /**
     * @brief Set the line length, which determines after how many chars (Sequence sites) lines breaks
     * are inserted when writing the Fastq file.
     *
     * Default is `0`, which means, no line breaks are inserted. This is because many other parsing
     * tools are implemented lazily and expect sequences in Fastq format to be on one line,
     * so we use this by default. If set to any value greater than zero, line breaks are inserted
     * into both the sequence and the quality score lines so that the do not exceed the specified
     * length.
     *
     * The functions returns the FastqWriter object to allow fluent interfaces.
     */
    FastqWriter& line_length( size_t value )
    {
        line_length_ = value;
        return *this;
    }

    /**
     * @brief Get the current line length.
     *
     * See the setter line_length( size_t ) for details.
     */
    size_t line_length() const
    {
        return line_length_;
    }

    /**
     * @brief Set the value to fill the quality scores of sequences that do not have a phread
     * score assigned to them.
     *
     * By default, we do not accept sequences without phred scores assigned to them. If such a
     * sequence occurs while writing, an exception is thrown. To change that behaviour, set the
     * value to fill all sites of sequences that are missing their phread scores with this function.
     * All values in the range `[0, 254]` are valid, but the encoding used in Fastq only ranges
     * from `0` to `93` - all values above that are clamped to be `93`.
     * See quality_encode_from_phred_score() for details.
     *
     * Lastly, in order to restore the original behaviour (that is, throw an exception if a sequence
     * is missing phred scores), set this function to the magic value `255`.
     */
    FastqWriter& fill_missing_quality( unsigned char value )
    {
        fill_missing_quality_ = value;
        return *this;
    }

    /**
     * @brief Get the current value to fill missing phred quality scores.
     */
    unsigned char fill_missing_quality() const
    {
        return fill_missing_quality_;
    }

    /**
     * @brief Set whether to repeat the sequence identifier (label) on the third line of each
     * sequence.
     *
     * Fastq allows the third line (starting with a `+` char) to either only contain that char,
     * or to repeat the label of the first line (for whatever reason...). By default, we do not
     * write the label again, to save a bit of storage space. Use this function to change that
     * behaviour.
     */
    FastqWriter& repeat_label( bool value )
    {
        repeat_label_ = value;
        return *this;
    }

    /**
     * @brief Get whether the setting to repeat the sequence identifier (label) on the third line
     * is set.
     */
    bool repeat_label() const
    {
        return repeat_label_;
    }

    // ---------------------------------------------------------------------
    //     Internal Members
    // ---------------------------------------------------------------------

private:

    std::string make_filled_quality_string_(
        size_t length
    ) const;

    void write_sequence_(
        std::string const& label,
        std::string const& sites,
        std::string const& quality,
        std::ostream& os
    ) const;

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    size_t line_length_ = 0;
    unsigned char fill_missing_quality_ = 255;
    bool repeat_label_ = false;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
