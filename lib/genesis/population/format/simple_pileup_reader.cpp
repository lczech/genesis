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
 * @ingroup population
 */

#include "genesis/population/format/simple_pileup_reader.hpp"

#include "genesis/population/function/functions.hpp"
#include "genesis/sequence/functions/codes.hpp"
#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/math/bitvector/helper.hpp"
#include "genesis/utils/text/char.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Reading Records
// =================================================================================================

std::vector<SimplePileupReader::Record> SimplePileupReader::read_records(
    std::shared_ptr< utils::BaseInputSource > source
) const {
    std::vector<SimplePileupReader::Record> result;
    utils::InputStream it( source );

    // Reset quality code counts.
    quality_code_counts_ = std::array<size_t, 128>{};

    // Read until end of input, pushing copies into the result
    // (moving would not reduce the number of times that we need to allocate memory here).
    Record rec;
    while( parse_line_( it, rec, {}, false )) {
        result.push_back( rec );
    }
    return result;
}

// std::vector<SimplePileupReader::Record> SimplePileupReader::read_records(
//     std::shared_ptr< utils::BaseInputSource > source,
//     std::vector<size_t> const& sample_indices
// ) const {
//     std::vector<SimplePileupReader::Record> result;
//     utils::InputStream it( source );
//
//     // Convert the list of indices to a bool vec that tells which samples we want to process.
//     auto const sample_filter = utils::make_bool_vector_from_indices( sample_indices );
//
//     Record rec;
//     while( parse_line_( it, rec, sample_filter, true )) {
//         result.push_back( rec );
//     }
//     return result;
// }

std::vector<SimplePileupReader::Record> SimplePileupReader::read_records(
    std::shared_ptr< utils::BaseInputSource > source,
    std::vector<bool> const& sample_filter
) const {
    std::vector<SimplePileupReader::Record> result;
    utils::InputStream it( source );

    // Reset quality code counts.
    quality_code_counts_ = std::array<size_t, 128>{};

    // Read until end of input, pushing copies into the result
    // (moving would not reduce the number of times that we need to allocate memory here).
    Record rec;
    while( parse_line_( it, rec, sample_filter, true )) {
        result.push_back( rec );
    }
    return result;
}

// =================================================================================================
//     Reading Variants
// =================================================================================================

std::vector<Variant> SimplePileupReader::read_variants(
    std::shared_ptr< utils::BaseInputSource > source
) const {
    std::vector<Variant> result;
    utils::InputStream it( source );

    // Read until end of input, pushing copies into the result
    // (moving would not reduce the number of times that we need to allocate memory here).
    Variant var;
    while( parse_line_( it, var, {}, false )) {
        result.push_back( var );
    }
    return result;
}

// std::vector<Variant> SimplePileupReader::read_variants(
//     std::shared_ptr< utils::BaseInputSource > source,
//     std::vector<size_t> const& sample_indices
// ) const {
//     std::vector<Variant> result;
//     utils::InputStream it( source );
//
//     // Convert the list of indices to a bool vec that tells which samples we want to process.
//     auto const sample_filter = utils::make_bool_vector_from_indices( sample_indices );
//
//     Variant var;
//     while( parse_line_( it, var, sample_filter, true )) {
//         result.push_back( var );
//     }
//     return result;
// }

std::vector<Variant> SimplePileupReader::read_variants(
    std::shared_ptr< utils::BaseInputSource > source,
    std::vector<bool> const& sample_filter
) const {
    std::vector<Variant> result;
    utils::InputStream it( source );

    // Read until end of input, pushing copies into the result
    // (moving would not reduce the number of times that we need to allocate memory here).
    Variant var;
    while( parse_line_( it, var, sample_filter, true )) {
        result.push_back( var );
    }
    return result;
}

// =================================================================================================
//     Parsing Records
// =================================================================================================

bool SimplePileupReader::parse_line_record(
    utils::InputStream& input_stream,
    SimplePileupReader::Record& record
) const {
    return parse_line_( input_stream, record, {}, false );
}

