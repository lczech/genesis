#ifndef GENESIS_SEQUENCE_FORMATS_PHYLIP_READER_H_
#define GENESIS_SEQUENCE_FORMATS_PHYLIP_READER_H_

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

#include "genesis/utils/tools/char_lookup.hpp"

#include <iosfwd>
#include <string>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {

class InputStream;

} // namespace utils

namespace sequence {

class SequenceSet;
class Sequence;

// =================================================================================================
//     Phylip Reader
// =================================================================================================

/**
 * @brief Read Phylip sequence data.
 *
 * This class provides simple facilities for reading Phylip data. It supports to read
 *
 *   * from_stream()
 *   * from_file()
 *   * from_string()
 *
 * Exemplary usage:
 *
 *     std::string infile = "path/to/file.phylip";
 *     SequenceSet sset;
 *
 *     PhylipReader()
 *         .to_upper()
 *         .valid_chars( nucleic_acid_codes_all() )
 *         .from_file( infile, sset );
 *
 * The expected data format roughly follows
 * [the original definition](http://evolution.genetics.washington.edu/phylip/doc/sequence.html).
 * See mode( Mode ) to selected between sequential, interleaved and automatic mode.
 * We furthermore support a relaxed version (by default), where the label can be of any length.
 * See label_length( size_t ) for more information.
 *
 * Using to_upper( bool ), the sequences can automatically be turned into upper case letter.
 * Also, see valid_chars( std::string const& ) for a way of checking correct input sequences.
 */
class PhylipReader
{
public:

    // ---------------------------------------------------------------------
    //     Types and Enums
    // ---------------------------------------------------------------------

    /**
     * @brief Helper that stores the header information of a Phylip file.
     */
    struct Header
    {
        /**
         * @brief Number of sequences in the Phylip file.
         */
        size_t      num_sequences = 0;

        /**
         * @brief Length of the sequences in the Phylip file.
         */
        size_t      len_sequences = 0;

        /**
         * @brief Store the options that might be at the end of the header line.
         *
         * Some Phylip files contain special options after the two mandatory numbers in the header
         * line. This data can be stored here. It is currenlty not used.
         *
         * Some programs use magic values for those options: If the (trimmed) remainder of the
         * header line is simply "s" or "i", this is used as a trigger to distinguish
         * between sequential and interleaved Phylip files. This is however currently not used.
         * See Mode for more information.
         */
        std::string options;
    };

    /**
     * @brief Enum to distinguish between the different file variants of Phylip.
     * See mode( Mode value ) for more details.
     */
    enum class Mode
    {
        /**
         * @brief Read the data in Phylip sequential mode.
         */
        kSequential,

        /**
        * @brief Read the data in Phylip interleaved mode.
        */
        kInterleaved,

        /**
        * @brief Infer the Phylip mode via trial and error.
        */
        kAutomatic
    };

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    /**
     * @brief Create a default PhylipReader. Per default, chars are turned upper case, but not validated.
     *
     * See to_upper() and valid_chars() to change this behaviour.
     */
    PhylipReader();

    ~PhylipReader() = default;

    PhylipReader( PhylipReader const& ) = default;
    PhylipReader( PhylipReader&& )      = default;

