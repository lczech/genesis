#ifndef GENESIS_SEQUENCE_FUNCTIONS_QUALITY_H_
#define GENESIS_SEQUENCE_FUNCTIONS_QUALITY_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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

#include <array>
#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward declarations
// =================================================================================================

namespace utils {
    class BaseInputSource;
}

namespace sequence {

// =================================================================================================
//     Quality Encoding and Decoding
// =================================================================================================

/**
 * @brief List of quality encodings for which we support decoding.
 *
 * We offer the following quality score encodings:
 *
 *  *  Sanger, with offset 33.
 *  *  Illumina 1.3+, with offset 64.
 *  *  Illumina 1.5+, with offset 64.
 *  *  Illumina 1.8+, with offset 33.
 *  *  Solexa, with offset 64, and a special encoding equation.
 *
 * These are the types of encodings used in fastq files over the years.
 * It seems that Sanger / Illumina 1.8+ is the most commonly used one today,
 * so this is also what we use as a default.
 */
enum class QualityEncoding
{
    kSanger,
    kSolexa,
    kIllumina13,
    kIllumina15,
    kIllumina18
};

/**
 * @brief Return a readable name for each of the encoding types.
 *
 * See QualityEncoding for the names being used here.
 */
std::string quality_encoding_name( QualityEncoding encoding );

/**
 * @brief Guess the QualityEncoding type, given its description name.
 *
 * This is the reverse of quality_encoding_name(), but additionally allows the given @p name to be
 * fuzzy. The @p name is stripped off all non-alphanumerical characters and made lower-case.
 * The remainder (e.g., `illumina13`) then is matched to the enum QualityEncoding, with the
 * additional option that just `illumina` (without any version number) is matched to
 * QualityEncoding::kIllumina18.
 */
QualityEncoding guess_quality_encoding_from_name( std::string const& name );

/**
 * @brief Decode a single quality score char (for example coming from a fastq file) to a phred score.
 *
 * The function allows to use different types of quality encoding as used by different sequencing
 * platforms/technologies. This format confusion is messy, see the FastqReader class for details.
 *
 * Note that Sanger as well as the Illumina encodings are simply encoded as phred plus ASCII offset,
 * while Solexa uses a formula based on odds instead of probability. Hence, when specifying Solexa
 * here, we internally convert to phred before returning the result here.
 */
unsigned char quality_decode_to_phred_score(
    char quality_code,
    QualityEncoding encoding = QualityEncoding::kSanger
);

/**
 * @brief Decode a string of quality scores (for example coming from a fastq file) to phred scores.
 *
 * @copydoc quality_decode_to_phred_score( char quality_code, QualityEncoding encoding )
 */
std::vector<unsigned char> quality_decode_to_phred_score(
    std::string const& quality_codes,
    QualityEncoding encoding = QualityEncoding::kSanger
);

/**
 * @brief Encode a phred score into a quality char, using the Sanger convention.
 *
 * This function takes a @p phred_score in the range 0 to 93, and encodes it, for example for
 * usage in a fastq file, by adding the ASCII offset 33 to it.
 *
 * While we can decode from numerous formats, see quality_decode_to_phred_score(), we only
 * support encoding back to the Sanger format, because we want to minimize confusion and
 * maximize compatability with other programs. Also, Sanger is used by the NCBI Short Read Archive
 * and Illumina 1.8+, and hence the most common format as of today.
 *
 * If the flag @p clamp is set (default), values outside of the valid range 0 to 93 are clamped,
 * that is, set to be inside the valid range. As the phred score is unsigned, this leads to values
 * above 93 simply being encoded as if they were exactly 93. If @p clamp is set to false,
 * an exception is thrown instead if a value above 93 is encountered.
 */
inline char quality_encode_from_phred_score( unsigned char phred_score, bool clamp = true )
{
    // Only do one branch here, as this should be rare case, and then test and branch again inside.
    if( phred_score > 93 ) {
        if( clamp ) {
            phred_score = 93;
            // phred_score = std::min( phred_score, static_cast<unsigned char>(93) );
        } else {
            throw std::invalid_argument(
                "Cannot encode phred score outside of [0, 93] to Sanger format."
            );
        }
    }
    return static_cast<char>( phred_score + 33 );
}

/**
 * @brief Encode phred scores into quality chars, using the Sanger convention.
 *
 * @copydoc quality_encode_from_phred_score( unsigned char, bool )
 */
inline std::string quality_encode_from_phred_score(
    std::vector<unsigned char> const& phred_scores,
    bool clamp = true
) {
    auto qualities = std::string( phred_scores.size(), ' ' );
    for( size_t i = 0; i < phred_scores.size(); ++i ) {
        qualities[i] = quality_encode_from_phred_score( phred_scores[i], clamp );
    }
    return qualities;
}

// =================================================================================================
//     Guess Quality Encoding Type
// =================================================================================================

/**
 * @brief Guess the quality score encoding, based on counts of how often each char appeared
 * in the quality string (of a fastq file for example).
 *
 * The @p char_counts needs to be filled with counts of how often each quality code char appeared
 * in the (fastq) quality strings. If any values outside of the printable character range (ASCII 33
 * to 127) are non-zero in the @p char_counts, the function throws, as these are invaliv qualiy
 * encodings. Otherwise, it guesses which QualityEncoding was used for the data, based on
 * which chars appear in it.
 */
QualityEncoding guess_quality_encoding( std::array<size_t, 128> const& char_counts );

/**
 * @brief Guess the quality score encoding for a fastq input, based on counts of how often each
 * char appeared in the quality string (of the input fastq file for example).
 *
 * The function reads and parses the input source as a fastq file, counts all quality score chars
 * as they appear in there, and then guesses the encoding that was used.
 */
QualityEncoding guess_fastq_quality_encoding( std::shared_ptr< utils::BaseInputSource > source );

// =================================================================================================
//     Quality Computations
// =================================================================================================

unsigned char error_probability_to_phred_score( double error_probability );
std::vector<unsigned char> error_probability_to_phred_score( std::vector<double> error_probability );

double phred_score_to_error_probability( unsigned char phred_score );
std::vector<double> phred_score_to_error_probability( std::vector<unsigned char> phred_score );

signed char error_probability_to_solexa_score( double error_probability );
std::vector<signed char> error_probability_to_solexa_score( std::vector<double> error_probability );

double solexa_score_to_error_probability( signed char solexa_score );
std::vector<double> solexa_score_to_error_probability( std::vector<signed char> solexa_score );

signed char phred_score_to_solexa_score( unsigned char phred_score );
std::vector<signed char> phred_score_to_solexa_score( std::vector<unsigned char> phred_score );

unsigned char solexa_score_to_phred_score( signed char solexa_score );
std::vector<unsigned char> solexa_score_to_phred_score( std::vector<signed char> solexa_score );

} // namespace sequence
} // namespace genesis

#endif // include guard
