#ifndef GENESIS_SEQUENCE_FORMATS_FASTQ_READER_H_
#define GENESIS_SEQUENCE_FORMATS_FASTQ_READER_H_

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "genesis/sequence/functions/quality.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/tools/char_lookup.hpp"
#include "genesis/utils/io/input_source.hpp"

#include <functional>
#include <iosfwd>
#include <memory>
#include <string>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {
    class InputStream;
}

namespace sequence {
    class SequenceSet;
    class Sequence;
}

// =================================================================================================
//     Fastq Reader
// =================================================================================================

namespace sequence {

/**
 * @brief Read Fastq sequence data.
 *
 * This class provides simple facilities for reading Fastq data.
 *
 * Exemplary usage:
 *
 *     std::string infile = "path/to/file.fastq";
 *     SequenceSet sequence_set;
 *
 *     FastqReader()
 *         .site_casing( SiteCasing::kUnchanged )
 *         .valid_chars( nucleic_acid_codes_all() )
 *         .read( utils::from_file( infile ), sequence_set );
 *
 * The expected data format is:
 *
 *  1. Line 1 begins with a '@' character and is followed by a sequence identifier (label)
 *     and an optional description (like a FASTA title line, see FastaReader for details).
 *  2. Line 2 (or more) is the raw sequence letters. In contrast to most other readers,
 *     we allow the sequence to use several lines.
 *  3. Line 3 begins with a '+' character and is optionally followed by the same sequence identifier
 *     (and any description) again. If this line is not empty, it has to be identical to line 1.
 *  4. Line 4 (or more) encodes the quality values for the sequence in Line 2, and must contain
 *     the same number of symbols as there were letters in the sequence (line 2).
 *
 * See https://en.wikipedia.org/wiki/FASTQ_format for details.
 *
 * As the encoding for the quality values can be substantially different depending on the sequencing
 * techonology used, parsing fastq files is more difficult than fasta. Two issues arise:
 *
 *  *  The quality encoding can be different depending on the used sequencing techonology. The most
 *     prominent difference is the used ASCII base for the phred quality scores.
 *     See https://en.wikipedia.org/wiki/FASTQ_format for a thorough discussion, or the article
 *     cited below [1]. Solexa even uses a different function to compute scores, making it even more
 *     complicated. We tried to make the standard use case as easy as possible, as explained below.
 *  *  Most parsers expect the four lines as above without line breaks in between them. This is because
 *     the quality encoding might use the characters '@' and '+', which are also used as the starting
 *     characters for the first and third line, respectively (we here ignore the fact that,
 *     in theory, the seqeunce letters themselves could also be different than 'ACGT' and their
 *     degenerates, as this is also not defined in the format...).<br />
 *     This simple format does work here as well. However, we are nice and also
 *     support line breaks. <br />
 *     There is only one edge case where this breaks. If the sequence sites (line 2) contain a '+'
 *     character at the beginning of a wrapped line (i.e., immediately after a line break),
 *     we cannot distinguish this from the beginning of line 3.
 *     Unfortunately, this is an issue of the format itself that cannot be solved in a parser, as
 *     this is simply ill-defined. <br />
 *     However, standard nucleic acid or amino acid codes do not use the `+` character, so
 *     this should rarely be an issue in practice.
 *
 * By default, we interpret quality values as phred scores in the Sanger format, that is,
 * use an ASCII offset of 33, where '!' stands for the lowest phred quality score of 0.
 * To change the encoding, use the quality_encoding() function, which accepts Sanger, Solexa,
 * and different Illumina versions.
 *
 * For even more advanced used cases, the whole function for parsing the quality string can be changed
 * as well, by setting the quality_string_plugin() function. This is for example useful if the
 * quality scores are not needed at all (simply provide an empty function in this case), or if
 * the file is first parsed once to detect the most probably encoding - see
 * guess_fastq_quality_encoding() for an example.
 *
 * To set the the quality_string_plugin(), use for example the following:
 *
 *     auto reader = FastqReader();
 *     reader.quality_string_plugin(
 *         [&]( std::string const& quality_string, Sequence& sequence )
 *         {
 *             // do something with the quality_string, and potentially store it in the sequence
 *         }
 *     );
 *     reader.read( utils::from_file( infile ), sequence_set );
 *
 * More information on the format can be found at:
 *
 * > [1] P. Cock, C. Fields, N. Goto, M. Heuer, P. Rice.<br>
 * > <b>"The Sanger FASTQ file format for sequences with quality scores,
 * > and the Solexa/Illumina FASTQ variants."</b><br>
 * > <i>Nucleic Acids Research</i>, 38(6), 1767–1771, 2009.<br>
 * > https://doi.org/10.1093/nar/gkp1137
 *
 * Using site_casing(), the sequences can automatically be turned into upper or lower case letter.
 * Also, see valid_chars( std::string const& chars ) for a way of checking correct input sequences.
 */
class FastqReader
{
public:

    // ---------------------------------------------------------------------
    //     Typedefs and Enums
    // ---------------------------------------------------------------------

    /**
     * @brief Function type that allows to work with the quality line(s) in fastq files.
     *
     * This reader class is adjustable towards the encoding and usage of the quality line(s) in
     * fastq files. Typically, these lines contain some encoding of the phread quality score of the
     * bases found in the sequence string. However, as there are several variants for this encoding,
     * and as not always the quality score is needed at all, we leave the usage of the quality
     * string adjustable.
     *
     * This function type here can hence be used to process the quality_string, for example
     * by storing it, or processing it to find the correct encoding first.
     * Use quality_string_plugin() to set an according function.
     */
    using quality_string_function = std::function< void(
        std::string const& quality_string, Sequence& sequence
    ) >;

    /**
     * @brief Enumeration of casing methods to apply to each site of a Sequence.
     */
    enum class SiteCasing
    {
        /**
         * @brief Do not change the case of the sites.
         */
        kUnchanged,

        /**
         * @brief Make all sites upper case.
         */
        kToUpper,

        /**
         * @brief Make all sites lower case.
         */
        kToLower
    };

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    /**
     * @brief Create a default FastqReader.
     *
     * Per default, chars are turned upper case, but not validated.
     * See site_casing() and valid_chars() to change this behaviour.
     *
     * Furthermore, by default, we interpret the quality score scrint as being phred scores
     * in the Sanger format.
     */
    FastqReader();
    ~FastqReader() = default;

    FastqReader( FastqReader const& ) = default;
    FastqReader( FastqReader&& )      = default;