bool SimplePileupReader::parse_line_record(
    utils::InputStream& input_stream,
    SimplePileupReader::Record& record,
    std::vector<bool> const& sample_filter
) const {
    return parse_line_( input_stream, record, sample_filter, true );
}

// =================================================================================================
//     Parsing Variants
// =================================================================================================

bool SimplePileupReader::parse_line_variant(
    utils::InputStream& input_stream,
    Variant& variant
) const {
    reset_status_( variant );
    return parse_line_( input_stream, variant, {}, false );
}

bool SimplePileupReader::parse_line_variant(
    utils::InputStream& input_stream,
    Variant& variant,
    std::vector<bool> const& sample_filter
) const {
    reset_status_( variant );
    return parse_line_( input_stream, variant, sample_filter, true );
}

// =================================================================================================
//     Internal Members
// =================================================================================================

// -------------------------------------------------------------------------
//     Reset Status
// -------------------------------------------------------------------------

void SimplePileupReader::reset_status_( Variant& variant ) const
{
    variant.status.reset();
    for( auto& sample : variant.samples ) {
        sample.status.reset();
    }
}

// -------------------------------------------------------------------------
//     Parse Line
// -------------------------------------------------------------------------

template<class T>
bool SimplePileupReader::parse_line_(
    utils::InputStream&      input_stream,
    T&                       target,
    std::vector<bool> const& sample_filter,
    bool                     use_sample_filter
) const {
    // Shorthand.
    auto& it = input_stream;

    // If we reached the end of the input stream, reset the target. We do not reset per default,
    // in order to avoid costly re-initialization of the sample vector. But when we finish with
    // an input stream, we want to reset, so that subsequent usage of this reader class does not
    // fail if the pileup file contains a different number of samples.
    // Still, the user will currently get an error when using the same reader instance to
    // simultaneously (interlaced) read from multiple pileup files with differing number of samples
    // into the same target... But who does that?! If you are a user having this issue, please
    // let me know!
    if( !it ) {
        target = T();
        return false;
    }
    assert( it );
    if( *it == '\n' ) {
        throw std::runtime_error(
            "Malformed pileup " + it.source_name() + " at " + it.at() + ": Invalid empty line"
        );
    }

    // Read chromosome.
    utils::affirm_char_or_throw( it, utils::is_graph );
    target.chromosome = utils::read_while( it, utils::is_graph );
    if( target.chromosome.empty() ) {
        throw std::runtime_error(
            "Malformed pileup " + it.source_name() + " at " + it.at() +
            ": empty chromosome name"
        );
    }
    assert( !it || !utils::is_graph( *it ));

    // Read position.
    next_field_( it );
    target.position = utils::parse_unsigned_integer<size_t>( it );
    if( target.position == 0 ) {
        throw std::runtime_error(
            "Malformed pileup " + it.source_name() + " at " + it.at() +
            ": chromosome position == 0, while pileup demands 1-based positions"
        );
    }
    assert( !it || !utils::is_digit( *it ));

    // Read reference base. We also set the alternative base, just in case, to make sure that
    // it has the value that we need it to have in absence of actual data.
    next_field_( it );
    auto rb = utils::to_upper( *it );
    if( ! is_valid_base_or_n( rb )) {
        if( strict_bases_ ) {
            throw std::runtime_error(
                "Malformed pileup " + it.source_name() + " at " + it.at() +
                ": Invalid reference base that is not in [ACGTN]"
            );
        } else {
            rb = 'N';
        }
    }
    target.reference_base = rb;
    set_target_alternative_base_( target );
    ++it;

    // Read the samples. We switch once for the first line, and thereafter check that we read the
    // same number of samples each time.
    size_t src_index = 0;
    if( target.samples.empty() ) {
        while( it && *it != '\n' ) {
            if(
                ! use_sample_filter ||
                ( src_index < sample_filter.size() && sample_filter[src_index] )
            ) {
                target.samples.emplace_back();
                process_sample_( it, target, target.samples.back() );
            } else {
                skip_sample_( it );
            }
            ++src_index;
        }
    } else {
        // Here we need two indices, one over the samples in the file (source),
        // and one for the samples that we are writing in our Record (destination).
        size_t dst_index = 0;
        while( it && *it != '\n' ) {
            if(
                ! use_sample_filter ||
                ( src_index < sample_filter.size() && sample_filter[src_index] )
            ) {
                if( dst_index >= target.samples.size() ) {
                    throw std::runtime_error(
                        "Malformed pileup " + it.source_name() + " at " + it.at() +
                        ": Line with different number of samples."
                    );
                }
                assert( dst_index < target.samples.size() );

                process_sample_( it, target, target.samples[dst_index] );
                ++dst_index;
            } else {
                skip_sample_( it );
            }
            ++src_index;
        }
        if( dst_index != target.samples.size() ) {
            throw std::runtime_error(
                "Malformed pileup " + it.source_name() + " at " + it.at() +
                ": Line with different number of samples."
            );
        }
    }
    if( use_sample_filter && src_index != sample_filter.size() ) {
        throw std::runtime_error(
            "Malformed pileup " + it.source_name() + " at " + it.at() +
            ": Number of samples in the line does not match the number of filter entries."
        );
    }

    assert( !it || *it == '\n' );
    ++it;
    return true;
}

