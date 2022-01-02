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
 * @ingroup population
 */

#include "genesis/population/formats/simple_pileup_reader.hpp"

#include "genesis/sequence/functions/codes.hpp"
#include "genesis/utils/io/char.hpp"
#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/math/bitvector/helper.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Reading Records
// =================================================================================================

/**
 * @brief Local helper function to remove code duplication for the correct input order check.
 */
void process_pileup_correct_input_order_check_(
    utils::InputStream const& it,
    std::string& cur_chr, size_t& cur_pos,
    std::string const& new_chr, size_t new_pos
) {
    if( new_chr < cur_chr || ( new_chr == cur_chr && new_pos <= cur_pos )) {
        throw std::runtime_error(
            "Malformed pileup " + it.source_name() + " at " + it.at() +
            ": unordered chromosomes and positions"
        );
    }
    cur_chr = new_chr;
    cur_pos = new_pos;
}

std::vector<SimplePileupReader::Record> SimplePileupReader::read_records(
    std::shared_ptr< utils::BaseInputSource > source
) const {
    std::vector<SimplePileupReader::Record> result;
    utils::InputStream it( source );

    // Read, with correct order check, just in case.
    std::string cur_chr = "";
    size_t      cur_pos = 0;
    Record rec;
    while( parse_line_( it, rec, {}, false )) {
        process_pileup_correct_input_order_check_( it, cur_chr, cur_pos, rec.chromosome, rec.position );
        result.push_back( rec );
    }
    return result;
}

std::vector<SimplePileupReader::Record> SimplePileupReader::read_records(
    std::shared_ptr< utils::BaseInputSource > source,
    std::vector<size_t> const& sample_indices
) const {
    std::vector<SimplePileupReader::Record> result;
    utils::InputStream it( source );

    // Convert the list of indices to a bool vec that tells which samples we want to process.
    auto const sample_filter = utils::make_bool_vector_from_indices( sample_indices );

    // Read, with correct order check, just in case.
    std::string cur_chr = "";
    size_t      cur_pos = 0;
    Record rec;
    while( parse_line_( it, rec, sample_filter, true )) {
        process_pileup_correct_input_order_check_( it, cur_chr, cur_pos, rec.chromosome, rec.position );
        result.push_back( rec );
    }
    return result;
}