    FastqReader& operator= ( FastqReader const& ) = default;
    FastqReader& operator= ( FastqReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    /**
     * @brief Read all Sequence%s from an input source in Fastq format and return them as a
     * SequenceSet.
     *
     * Use functions such as utils::from_file() and utils::from_string() to conveniently
     * get an input source that can be used here.
     */
    SequenceSet read( std::shared_ptr< utils::BaseInputSource > source ) const;

    /**
     * @brief Read all Sequence%s from an input source in Fastq format into a SequenceSet.
     *
     * The Sequences are added to the SequenceSet, whose existing Sequences are kept. Thus, by
     * repeatedly calling this or similar read functions, multiple input files can easily be read
     * into one SequenceSet.
     *
     * Use functions such as utils::from_file() and utils::from_string() to conveniently
     * get an input source that can be used here.
     */
    void read( std::shared_ptr< utils::BaseInputSource > source, SequenceSet& sequence_set ) const;

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    /**
     * @brief Parse a whole fastq document into a SequenceSet.
     *
     * This function is mainly used internally by the reading functions read().
     * It is however also fine to call it from the outside.
     */
    void parse_document(
        utils::InputStream& input_stream,
        SequenceSet&        sequence_set
    ) const;

    /**
     * @brief Parse a Sequence in Fastq format.
     *
     * This function takes an utils::InputStream and interprets it as a Fastq formatted sequence.
     * It extracts the data and writes it into the given Sequence object. See the class description
     * of FastqReader for the expected data format.
     *
     * The function stops after parsing one such sequence, and leaves the stream at the first
     * character of the next line that follows the quality score string. It returns true if a
     * sequence was extracted and false if the stream is empty. If the input is not in the correct
     * format, an `std::runtime_error` exception is thrown indicating the malicious position in the
     * input stream.
     */
    bool parse_sequence(
        utils::InputStream& input_stream,
        Sequence&           sequence
    ) const;

protected:

    /**
     * @brief Parse a fastq sequence into the given @p sequence object.
     */
    bool parse_sequence_( utils::InputStream& input_stream, Sequence& sequence ) const;

    /**
     * @brief Parse the first label line (starting with an @).
     */
    void parse_label1_(   utils::InputStream& input_stream, Sequence& sequence ) const;

    /**
     * @brief Parse the sequence line(s).
     */
    void parse_sites_(    utils::InputStream& input_stream, Sequence& sequence ) const;

    /**
     * @brief Parse the second label line (starting with a +, and either empty or equal to the first).
     */
    void parse_label2_(   utils::InputStream& input_stream, Sequence& sequence ) const;

    /**
     * @brief Parse the quality score line(s), which also runs the plugin, if available.
     */
    void parse_quality_(  utils::InputStream& input_stream, Sequence& sequence ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

public:

    /**
     * @brief Set whether Sequence sites are automatically turned into upper or lower case.
     *
     * Default is SiteCasing::kToUpper, that is, all sites of the read Sequences are turned into
     * upper case letters automatically. This is typical behaviour, although not standardized.
     * The function returns the FastqReader object to allow for fluent interfaces.
     */
    FastqReader& site_casing( SiteCasing value );

    /**
     * @brief Return whether Sequence sites are automatically turned into upper or lower case.
     */
    SiteCasing site_casing() const;

    /**
     * @brief Set the chars that are used for validating Sequence sites when reading them.
     *
     * When this function is called with a string of chars, those chars are used to validate the
     * sites when reading them. That is, only sequences consisting of the given chars are valid.
     *
     * If set to an empty string, this check is deactivated. This is also the default, meaning that
     * no checking is done.
     *
     * In case that site_casing() is set to a value other than `SiteCasing::kUnchanged`:
     * The validation is done after changing the casing, so that only lower or capital letters have
     * to be provided for validation. In case that site_casing() is set to `SiteCasing::kUnchanged`:
     * All chars that are to be considered valid have to be provided for validation.
     *
     * See `nucleic_acid_codes...()` and `amino_acid_codes...()` functions for presettings of chars
     * that can be used for validation here.
     */
    FastqReader& valid_chars( std::string const& chars );

    /**
     * @brief Return the currently set chars used for validating Sequence sites.
     *
     * An empty string means that no validation is done.
     */
    std::string  valid_chars() const;

    /**
     * @brief Return the internal CharLookup that is used for validating the Sequence sites.
     *
     * This function is provided in case direct access to the lookup is needed. Usually, the
     * valid_chars() function should suffice. See there for details.
     */
    utils::CharLookup<bool>& valid_char_lookup();

    /**
     * @brief Set the QualityEncoding used for decoding the quality score line of the Fastq file.
     *
     * By default, we use Sanger encoding. This can be changed here.
     */
    FastqReader& quality_encoding( QualityEncoding encoding );

    /**
     * @brief Return the currently set QualityEncoding that is used for decoding the quality score
     * line of the Fastq file.
     */
    QualityEncoding quality_encoding();

    /**
     * @brief Functional that can be set to process the quality string found in fastq files.
     *
     * See the class description for details.
     */
    FastqReader& quality_string_plugin( quality_string_function const& plugin );

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    SiteCasing              site_casing_      = SiteCasing::kUnchanged;
    bool                    use_validation_   = false;
    utils::CharLookup<bool> lookup_;

    QualityEncoding         quality_encoding_ = QualityEncoding::kSanger;

    // Functional that can be set to process the quality string found in fastq files.
    quality_string_function quality_string_plugin_ = [&](
        std::string const& quality_string, Sequence& sequence
    ){
        sequence.phred_scores( quality_decode_to_phred_score( quality_string, quality_encoding_ ));
    };

    // Internal reading buffer
    mutable std::string buffer_;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