// -------------------------------------------------------------------------
//     Process Sample
// -------------------------------------------------------------------------

template<class T, class S>
void SimplePileupReader::process_sample_(
    utils::InputStream& input_stream,
    T const&            target,
    S&                  sample
) const {
    // Shorthand.
    auto& it = input_stream;

    // Reset the sample.
    sample = S();

    // Read the total read depth / coverage.
    next_field_( it );
    auto const read_depth = utils::parse_unsigned_integer<size_t>( it );
    set_sample_read_depth_( read_depth, sample );
    assert( !it || !utils::is_digit( *it ));

    // Read the nucleotides, skipping everything that we don't want. We need to store these
    // in a string first, as we want to do quality checks. Bit unfortunate, and maybe there
    // is a smart way to avoid this for cases without quality string (without code duplication).
    // Goot enough for now though.
    // We use two processing methods, a fast one based on the input buffer if possible,
    // and if that failes (because the end of the bases is beyond the buffer end),
    // we run a slower version again that does not work on the buffer, but that should be rare.
    next_field_( it );
    auto const done_reading_bases = process_sample_read_bases_buffer_( it, target.reference_base );
    if( ! done_reading_bases ) {
        // Try again with the slow method.
        process_sample_read_bases_stream_( it, target.reference_base );
    }
    set_sample_read_bases_( base_buffer_, sample );

    // Read depth count error check. We here allow for the same weird special case of a deletion
    // that does not count for the depth, as described in convert_to_sample_counts().
    if(
        base_buffer_.size() != read_depth &&
        !( read_depth == 0 && base_buffer_.size() == 1 )
    ) {
        throw std::runtime_error(
            "Malformed pileup " + it.source_name() + " at " + it.at() +
            ": Given read count (" + std::to_string( read_depth ) +
            ") does not match the number of bases found in the sample (" +
            std::to_string( base_buffer_.size() ) + ")."
        );
    }

    // Now read the quality codes, if present.
    process_quality_string_( it, sample );

    // Also check if we want to read the ancestral base, if present.
    process_ancestral_base_( it, sample );

    // Final file sanity checks.
    if( it && !( utils::is_blank( *it ) || utils::is_newline( *it ))) {
        throw std::runtime_error(
            "Malformed pileup " + it.source_name() + " at " + it.at() +
            ": Invalid characters."
        );
    }
}

// -------------------------------------------------------------------------
//     set_target_alternative_base_
// -------------------------------------------------------------------------