    PhylipReader& operator= ( PhylipReader const& ) = default;
    PhylipReader& operator= ( PhylipReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    /**
     * @brief Read all Sequence%s from a std::istream in Phylip format into a SequenceSet.
     *
     * The Sequences are added to the SequenceSet, whose existing Sequences are kept. Thus, by
     * repeatedly calling this or similar read functions, multiple input files can easily be read
     * into one SequenceSet.
     *
     * This function is only allowed for Mode::kSequential and Mode::kInterleaved. Automatic mode
     * does not work, as the stream might need to be reset, which is not possible. See mode(Mode)
     * for details.
     */
    void from_stream( std::istream& input_stream, SequenceSet& sequence_set ) const;

    /**
     * @brief Read all Sequence%s from a std::istream in Phylip format and return them as a
     * SequenceSet.
     *
     * This function is only allowed for Mode::kSequential and Mode::kInterleaved. Automatic mode
     * does not work, as the stream might need to be reset, which is not possible. See mode(Mode)
     * for details.
     */
    SequenceSet from_stream( std::istream& input_stream ) const;

    /**
     * @brief Read all Sequence%s from a file in Phylip format into a SequenceSet.
     *
     * The Sequences are added to the SequenceSet, whose existing Sequences are kept. Thus, by
     * repeatedly calling this or similar read functions, multiple input files can easily be read
     * into one SequenceSet.
     */
    void from_file( std::string const& file_name, SequenceSet& sequence_set ) const;

    /**
     * @brief Read all Sequence%s from a file in Phylip format and return them as a
     * SequenceSet.
     */
    SequenceSet from_file( std::string const& file_name ) const;

    /**
     * @brief Read all Sequence%s from a std::string in Phylip format into a SequenceSet.
     *
     * The Sequences are added to the SequenceSet, whose existing Sequences are kept. Thus, by
     * repeatedly calling this or similar read functions, multiple input files can easily be read
     * into one SequenceSet.
     */
    void from_string( std::string const& input_string, SequenceSet& sequence_set ) const;

    /**
     * @brief Read all Sequence%s from a std::string in Phylip format and return them as a
     * SequenceSet.
     */
    SequenceSet from_string( std::string const& input_string ) const;

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    /**
     * @brief Parse a Phylip header and return the contained sequence count and length.
     *
     * This helper function expects to find a Phylip header line in the form `x y`, which describes
     * the number of sequences `x` in the Phylip data and their length `y`. The remainder of the
     * header line is interpreted as Phylip options. See Header struct for more information.
     *
     * The function then advances the stream and skips potential empty lines after the header. It
     * thus leaves the stream at the beginning of the first sequence line.
     */
    Header parse_phylip_header(
        utils::InputStream& it
    ) const;

    /**
     * @brief Parse and return a Phylip label.
     *
     * This helper functions either takes the first `label_length` chars as a label or, if
     * `label_length == 0` takes all chars until the first blank as label. It returns the trimmed
     * label and leaves the stream at the next char after the label (and after subsequent blanks).
     */
    std::string parse_phylip_label(
        utils::InputStream& it
    ) const;

    /**
     * @brief Parse one sequence line.
     *
     * The line (which can also start after a label) is parsed until the first '\\n' char.
     * While parsing, the options to_upper() and valid_chars() are applied according to their
     * settings. The stream is left at the beginning of the next line.
     */
    std::string parse_phylip_sequence_line(
        utils::InputStream& it
    ) const;

    /**
     * @brief Parse a whole Phylip file using the sequential variant (Mode::kSequential).
     */
    void parse_phylip_interleaved(
        utils::InputStream& it,
        SequenceSet& sset
    ) const;

    /**
     * @brief Parse a whole Phylip file using the interleaved variant (Mode::kInterleaved).
     */
    void parse_phylip_sequential(
        utils::InputStream& it,
        SequenceSet& sset
    ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    /**
     * @brief Set the mode for reading sequences.
     *
     * Phylip offers two variants for storing the sequences: sequential and interleaved. As there is no
     * option or flag in the file itself, there is no chance of knowing the variant without trying to
     * parse it. If one fails but not the other, it is proabably the latter variant. However, there are
     * instances where both variants are valid at the same time, but yield different sequences.
     * So, in general detecting the correct variant is undecidable, making Phylip a non-well-defined
     * format.
     *
     * In order to avoid those problems, this function explicitly sets the variant being used for
     * parsing. By default, it is set to Mode::kSequential. Use Mode::kInterleaved for the other
     * variant.
     *
     * We also offer a Mode::kAutomatic. It first tries to parse in sequential mode, and, if this fails,
     * in interleaved mode. However, as this might involve starting from the beginning of the data, this
     * is only possible with the from_file() and from_string() readers and does not work when using the
     * from_stream() reader. Also, be aware that using automatic mode is slower because of
     * implementation details induced by those limitations.
     * Try to avoid automatic mode. If possible, try to avoid Phylip at all.
     */
    PhylipReader& mode( Mode value );

    /**
     * Return the currently set mode for parsing Phylip.
     *
     * See the setter mode( Mode ) for details.
     */
    Mode          mode() const;

    /**
     * @brief Set the length of the label in front of the sequences.
     *
     * Phylip has the weird property that labels are written in front of sequences and do not need
     * to have a delimiter, but instead are simply the first `n` characters of the string. This value
     * determines after how many chars the label ends and the actual sequence begins.
     *
     * If set to a value greater than 0, exaclty this many characters are read as label. Thus, they
     * can also contain spaces. Spaces at the beginning or end of a label are stripped. The length
     * that is dictated by the Phylip standard is 10, but any other length can also be used.
     *
     * If set to 0 (default), a relaxed version of Phylip is used instead, where the sequence begin is
     * automatically detected. Labels can then be of arbitrary lengths, as long as they do not contain
     * white spaces. However, in this case, there has to be at least one space or tab character between
     * the label and the sequence. After the whitespace(s), the rest of the line is then treated
     * as sequence data.
     *
     * The function returns the PhylipReader object to allow for fluent interfaces.
     */
    PhylipReader& label_length( size_t value );

    /**
     * @brief Return the currently set label length.
     *
     * See the setter label_length( size_t ) for details.
     */
    size_t        label_length() const;

    /**
     * @brief Set whether Sequence sites are automatically turned into upper case.
     *
     * If set to `true` (default), all sites of the read Sequences are turned into upper case letters
     * automatically. This is demanded by the Phylip standard.
     *
     * The function returns the PhylipReader object to allow for fluent interfaces.
     */
    PhylipReader& to_upper( bool value );

    /**
     * @brief Return whether Sequence sites are automatically turned into upper case.
     */
    bool          to_upper() const;

    /**
     * @brief Set the chars that are used for validating Sequence sites when reading them.
     *
     * When this function is called with a string of chars, those chars are used to validate the sites
     * when reading them. That is, only sequences consisting of the given chars are valid.
     *
     * If set to an empty string, this check is deactivated. This is also the default, meaning that no
     * checking is done.
     *
     * In case that to_upper() is set to `true`: The validation is done after making the char upper
     * case, so that only capital letters have to be provided for validation.
     * In case that to_upper() is set to `false`: All chars that are to be considered valid have to be
     * provided for validation.
     *
     * See `nucleic_acid_codes...()` and `amino_acid_codes...()` functions for presettings of chars
     * that can be used for validation here.
     */
    PhylipReader& valid_chars( std::string const& chars );

    /**
     * @brief Return the currently set chars used for validating Sequence sites.
     *
     * An empty string means that no validation is done.
     */
    std::string   valid_chars() const;

    /**
     * @brief Return the internal CharLookup that is used for validating the Sequence sites.
     *
     * This function is provided in case direct access to the lookup is needed. Usually, the
     * valid_chars() function should suffice. See there for details.
     */
    utils::CharLookup<bool>& valid_char_lookup();

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    Mode                    mode_           = Mode::kSequential;
    size_t                  label_length_   = 0;

    bool                    to_upper_       = true;
    bool                    use_validation_ = false;
    utils::CharLookup<bool> lookup_;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