std::vector<SimplePileupReader::Record> SimplePileupReader::read_records(
    std::shared_ptr< utils::BaseInputSource > source,
    std::vector<bool> const& sample_filter
) const {
    std::vector<SimplePileupReader::Record> result;
    utils::InputStream it( source );

    // Read, with correct order check, just in case.
    std::string cur_chr = "";
    size_t      cur_pos = 0;
    Record rec;
    while( parse_line_( it, rec, sample_filter, true )) {
        process_pileup_correct_input_order_check_( it, cur_chr, cur_pos, rec.chromosome, rec.position );
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

    // Read, with correct order check, just in case.
    std::string cur_chr = "";
    size_t      cur_pos = 0;
    Variant var;
    while( parse_line_( it, var, {}, false )) {
        process_pileup_correct_input_order_check_( it, cur_chr, cur_pos, var.chromosome, var.position );
        result.push_back( var );
    }
    return result;
}

std::vector<Variant> SimplePileupReader::read_variants(
    std::shared_ptr< utils::BaseInputSource > source,
    std::vector<size_t> const& sample_indices
) const {
    std::vector<Variant> result;
    utils::InputStream it( source );

    // Convert the list of indices to a bool vec that tells which samples we want to process.
    auto const sample_filter = utils::make_bool_vector_from_indices( sample_indices );

    // Read, with correct order check, just in case.
    std::string cur_chr = "";
    size_t      cur_pos = 0;
    Variant var;
    while( parse_line_( it, var, sample_filter, true )) {
        process_pileup_correct_input_order_check_( it, cur_chr, cur_pos, var.chromosome, var.position );
        result.push_back( var );
    }
    return result;
}

std::vector<Variant> SimplePileupReader::read_variants(
    std::shared_ptr< utils::BaseInputSource > source,
    std::vector<bool> const& sample_filter
) const {
    std::vector<Variant> result;
    utils::InputStream it( source );

    // Read, with correct order check, just in case.
    std::string cur_chr = "";
    size_t      cur_pos = 0;
    Variant var;
    while( parse_line_( it, var, sample_filter, true )) {
        process_pileup_correct_input_order_check_( it, cur_chr, cur_pos, var.chromosome, var.position );
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
    return parse_line_( input_stream, variant, {}, false );
}

bool SimplePileupReader::parse_line_variant(
    utils::InputStream& input_stream,
    Variant& variant,
    std::vector<bool> const& sample_filter
) const {
    return parse_line_( input_stream, variant, sample_filter, true );
}

// =================================================================================================
//     Internal Members
// =================================================================================================

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
            ": chromosome position == 0"
        );
    }
    assert( !it || !utils::is_digit( *it ));

    // Read reference base.
    next_field_( it );
    auto rb = utils::to_upper( *it );
    if( rb != 'A' && rb != 'C' && rb != 'G' && rb != 'T' && rb != 'N' ) {
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
    ++it;

    // Read the samples. We switch once for the first line, and thereafter check that we read the
    // same number of samples each time.
    if( target.samples.empty() ) {
        size_t src_index = 0;
        while( it && *it != '\n' ) {
            if( !use_sample_filter || ( src_index < sample_filter.size() && sample_filter[src_index] )) {
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
        size_t src_index = 0;
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
    base_buffer_.clear();

    // Read the total read count / coverage.
    next_field_( it );
    auto const read_coverage = utils::parse_unsigned_integer<size_t>( it );
    set_sample_read_coverage_( read_coverage, sample );
    assert( !it || !utils::is_digit( *it ));

    // Read the nucleotides, skipping everything that we don't want. We need to store these
    // in a string first, as we want to do quality checks. Bit unfortunate, and maybe there
    // is a smart way to avoid this for cases without quality string (without code duplication).
    // Goot enough for now though.
    next_field_( it );
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
                base_buffer_ += utils::to_upper( target.reference_base );
                ++it;
                break;
            }
            case ',': {
                // ...and ',' to be the ref base in lower case
                base_buffer_ += utils::to_lower( target.reference_base );
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
    set_sample_read_bases_( base_buffer_, sample );

    // Read coverage count error check. We here allow for the same weird special case of a deletion
    // that does not count for the coverage, as described in convert_to_base_counts().
    if(
        base_buffer_.size() != read_coverage &&
        !( read_coverage == 0 && base_buffer_.size() == 1 )
    ) {
        throw std::runtime_error(
            "Malformed pileup " + it.source_name() + " at " + it.at() +
            ": Given read count (" + std::to_string( read_coverage ) +
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
//     Helper for read coverage
// -------------------------------------------------------------------------

template<>
void SimplePileupReader::set_sample_read_coverage_<SimplePileupReader::Sample>(
    size_t read_coverage,
    SimplePileupReader::Sample& sample
) const {
    sample.read_coverage = read_coverage;
}

template<>
void SimplePileupReader::set_sample_read_coverage_<BaseCounts>(
    size_t read_coverage,
    BaseCounts& sample
) const {
    // Variant BaseCounts don't use read coverage
    (void) read_coverage;
    (void) sample;
}

// -------------------------------------------------------------------------
//     Helper for read bases
// -------------------------------------------------------------------------

template<>
void SimplePileupReader::set_sample_read_bases_<SimplePileupReader::Sample>(
    std::string const& read_bases,
    SimplePileupReader::Sample& sample
) const {
    sample.read_bases = read_bases;
}

template<>
void SimplePileupReader::set_sample_read_bases_<BaseCounts>(
    std::string const& read_bases,
    BaseCounts& sample
) const {
    // Variant BaseCounts don't use read bases
    (void) read_bases;
    (void) sample;
}

// -------------------------------------------------------------------------
//     Helper for quality strings
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
        sample.phred_scores.reserve( sample.read_coverage );
        while( it && utils::is_graph( *it )) {
            sample.phred_scores.push_back( sequence::quality_decode_to_phred_score(
                *it, quality_encoding_
            ));
            ++it;
        }
        assert( !it || !utils::is_graph( *it ) );

        // Version that processes the whole string at once. Not much time saved, as we need
        // to allocate the string first. Maybe refine later for speed.
        // std::string qual;
        // qual.reserve( sample.read_coverage );
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
void SimplePileupReader::process_quality_string_<BaseCounts>(
    utils::InputStream& input_stream,
    BaseCounts& sample
) const {
    // Shorthand.
    auto& it = input_stream;

    // Now read the quality codes, if present.
    if( with_quality_string_ ) {
        next_field_( it );

        // Go through the quality scores, and tally up the bases that have a high enough quality,
        // keeping track of the position (pos) in the buffer.
        size_t pos = 0;
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

void SimplePileupReader::tally_base_(
    utils::InputStream& input_stream,
    BaseCounts& base_count,
    char b
) const {
    switch( b ) {
        case 'a':
        case 'A': {
            ++base_count.a_count;
            break;
        }
        case 'c':
        case 'C': {
            ++base_count.c_count;
            break;
        }
        case 'g':
        case 'G': {
            ++base_count.g_count;
            break;
        }
        case 't':
        case 'T': {
            ++base_count.t_count;
            break;
        }
        case 'n':
        case 'N': {
            ++base_count.n_count;
            break;
        }
        case '*':
        case '#': {
            ++base_count.d_count;
            break;
        }
        case '<':
        case '>': {
            break;
        }
        default: {
            throw std::runtime_error(
                "Malformed pileup " + input_stream.source_name() + " at " + input_stream.at() +
                ": Invalid allele character " + utils::char_to_hex( b )
            );
        }
    }
}

// -------------------------------------------------------------------------
//     Helper for ancestral base
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
        if( !it || ( ab != 'A' && ab != 'C' && ab != 'G' && ab != 'T' && ab != 'N' )) {
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
void SimplePileupReader::process_ancestral_base_<BaseCounts>(
    utils::InputStream& input_stream,
    BaseCounts& sample
) const {
    (void) input_stream;
    (void) sample;

    // Also check if we want to read the ancestral base, if present.
    if( with_ancestral_base_ ) {
        // throw std::runtime_error(
        //     "SimplePileupReader currently does not implement to read (m)pileup files "
        //     "with ancestral bases when reading Variants."
        // );

        // Let's simply read and ignore the ancestral base, as our Variant/BaseCounts setup
        // does not store those at the moment.
        // For simplicity and to avoid code duplication, we just call the other version of this
        // function with a dummy Sample. This is not super efficient, but given how rare pileups
        // with ancestral base are, this is totally fine for now. Can optimize later.
        SimplePileupReader::Sample dummy;
        process_ancestral_base_( input_stream, dummy );
    }
}

// -------------------------------------------------------------------------
//     Skip Sample
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
//     Next Field
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