template<>
void SimplePileupReader::set_target_alternative_base_<SimplePileupReader::Record>(
    SimplePileupReader::Record& target
) const {
    // The pileup format does not have an alterantive base, so we do nothing here.
    (void) target;
}

template<>
void SimplePileupReader::set_target_alternative_base_<Variant>(
    Variant& target
) const {
    // The format does not have an ancestral base,
    // but we want to make sure that it is set to a defined value in the Variant.
    target.alternative_base = 'N';
}

// -------------------------------------------------------------------------
//     set_sample_read_depth_
// -------------------------------------------------------------------------

template<>
void SimplePileupReader::set_sample_read_depth_<SimplePileupReader::Sample>(
    size_t read_depth,
    SimplePileupReader::Sample& sample
) const {
    sample.read_depth = read_depth;
}

template<>
void SimplePileupReader::set_sample_read_depth_<SampleCounts>(
    size_t read_depth,
    SampleCounts& sample
) const {
    // Variant SampleCounts don't use read depth
    (void) read_depth;
    (void) sample;
}

// -------------------------------------------------------------------------
//     process_sample_read_bases_buffer_
// -------------------------------------------------------------------------

bool SimplePileupReader::process_sample_read_bases_buffer_(
    utils::InputStream& input_stream,
    char reference_base
) const {
    // Shorthand.
    auto& it = input_stream;
    auto const in_buff = input_stream.buffer();
    base_buffer_.clear();

    // No need to compute upper and lower case again and again here.
    auto const u_ref_base = utils::to_upper( reference_base );
    auto const l_ref_base = utils::to_lower( reference_base );

    // Go through the bases and store them in the buffer,
    // keeping track of the position (pos) in the buffer.
    size_t pos = 0;
    while( pos < in_buff.second ) {

        // Stop when we reach the end of the bases.
        if( ! utils::is_graph( in_buff.first[pos] )) {
            break;
        }

        // Process the char. This is some code duplication with process_sample_read_bases_stream_(),
        // but the methods process the stream/buffer differently for each of those choices here,
        // so not much we can do here without too much effort...
        switch( in_buff.first[pos] ) {
            case '+':
            case '-': {
                // A sequence matching `[+-][0-9]+[ACGTNacgtn]+` is an insertion or deletion.
                // We skip/ignore those, following the format definition to get the valid chars.
                // See http://www.htslib.org/doc/samtools-mpileup.html
                static const std::string allowed_codes = "ACGTN*#";

                // First, we need to get how many chars there are in this indel.
                ++pos;
                if( pos >= in_buff.second ) {
                    // If we reached the end of the buffer here, we do not have enough chars
                    // in the buffer to continue here... use the slow method instead.
                    // This means that we will go to the next buffer block there, and hence will
                    // likely return to this faster method for the next sample again then.
                    return false;
                }
                errno = 0;
                char* endptr;
                auto const indel_cnt = std::strtoul( &in_buff.first[pos], &endptr, 10 );
                if( errno == ERANGE || &in_buff.first[pos] == endptr ) {
                    throw std::runtime_error(
                        "Malformed pileup " + it.source_name() + " near " + it.at() +
                        ": Line with invalid indel characters count that is not a valid number."
                    );
                }

                // We have read a number. Go to the next char in the buffer after that.
                pos += endptr - &in_buff.first[pos];
                if( pos >= in_buff.second ) {
                    // Same as before: Buffer not large enough,
                    // need to do slow method for this sample.
                    return false;
                }

                // Now, we skip as many chars as the number we read, making sure that all is in order.
                for( size_t i = 0; i < indel_cnt; ++i ) {
                    if( pos >= in_buff.second ) {
                        // Same as before: Buffer not large enough,
                        // need to do slow method for this sample.
                        return false;
                    }
                    if(
                        strict_bases_ &&
                        !std::strchr( allowed_codes.c_str(), utils::to_upper( in_buff.first[pos]))
                    ) {
                        throw std::runtime_error(
                            "Malformed pileup " + it.source_name() + " near " + it.at() +
                            ": Line with invalid indel character " +
                            utils::char_to_hex( in_buff.first[pos])
                        );
                    }
                    ++pos;
                }
                break;
            }
            case '^': {
                // Caret marks the start of a read segment, followed by a char for the mapping
                // quality. We skip both of these.
                ++pos;
                if( pos >= in_buff.second ) {
                    // Same as above: Buffer not large enough,
                    // need to do slow method for this sample.
                    return false;
                }
                ++pos;
                break;
            }
            case '$': {
                // Dollar marks the end of a read segment. Skip.
                ++pos;
                break;
            }
            case '.': {
                // pileup wants '.' to be the ref base in upper case...
                base_buffer_ += u_ref_base;
                ++pos;
                break;
            }
            case ',': {
                // ...and ',' to be the ref base in lower case
                base_buffer_ += l_ref_base;
                ++pos;
                break;
            }
            default: {
                // Everything else we simply add as-is.
                base_buffer_ += in_buff.first[pos];
                ++pos;
                break;
            }
        }
    }
    assert( pos == in_buff.second || !utils::is_graph( in_buff.first[pos] ) );

    // Now we have reached the end of the buffer-based approach.
    // If that worked, that is, if we are not at the end of the buffer, and so have found
    // the end of the bases, we are good and can move to the end of what we just read.
    // If not, we return false in order to do a second, slower pass, to catch everything.
    if( pos < in_buff.second ) {
        // We stopped the above loop on any non-graph char, such as new lines,
        // so this jump never goes across one, which is important.
        it.jump_unchecked( pos );
        return true;
    }
    return false;
}

