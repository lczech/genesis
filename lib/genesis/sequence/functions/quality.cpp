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

#include "genesis/sequence/formats/fastq_reader.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <stdexcept>

#if defined(GENESIS_AVX) || defined(GENESIS_AVX2) || defined(GENESIS_AVX512)

    #include <immintrin.h>

#endif

namespace genesis {
namespace sequence {

// =================================================================================================
//     Phred Score To Error Probability
// =================================================================================================

// Local array for faster lookup of phred score error probabilities.
// Code to generate the array:
//
//     #include <iostream>
//     std::cout.precision( 15 );
//     size_t c = 0;
//     for( size_t i = 0; i < 256; ++i ) {
//         if( ++c == 4 || i % 10 == 0 ) {
//             c = 0;
//             std::cout << "\n    ";
//         }
//         if( i % 10 == 0 ) {
//             std::cout << "/* " << i << " */ ";
//         }
//         std::cout << std::pow( 10.0, static_cast<double>( i ) / -10.0 ) << ", ";
//     }
//     std::cout << "\n";
//
// See phred_score_to_error_probability() for usage. We probably never ever need the values above 93,
// as those cannot even be encoded in normal ASCII-based phred score codes, and also probably
// don't even need values above 60, which is usually taken as an upper bound. But let's be through
// here, and include the whole range of possible values for our data type.
static const std::array<double, 256> phred_score_to_error_probability_lookup_ = {{
    /* 0 */ 1.0, 0.794328234724281, 0.630957344480193, 0.501187233627272,
    0.398107170553497, 0.316227766016838, 0.251188643150958, 0.199526231496888,
    0.158489319246111, 0.125892541179417,
    /* 10 */ 0.1, 0.0794328234724281, 0.0630957344480193, 0.0501187233627272,
    0.0398107170553497, 0.0316227766016838, 0.0251188643150958, 0.0199526231496888,
    0.0158489319246111, 0.0125892541179417,
    /* 20 */ 0.01, 0.00794328234724281, 0.00630957344480193, 0.00501187233627272,
    0.00398107170553497, 0.00316227766016838, 0.00251188643150958, 0.00199526231496888,
    0.00158489319246111, 0.00125892541179417,
    /* 30 */ 0.001, 0.000794328234724281, 0.000630957344480193, 0.000501187233627273,
    0.000398107170553497, 0.000316227766016838, 0.000251188643150958, 0.000199526231496888,
    0.000158489319246111, 0.000125892541179417,
    /* 40 */ 0.0001, 7.94328234724282e-05, 6.30957344480193e-05, 5.01187233627273e-05,
    3.98107170553497e-05, 3.16227766016838e-05, 2.51188643150958e-05, 1.99526231496888e-05,
    1.58489319246111e-05, 1.25892541179417e-05,
    /* 50 */ 1e-05, 7.94328234724282e-06, 6.30957344480193e-06, 5.01187233627272e-06,
    3.98107170553497e-06, 3.16227766016838e-06, 2.51188643150958e-06, 1.99526231496888e-06,
    1.58489319246111e-06, 1.25892541179417e-06,
    /* 60 */ 1e-06, 7.94328234724282e-07, 6.30957344480193e-07, 5.01187233627272e-07,
    3.98107170553497e-07, 3.16227766016838e-07, 2.51188643150958e-07, 1.99526231496888e-07,
    1.58489319246111e-07, 1.25892541179417e-07,
    /* 70 */ 1e-07, 7.94328234724282e-08, 6.30957344480193e-08, 5.01187233627272e-08,
    3.98107170553497e-08, 3.16227766016838e-08, 2.51188643150958e-08, 1.99526231496888e-08,
    1.58489319246111e-08, 1.25892541179417e-08,
    /* 80 */ 1e-08, 7.94328234724282e-09, 6.30957344480194e-09, 5.01187233627271e-09,
    3.98107170553497e-09, 3.16227766016838e-09, 2.51188643150958e-09, 1.99526231496888e-09,
    1.58489319246111e-09, 1.25892541179417e-09,
    /* 90 */ 1e-09, 7.94328234724282e-10, 6.30957344480194e-10, 5.01187233627271e-10,
    3.98107170553497e-10, 3.16227766016838e-10, 2.51188643150958e-10, 1.99526231496888e-10,
    1.58489319246111e-10, 1.25892541179417e-10,
    /* 100 */ 1e-10, 7.94328234724282e-11, 6.30957344480194e-11, 5.01187233627271e-11,
    3.98107170553497e-11, 3.16227766016838e-11, 2.51188643150958e-11, 1.99526231496888e-11,
    1.58489319246111e-11, 1.25892541179417e-11,
    /* 110 */ 1e-11, 7.94328234724282e-12, 6.30957344480194e-12, 5.01187233627271e-12,
    3.98107170553497e-12, 3.16227766016838e-12, 2.51188643150958e-12, 1.99526231496888e-12,
    1.58489319246111e-12, 1.25892541179417e-12,
    /* 120 */ 1e-12, 7.94328234724282e-13, 6.30957344480194e-13, 5.01187233627271e-13,
    3.98107170553497e-13, 3.16227766016838e-13, 2.51188643150958e-13, 1.99526231496888e-13,
    1.58489319246111e-13, 1.25892541179417e-13,
    /* 130 */ 1e-13, 7.94328234724282e-14, 6.30957344480194e-14, 5.01187233627271e-14,
    3.98107170553497e-14, 3.16227766016838e-14, 2.51188643150958e-14, 1.99526231496888e-14,
    1.58489319246111e-14, 1.25892541179417e-14,
    /* 140 */ 1e-14, 7.94328234724282e-15, 6.30957344480194e-15, 5.01187233627271e-15,
    3.98107170553497e-15, 3.16227766016838e-15, 2.51188643150958e-15, 1.99526231496888e-15,
    1.58489319246111e-15, 1.25892541179417e-15,
    /* 150 */ 1e-15, 7.94328234724282e-16, 6.30957344480194e-16, 5.01187233627271e-16,
    3.98107170553497e-16, 3.16227766016838e-16, 2.51188643150958e-16, 1.99526231496888e-16,
    1.58489319246111e-16, 1.25892541179417e-16,
    /* 160 */ 1e-16, 7.94328234724279e-17, 6.30957344480194e-17, 5.01187233627271e-17,
    3.98107170553499e-17, 3.16227766016838e-17, 2.51188643150957e-17, 1.99526231496888e-17,
    1.58489319246111e-17, 1.25892541179417e-17,
    /* 170 */ 1e-17, 7.94328234724279e-18, 6.30957344480194e-18, 5.01187233627271e-18,
    3.98107170553499e-18, 3.16227766016838e-18, 2.51188643150957e-18, 1.99526231496888e-18,
    1.58489319246111e-18, 1.25892541179417e-18,
    /* 180 */ 1e-18, 7.94328234724279e-19, 6.30957344480194e-19, 5.01187233627271e-19,
    3.98107170553499e-19, 3.16227766016838e-19, 2.51188643150957e-19, 1.99526231496888e-19,
    1.58489319246111e-19, 1.25892541179417e-19,
    /* 190 */ 1e-19, 7.94328234724279e-20, 6.30957344480194e-20, 5.01187233627271e-20,
    3.98107170553499e-20, 3.16227766016838e-20, 2.51188643150957e-20, 1.99526231496888e-20,
    1.58489319246111e-20, 1.25892541179417e-20,
    /* 200 */ 1e-20, 7.94328234724279e-21, 6.30957344480194e-21, 5.01187233627271e-21,
    3.98107170553499e-21, 3.16227766016838e-21, 2.51188643150957e-21, 1.99526231496888e-21,
    1.58489319246111e-21, 1.25892541179417e-21,
    /* 210 */ 1e-21, 7.94328234724279e-22, 6.30957344480194e-22, 5.01187233627272e-22,
    3.98107170553499e-22, 3.16227766016838e-22, 2.51188643150957e-22, 1.99526231496888e-22,
    1.58489319246111e-22, 1.25892541179417e-22,
    /* 220 */ 1e-22, 7.94328234724279e-23, 6.30957344480194e-23, 5.01187233627271e-23,
    3.98107170553499e-23, 3.16227766016838e-23, 2.51188643150957e-23, 1.99526231496888e-23,
    1.58489319246111e-23, 1.25892541179417e-23,
    /* 230 */ 1e-23, 7.94328234724279e-24, 6.30957344480194e-24, 5.01187233627271e-24,
    3.98107170553499e-24, 3.16227766016838e-24, 2.51188643150957e-24, 1.99526231496888e-24,
    1.58489319246111e-24, 1.25892541179417e-24,
    /* 240 */ 1e-24, 7.94328234724279e-25, 6.30957344480194e-25, 5.01187233627272e-25,
    3.98107170553499e-25, 3.16227766016838e-25, 2.51188643150957e-25, 1.99526231496888e-25,
    1.58489319246111e-25, 1.25892541179417e-25,
    /* 250 */ 1e-25, 7.94328234724279e-26, 6.30957344480194e-26, 5.01187233627271e-26,
    3.98107170553499e-26, 3.16227766016838e-26
}};

// =================================================================================================
//     Quality Encoding and Decoding
// =================================================================================================

/**
 * @brief Local helper function to throw if any invalid fastq quality chars are being used.
 */
inline void throw_invalid_quality_code_( char quality_code, QualityEncoding encoding )
{
    throw std::invalid_argument(
        "Invalid quality code: " + utils::char_to_hex( quality_code ) +
        " is not in the valid range for " + quality_encoding_name( encoding ) + " quality codes."
    );
}

std::string quality_encoding_name( QualityEncoding encoding, bool with_offset )
{
    auto const wo = with_offset;
    switch( encoding ) {
        case QualityEncoding::kSanger:
            return std::string( "Sanger" )        + std::string( wo ? " (ASCII offset 33)" : "" );
        case QualityEncoding::kIllumina13:
            return std::string( "Illumina 1.3+" ) + std::string( wo ? " (ASCII offset 64)" : "" );
        case QualityEncoding::kIllumina15:
            return std::string( "Illumina 1.5+" ) + std::string( wo ? " (ASCII offset 64)" : "" );
        case QualityEncoding::kIllumina18:
            return std::string( "Illumina 1.8+" ) + std::string( wo ? " (ASCII offset 33)" : "" );
        case QualityEncoding::kSolexa:
            return std::string( "Solexa" )        + std::string( wo ? " (ASCII offset 64)" : "" );

        default:
            throw std::invalid_argument( "Invalid quality encoding type." );
    };
}

QualityEncoding guess_quality_encoding_from_name( std::string const& name )
{
    // Transform the string into a uniform form.
    auto s = utils::to_lower( name );
    s.erase( std::remove_if(s.begin(), s.end(), []( char c ){
        return ! utils::is_alnum( c );
    }), s.end() );

    // Match the enum.
    if( s == "sanger" ) {
        return QualityEncoding::kSanger;
    } else if( s == "illumina13" ) {
        return QualityEncoding::kIllumina13;
    } else if( s == "illumina15" ) {
        return QualityEncoding::kIllumina15;
    } else if( s == "illumina18" || s == "illumina" ) {
        return QualityEncoding::kIllumina18;
    } else if( s == "solexa" ) {
        return QualityEncoding::kSolexa;
    } else {
        throw std::runtime_error( "Invalid quality encoding name: \"" + name + "\""  );
    }
}

unsigned char quality_decode_to_phred_score( char quality_code, QualityEncoding encoding )
{
    // Convert using an offset. It's as simple as that.
    // Except that we have different offsets for different fastq encoding styles.
    // And also, Solexa needs special treatment, as we internally use phred scores only.
    // Basically, fastq is again one of those weird bioinformatics file formats that drives
    // everyone crazy by being ill-defined, and having contraticting variants and conventions...
    // NB: We do not check for upper bounds of the scores here, as higher-quality scores can come
    // from downstream processing.
    switch( encoding ) {
        case QualityEncoding::kSanger:
        case QualityEncoding::kIllumina18:
            if( quality_code < 33 || quality_code >= 127 ) {
                throw_invalid_quality_code_( quality_code, encoding );
            }
            return static_cast<unsigned char>( quality_code ) - 33;

        case QualityEncoding::kIllumina13:
        case QualityEncoding::kIllumina15:
            if( quality_code < 64 || quality_code >= 127 ) {
                throw_invalid_quality_code_( quality_code, encoding );
            }
            return static_cast<unsigned char>( quality_code ) - 64;

        case QualityEncoding::kSolexa:
            if( quality_code < 59 || quality_code >= 127 ) {
                throw_invalid_quality_code_( quality_code, encoding );
            }
            return solexa_score_to_phred_score( static_cast<signed char>( quality_code ) - 64 );

        default:
            throw std::invalid_argument( "Invalid quality encoding type." );
    };
}

std::vector<unsigned char> quality_decode_to_phred_score(
    std::string const& quality_codes,
    QualityEncoding encoding
) {
    // Reserve space for the result. Do this first, to facilitate copy elision.
    auto result = std::vector<unsigned char>( quality_codes.size() );

    // Only switch on the encoding once, for speed. We use a fake offset for Solexa,
    // as they can go in the negative range to -5. Doing it this way makes our error checking
    // code consistent. We correct for this later in the Solxa conversion loop below.
    unsigned char offset;
    switch( encoding ) {
        case QualityEncoding::kSanger:
        case QualityEncoding::kIllumina18:
            offset = 33;
            break;

        case QualityEncoding::kIllumina13:
        case QualityEncoding::kIllumina15:
            offset = 64;
            break;

        case QualityEncoding::kSolexa:
            offset = 59;
            break;

        default:
            throw std::invalid_argument( "Invalid quality encoding type." );
    };

    // We use this as an indicator whether we found an error in the sequence.
    // This allows to run the inner loops without branching.
    bool good = true;

    // We use a loop variable here that is shared between the AVX and normal version,
    // so that we do not have to duplicate the loop.
    size_t i = 0;

    // In the loops below, we convert Solexa as if it was phred, and fix this later.
    // This avoids code duplication for the error checking.

    #ifdef GENESIS_AVX2

        // Get the data in the right format
        auto const* data = reinterpret_cast<__m256i const*>( quality_codes.c_str() );
        auto* write = reinterpret_cast<__m256i*>( result.data() );

        // Define some masks. We use 32 byte full of the offset, and full of the upper limit for
        // ascii-encoded phred scores.
        auto const m_offset = _mm256_set1_epi8( offset );
        auto static const m_upper = _mm256_set1_epi8( 126 );

        // Process in 32-byte chunks. The loop condition uses integer division / 32,
        // so we miss the rest that is above a multiple of 32. We process this later.
        for( size_t j = 0; j < quality_codes.size() / 32; ++j ) {

            // Load the data and compare it to the offset and upper limit.
            auto const m_data = _mm256_loadu_si256( data + j );
            auto const m_min = _mm256_cmpgt_epi8( m_offset, m_data );
            auto const m_max = _mm256_cmpgt_epi8( m_data, m_upper );

            // If any char is below the offset or above the upper limit, we have an error.
            // We just store the result of the test, to avoid branching.
            good &= _mm256_testz_si256( m_min, m_min ) && _mm256_testz_si256( m_max, m_max );

            // Subtract the offset, and store the result.
            auto const m_result = _mm256_sub_epi8( m_data, m_offset );
            _mm256_storeu_si256( write + j, m_result );
        }

        // Set i to what it needs to be so that the loop below processes the remaining chars
        // that are left in the string after AVX is done with the 32 byte chunks.
        i = quality_codes.size() / 32 * 32;

    #endif // GENESIS_AVX2

    // Run the conversion. Again, we avoid branching in the inner loop.
    // i is already initialized, either from before the AVX code, or from within the AVX code.
    // This avoids duplicating the below loop for the rest of the chars that are in the remainder
    // of the string after AVX has processed all 32 byte chunks.
    for( ; i < quality_codes.size(); ++i ) {
        good &= ( quality_codes[i] >= offset ) && ( quality_codes[i] <= 126 );
        result[i] = static_cast<unsigned char>( quality_codes[i] ) - offset;
    }

    // If we found an error in the sequence, throw an exception. We run the whole sequence again,
    // to get the exact char that is bad. This is only in the error case, so we do not care about speed.
    if( !good ) {
        for( size_t i = 0; i < quality_codes.size(); ++i ) {
            if( quality_codes[i] < offset || quality_codes[i] >= 127 ) {
                throw_invalid_quality_code_( quality_codes[i], encoding );
            }
        }

        // We cannot get here, otherwise, the !good condition would not have led us here.
        assert( false );
    }

    // Use 64bit words instead of vectorization. Maybe later, as an alternative to AVX.
    // Macros from https://graphics.stanford.edu/~seander/bithacks.html#HasLessInWord
    //
    // #define hasless(x,n) (((x)-~static_cast<uint64_t>(0)/255U*(n))&~(x)&~static_cast<uint64_t>(0)/255U*128)
    // #define hasmore(x,n) (((x)+~static_cast<uint64_t>(0)/255U*(127-(n))|(x))&~static_cast<uint64_t>(0)/255U*128)
    //
    // auto const subtrahend = ~static_cast<uint64_t>(0) / 255U * offset;
    //
    // auto const* data = reinterpret_cast<uint64_t const*>( quality_codes.c_str() );
    // auto* write = reinterpret_cast<uint64_t*>( result.data() );
    // bool failed = false;
    //
    // for( size_t i = 0; i < quality_codes.size() / 8; ++i ) {
    //     bool const l = hasless( data[i], offset );
    //     bool const m = hasmore( data[i], 126 );
    //     failed |= l | m;
    //
    //     write[i] = data[i] - subtrahend;
    // }
    //
    // #undef hasless
    // #undef hasmore
    //
    // if( failed ) {
    //     throw std::invalid_argument(
    //         "Invalid quality code that is not in the valid range for " +
    //         quality_encoding_name( encoding ) + " quality codes found in sequence."
    //     );
    // }

    // For Solexa, we iterate the sequence again in order to convert it to phred.
    // This is slower and could be avoided with a bit of code duplication, but no one uses that
    // format anyway any more, so that case should be rare.
    if( encoding == QualityEncoding::kSolexa ) {
        for( size_t i = 0; i < quality_codes.size(); ++i ) {
            result[i] = solexa_score_to_phred_score( result[i] - 5 );
        }
    }

    // Finally, we are done.
    return result;
}

// =================================================================================================
//     Guess Quality Encoding Type
// =================================================================================================

bool compatible_quality_encodings( QualityEncoding lhs, QualityEncoding rhs )
{
    // Check rhs.
    if(
        rhs != QualityEncoding::kSanger     &&
        rhs != QualityEncoding::kIllumina13 &&
        rhs != QualityEncoding::kIllumina15 &&
        rhs != QualityEncoding::kIllumina18 &&
        rhs != QualityEncoding::kSolexa
    ) {
        throw std::invalid_argument( "Invalid quality encoding type." );
    }

    // Check lhs, and return compatibility.
    switch( lhs ) {
        case QualityEncoding::kSanger:
        case QualityEncoding::kIllumina18:
            return rhs == QualityEncoding::kSanger || rhs ==  QualityEncoding::kIllumina18;

        case QualityEncoding::kIllumina13:
        case QualityEncoding::kIllumina15:
            return rhs == QualityEncoding::kIllumina13 || rhs ==  QualityEncoding::kIllumina15;

        case QualityEncoding::kSolexa:
            return rhs == QualityEncoding::kSolexa;

        default:
            throw std::invalid_argument( "Invalid quality encoding type." );
    };
    return false;
}

QualityEncoding guess_quality_encoding( std::array<size_t, 128> const& char_counts )
{
    // Find the first entry that is not 0
    size_t min = 0;
    for( size_t i = 0; i < char_counts.size(); ++i ) {
        if( char_counts[i] > 0 ) {
            min = i;
            break;
        }
    }

    // Find the last entry that is not 0
    size_t max = 128;
    for( size_t i = char_counts.size(); i > 0; --i ) {
        if( char_counts[i-1] > 0 ) {
            max = i-1;
            break;
        }
    }

    // Some checks.
    if( min == char_counts.size() ) {
        assert( max == 0 );
        throw std::runtime_error(
            "Cannot guess quality encoding, as all quality code counts are zero."
        );
    }
    if( min < 33 || max >= 127 ) {
        throw std::runtime_error(
            "Invalid char counts provided to guess quality score encoding. Only printable "
            "characters (ASCII range 33 to 127) are allowed in fastq quality encodings."
        );
    }
    assert( min <= max );

    // Sanger and Illumina 1.8 use an offset of 33. The next higher offset is 64, but with Solexa ranging into the negative until -5, we find that anything below 64-5=59 cannot have the 64 offset, and hence must have the 33 offset.
    if( min < 59 ) {
        // Sanger and Illumina 1.8 are basically the same, so it does not make a difference
        // whether we detect them correctly or not. However, we can still try to guess, for
        // completeness. Illumina 1.8 seems to have one more character that can be used. Thus, if
        // this character occurs, we can be sure. If not, it might just be that no base was that
        // accurate. But then, it doesn't really matter anyway.
        if( max > 73 ) {
            return QualityEncoding::kIllumina18;
        } else {
            return QualityEncoding::kSanger;
        }
    }

    // Solexa goes down to a score of -5, with an offset of 64 for 0, so anything below 64 is
    // negative, meaning that it cannot be Illumina 1.3 or 1.5.
    if( min < 64 ) {
        return QualityEncoding::kSolexa;
    }

    // At this point, we could use a heuristic to test how common 'B' is, which is special in Illumina 1.5,
    // see https://github.com/brentp/bio-playground/blob/master/reads-utils/guess-encoding.py for
    // details. This would enable more fine-grained distinction between Illumina 1.3 and 1.5.
    // But for now, we simply assume that an encoding without anything before 'B' is Illumina 1.5
    if( min < 66 ) {
        return QualityEncoding::kIllumina13;
    }
    return QualityEncoding::kIllumina15;
}

QualityEncoding guess_fastq_quality_encoding(
    std::shared_ptr< utils::BaseInputSource > source,
    size_t max_lines,
    size_t max_chars
) {
    // Init a counting array for each char, use value-initialization to 0
    auto char_counts = std::array<size_t, 128>();
    size_t lines_cnt = 0;
    size_t chars_cnt = 0;

    // Prepare a reader that simply increments all char counts for the quality chars
    // that are found in the sequences.
    auto reader = FastqReader();
    reader.quality_string_plugin(
        [&]( std::string const& quality_string, Sequence& sequence )
        {
            (void) sequence;
            for( auto q : quality_string ) {

                // Cast, so that we catch the issue that char can be signed or unsigned,
                // depending on the compiler. This might cause unnecessary warnings otherwise.
                auto qu = static_cast<unsigned char>(q);
                if( qu > 127 ) {
                    throw std::invalid_argument(
                        "Invalid quality score character outside of the ASCII range."
                    );
                }

                // Increment the count for the given character.
                // assert( (qu & ~0x7f) == 0 );
                assert( qu < char_counts.size() );
                ++char_counts[ qu ];

                // Only count up to a limit of chars, if provided.
                ++chars_cnt;
                if( max_chars > 0 && chars_cnt > max_chars ) {
                    break;
                }
            }
        }
    );

    // Read the input, sequence by sequence.
    utils::InputStream input_stream( source );
    Sequence seq;
    while( reader.parse_sequence( input_stream, seq ) ) {
        // Here, we check the limits, if used.
        if( max_chars > 0 && chars_cnt > max_chars ) {
            break;
        }

        // For lines, we need to stop once we have reached the limit, not one after,
        // as the counting is dont in the preprocessing of the reader iterator already.
        ++lines_cnt;
        if( max_lines > 0 && lines_cnt >= max_lines ) {
            break;
        }
    }

    // Return our guess based on the quality characters that were found in the sequences.
    return guess_quality_encoding( char_counts );
}

// =================================================================================================
//     Quality Computations
// =================================================================================================

unsigned char error_probability_to_phred_score( double error_probability )
{
    if( ! std::isfinite( error_probability ) || error_probability < 0.0 || error_probability > 1.0 ) {
        throw std::invalid_argument(
            "Cannot convert error probability outside of range [0.0, 1.0] to phred score."
        );
    }

    // Compute the value and put into the valid range for unsigned chars. This might exceed
    // the encoding that is later used to store the scores in fastq, but this does not concern us
    // here. Instead, we offer the full range here, and clamp later to the value range when encoding.
    double v = std::round( -10.0 * std::log10( error_probability ));
    v = std::min( v, 255.0 );
    return static_cast<unsigned char>( v );
}

double phred_score_to_error_probability( unsigned char phred_score )
{
    return phred_score_to_error_probability_lookup_[ phred_score ];
    // return std::pow( 10.0, static_cast<double>( phred_score ) / -10.0 );
}

signed char error_probability_to_solexa_score( double error_probability )
{
    if( ! std::isfinite( error_probability ) || error_probability < 0.0 || error_probability > 1.0 ) {
        throw std::invalid_argument(
            "Cannot convert error probability outside of range [0.0, 1.0] to solexa score."
        );
    }

    // The following are the limits than can be encoded in typical fastq-solexa encoding.
    // We are not using them here, but instead use more relaxed limts, and will apply the actual
    // limits only when encoding to fastq.
    // // min that can be encoded in fastq with solexa encoding
    // if( error_probability < 6.30957344e-7 ) {
    //     return 62;
    // }
    // // max that can be encoded in fastq with solexa encoding
    // if( error_probability > 0.75 ) {
    //     return -5;
    // }

    // Compute the score, and check its boundaries. We did a check before, so this is duplication,
    // but leads to nicer user output.
    double v = std::round( -10.0 * std::log10( error_probability / ( 1.0 - error_probability )));
    v = std::min( v, 127.0 );
    v = std::max( v, -128.0 );
    return static_cast<signed char>( v );
}

double solexa_score_to_error_probability( signed char solexa_score )
{
    if( solexa_score < -5 ) {
        solexa_score = -5;
    }
    double const t = std::pow( 10.0, static_cast<double>( solexa_score ) / -10.0 );
    return t / ( 1.0 + t );
}

signed char phred_score_to_solexa_score( unsigned char phred_score )
{
    if( phred_score <= 1 ) {
        return -5;
    }
    double v = std::round(
        10.0 * std::log10( std::pow( 10.0, static_cast<double>( phred_score ) / 10.0 ) - 1.0 )
    );
    v = std::min( v, 127.0 );
    return static_cast<signed char>( v );
}

unsigned char solexa_score_to_phred_score( signed char solexa_score )
{
    // This cannot overflow, so no need to check.
    return static_cast<unsigned char>( std::round(
        10.0 * std::log10( std::pow( 10.0, static_cast<double>( solexa_score ) / 10.0 ) + 1.0 )
    ));
}

std::vector<unsigned char> error_probability_to_phred_score( std::vector<double> error_probability )
{
    auto res = std::vector<unsigned char>( error_probability.size(), 0 );
    for( size_t i = 0; i < error_probability.size(); ++i ) {
        res[i] = error_probability_to_phred_score( error_probability[i] );
    }
    return res;
}

std::vector<double> phred_score_to_error_probability( std::vector<unsigned char> phred_score )
{
    auto res = std::vector<double>( phred_score.size(), 0 );
    for( size_t i = 0; i < phred_score.size(); ++i ) {
        res[i] = phred_score_to_error_probability( phred_score[i] );
    }
    return res;
}

std::vector<signed char> error_probability_to_solexa_score( std::vector<double> error_probability )
{
    auto res = std::vector<signed char>( error_probability.size(), 0 );
    for( size_t i = 0; i < error_probability.size(); ++i ) {
        res[i] = error_probability_to_solexa_score( error_probability[i] );
    }
    return res;
}

std::vector<double> solexa_score_to_error_probability( std::vector<signed char> solexa_score )
{
    auto res = std::vector<double>( solexa_score.size(), 0 );
    for( size_t i = 0; i < solexa_score.size(); ++i ) {
        res[i] = solexa_score_to_error_probability( solexa_score[i] );
    }
    return res;
}

std::vector<signed char> phred_score_to_solexa_score( std::vector<unsigned char> phred_score )
{
    auto res = std::vector<signed char>( phred_score.size(), 0 );
    for( size_t i = 0; i < phred_score.size(); ++i ) {
        res[i] = phred_score_to_solexa_score( phred_score[i] );
    }
    return res;
}

std::vector<unsigned char> solexa_score_to_phred_score( std::vector<signed char> solexa_score )
{
    auto res = std::vector<unsigned char>( solexa_score.size(), 0 );
    for( size_t i = 0; i < solexa_score.size(); ++i ) {
        res[i] = solexa_score_to_phred_score( solexa_score[i] );
    }
    return res;
}

} // namespace sequence
} // namespace genesis