// -------------------------------------------------------------------------
//     process_sample_read_bases_stream_
// -------------------------------------------------------------------------

void SimplePileupReader::process_sample_read_bases_stream_(
    utils::InputStream& input_stream,
    char reference_base
) const {
    // Shorthand.
    auto& it = input_stream;
    base_buffer_.clear();

    while( it && utils::is_graph( *it )) {
        auto const c = *it;
        switch( c ) {
            case '+':
            case '-': {
                // A sequence matching `[+-][0-9]+[ACGTNacgtn]+` is an insertion or deletion.
                // We skip/ignore those.

                // We first here allowed degenerated chars, and made a buffer for the codes
                // that are allowed in indels - like this:
                // static const std::string allowed_codes = sequence::nucleic_acid_codes_all_letters();

                // But later, we changed this to use the the proper pileup definition here,
                // see http://www.htslib.org/doc/samtools-mpileup.html
                static const std::string allowed_codes = "ACGTN*#";

                // First, we need to get how many chars there are in this indel.
                ++it;
                size_t const indel_cnt = utils::parse_unsigned_integer<size_t>( it );

                // Then, we skip that many chars, making sure that all is in order.
                for( size_t i = 0; i < indel_cnt; ++i ) {
                    if( !it ) {
                        throw std::runtime_error(
                            "Malformed pileup " + it.source_name() + " at " + it.at() +
                            ": Line with missing indel characters."
                        );
                    }
                    if(
                        strict_bases_ &&
                        !std::strchr( allowed_codes.c_str(), utils::to_upper( *it ))
                    ) {
                        throw std::runtime_error(
                            "Malformed pileup " + it.source_name() + " at " + it.at() +
                            ": Line with invalid indel character " + utils::char_to_hex( *it )
                        );
                    }
                    ++it;
                }
                break;
            }
            case '^': {
                // Caret marks the start of a read segment, followed by a char for the mapping
                // quality. We skip both of these.
                ++it;
                if( !it ) {
                    throw std::runtime_error(
                        "Malformed pileup " + it.source_name() + " at " + it.at() +
                        ": Line with invalid start of read segment marker"
                    );
                }
                ++it;
                break;
            }
            case '$': {
                // Dollar marks the end of a read segment. Skip.
                ++it;
                break;
            }
            case '.': {
                // pileup wants '.' to be the ref base in upper case...
                base_buffer_ += utils::to_upper( reference_base );
                ++it;
                break;
            }
            case ',': {
                // ...and ',' to be the ref base in lower case
                base_buffer_ += utils::to_lower( reference_base );
                ++it;
                break;
            }
            default: {
                // Everything else we simply add as-is.
                base_buffer_ += c;
                ++it;
                break;
            }
        }
    }
    assert( !it || !utils::is_graph( *it ) );
}

// -------------------------------------------------------------------------
//     set_sample_read_bases_
// -------------------------------------------------------------------------

template<>
void SimplePileupReader::set_sample_read_bases_<SimplePileupReader::Sample>(
    std::string const& read_bases,
    SimplePileupReader::Sample& sample
) const {
    sample.read_bases = read_bases;
}

template<>
void SimplePileupReader::set_sample_read_bases_<SampleCounts>(
    std::string const& read_bases,
    SampleCounts& sample
) const {
    // Variant SampleCounts don't use read bases
    (void) read_bases;
    (void) sample;
}

// -------------------------------------------------------------------------
//     process_quality_string_
// -------------------------------------------------------------------------

template<>
void SimplePileupReader::process_quality_string_<SimplePileupReader::Sample>(
    utils::InputStream& input_stream,
    SimplePileupReader::Sample& sample
) const {
    // Shorthand.
    auto& it = input_stream;

    // Now read the quality codes, if present.
    if( with_quality_string_ ) {
        next_field_( it );
        sample.phred_scores.reserve( sample.read_depth );
        while( it && utils::is_graph( *it )) {
            ++quality_code_counts_[*it];
            sample.phred_scores.push_back( sequence::quality_decode_to_phred_score(
                *it, quality_encoding_
            ));
            ++it;
        }
        assert( !it || !utils::is_graph( *it ) );

        // Version that processes the whole string at once. Not much time saved, as we need
        // to allocate the string first. Maybe refine later for speed.
        // std::string qual;
        // qual.reserve( sample.read_depth );
        // while( it && utils::is_graph( *it )) {
        //     qual += *it;
        //     ++it;
        // }
        // sample.phred_scores = sequence::quality_decode_to_phred_score( qual, quality_encoding_ );

        if( sample.read_bases.size() != sample.phred_scores.size() ) {
            throw std::runtime_error(
                "Malformed pileup " + it.source_name() + " at " + it.at() +
                ": Line contains " + std::to_string( sample.read_bases.size() ) + " bases, but " +
                std::to_string( sample.phred_scores.size() ) + " quality score codes."
            );
        }
    }
    assert( sample.phred_scores.empty() || sample.read_bases.size() == sample.phred_scores.size() );
    assert( !it || !utils::is_graph( *it ) );
}

template<>
void SimplePileupReader::process_quality_string_<SampleCounts>(
    utils::InputStream& input_stream,
    SampleCounts& sample
) const {
    // Shorthand.
    auto& it = input_stream;

    // Now read the quality codes, if present.
    if( with_quality_string_ ) {
        next_field_( it );

        // We use the internal buffer of the input stream for speed.
        // If that failes, because we reach the end of it before finishing the field here,
        // we do a slow pass, but that should be rare.
        auto const in_buff = it.buffer();

        // The counts should not have been touched yet. We started with a fresh SampleCounts,
        // and this function is the only one that calls tally_base_(), so all counts should be 0.
        assert(
            sample.a_count == 0 && sample.c_count == 0 && sample.g_count == 0 &&
            sample.t_count == 0 && sample.n_count == 0 && sample.d_count == 0
        );

        // Go through the quality scores, and tally up the bases that have a high enough quality,
        // keeping track of the position (pos) in the buffer.
        size_t pos = 0;
        for( ; pos < in_buff.second; ++pos ) {
            // Stop when we reach the end of the quality scores.
            if( ! utils::is_graph( in_buff.first[pos] )) {
                break;
            }

            // Check that we do not read more quality than we have bases.
            if( pos >= base_buffer_.size() ) {
                throw std::runtime_error(
                    "Malformed pileup " + it.source_name() + " at " + it.at() +
                    ": Line contains " + std::to_string( base_buffer_.size() ) + " bases, but " +
                    std::to_string( pos ) + " or more quality score codes."
                );
            }

            // Process the score, and tally up its base if the score is high enough.
            if( min_base_quality_ > 0 ) {
                auto const score = sequence::quality_decode_to_phred_score(
                    in_buff.first[pos], quality_encoding_
                );
                if( score >= min_base_quality_ ) {
                    tally_base_( it, sample, base_buffer_[pos] );
                }
            } else {
                // If the min qual is 0, we do not need to check and convert the score,
                // as it will pass that threshold anyway.
                tally_base_( it, sample, base_buffer_[pos] );
            }
        }
        assert( pos == in_buff.second || !utils::is_graph( in_buff.first[pos] ) );

        // Now we have reached the end of the buffer-based approach.
        // If that worked, that is, if we are not at the end of the buffer, and so have found
        // the end of the quality scores, we are good and can move to the end of what we just read.
        // If not, we reset the counts and do a second, slower pass, to catch everything.
        if( pos < in_buff.second ) {
            // We stopped the above loop on any non-graph char, such as new lines,
            // so this jump never goes across one, which is important.
            it.jump_unchecked( pos );
        } else {
            // Reset
            pos = 0;
            sample = SampleCounts();

            // Go through the quality scores, and tally up the bases that have a high enough quality,
            // keeping track of the position (pos) in the buffer.
            while( it && utils::is_graph( *it )) {
                if( pos >= base_buffer_.size() ) {
                    throw std::runtime_error(
                        "Malformed pileup " + it.source_name() + " at " + it.at() +
                        ": Line contains " + std::to_string( base_buffer_.size() ) + " bases, but " +
                        std::to_string( pos ) + " or more quality score codes."
                    );
                }

                // Process the score, and tally up its base if the score is high enough.
                auto const score = sequence::quality_decode_to_phred_score( *it, quality_encoding_ );
                if( score >= min_base_quality_ ) {
                    tally_base_( it, sample, base_buffer_[pos] );
                }

                ++pos;
                ++it;
            }
            assert( !it || !utils::is_graph( *it ) );
        }

        // Last check: Did we reach exactly as many quality codes as we have bases?
        if( pos != base_buffer_.size() ) {
            throw std::runtime_error(
                "Malformed pileup " + it.source_name() + " at " + it.at() +
                ": Line contains " + std::to_string( base_buffer_.size() ) + " bases, but " +
                std::to_string( pos ) + " quality score codes."
            );
        }
    } else {
        // Without quality scores, simply tally up all the bases.
        // This is the part that could be optimized by not storing the bases in a string first.
        for( auto const c : base_buffer_ ) {
            tally_base_( it, sample, c );
        }
    }
    assert( !it || !utils::is_graph( *it ) );
}

inline void SimplePileupReader::tally_base_(
    utils::InputStream& input_stream,
    SampleCounts& sample,
    char b
) const {
    switch( b ) {
        case 'a':
        case 'A': {
            ++sample.a_count;
            break;
        }
        case 'c':
        case 'C': {
            ++sample.c_count;
            break;
        }
        case 'g':
        case 'G': {
            ++sample.g_count;
            break;
        }
        case 't':
        case 'T': {
            ++sample.t_count;
            break;
        }
        case 'n':
        case 'N': {
            ++sample.n_count;
            break;
        }
        case '*':
        case '#': {
            ++sample.d_count;
            break;
        }
        case '<':
        case '>': {
            break;
        }
        default: {
            throw std::runtime_error(
                "Malformed pileup " + input_stream.source_name() + " near " + input_stream.at() +
                ": Invalid allele character " + utils::char_to_hex( b )
            );
        }
    }
}

// -------------------------------------------------------------------------
//     process_ancestral_base_
// -------------------------------------------------------------------------

template<>
void SimplePileupReader::process_ancestral_base_<SimplePileupReader::Sample>(
    utils::InputStream& input_stream,
    SimplePileupReader::Sample& sample
) const {
    // Shorthand.
    auto& it = input_stream;

    if( with_ancestral_base_ ) {
        next_field_( it );
        // We can simply read in the char here. Even if the iterator is at its end, it will
        // simply return a null char, which will trigger the subsequent error check.
        char ab = utils::to_upper( *it );
        if( !it || is_valid_base_or_n( ab )) {
            if( strict_bases_ ) {
                throw std::runtime_error(
                    "Malformed pileup " + it.source_name() + " at " + it.at() +
                    ": Expecting ancestral base character in [ACGTN]."
                );
            } else {
                ab = 'N';
            }
        }
        sample.ancestral_base = ab;
        ++it;
    }
}

template<>
void SimplePileupReader::process_ancestral_base_<SampleCounts>(
    utils::InputStream& input_stream,
    SampleCounts& sample
) const {
    (void) input_stream;
    (void) sample;

    // Also check if we want to read the ancestral base, if present.
    if( with_ancestral_base_ ) {
        // throw std::runtime_error(
        //     "SimplePileupReader currently does not implement to read (m)pileup files "
        //     "with ancestral bases when reading Variants."
        // );

        // Let's simply read and ignore the ancestral base, as our Variant/SampleCounts setup
        // does not store those at the moment.
        // For simplicity and to avoid code duplication, we just call the other version of this
        // function with a dummy Sample. This is not super efficient, but given how rare pileups
        // with ancestral base are, this is totally fine for now. Can optimize later.
        SimplePileupReader::Sample dummy;
        process_ancestral_base_( input_stream, dummy );
    }
}

// -------------------------------------------------------------------------
//     skip_sample_
// -------------------------------------------------------------------------

void SimplePileupReader::skip_sample_(
    utils::InputStream& input_stream
) const {
    // Shorthand.
    auto& it = input_stream;

    // Read the total read count / coverage.
    next_field_( it );
    utils::skip_while( it, utils::is_digit );
    assert( !it || !utils::is_digit( *it ));

    // Read the nucleotides.
    next_field_( it );
    utils::skip_while( it, utils::is_graph );
    assert( !it || !utils::is_graph( *it ) );

    // Read the quality codes, if present.
    if( with_quality_string_ ) {
        next_field_( it );
        utils::skip_while( it, utils::is_graph );
    }
    assert( !it || !utils::is_graph( *it ) );

    // Read the ancestral base, if present.
    if( with_ancestral_base_ ) {
        next_field_( it );
        utils::skip_while( it, utils::is_graph );
    }
    assert( !it || !utils::is_graph( *it ) );

    // Final file sanity checks.
    if( it && !( utils::is_blank( *it ) || utils::is_newline( *it ))) {
        throw std::runtime_error(
            "Malformed pileup " + it.source_name() + " at " + it.at() +
            ": Invalid characters."
        );
    }
}

// -------------------------------------------------------------------------
//     next_field_
// -------------------------------------------------------------------------

void SimplePileupReader::next_field_( utils::InputStream& input_stream ) const
{
    // There needs to be at last some whitespace that separates the field. Affirm that,
    // then skip it until we are at the content of the next field.
    // utils::affirm_char_or_throw( input_stream, utils::is_blank );
    // utils::skip_while( input_stream, utils::is_blank );
    // assert( !input_stream || !utils::is_blank( *input_stream ));

    // Nope, the above skips empty fields, which can occurr when there are no bases
    // at a position at all. Let's just follow the standard more strictly, and check for a tab.
    utils::affirm_char_or_throw( input_stream, '\t' );
    ++input_stream;
}

} // namespace population
} // namespace genesis
