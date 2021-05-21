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

#include "genesis/population/formats/variant_pileup_reader.hpp"

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
//     Reading & Parsing
// =================================================================================================

std::vector<Variant> VariantPileupReader::read(
    std::shared_ptr< utils::BaseInputSource > source
) const {
    std::vector<Variant> result;
    utils::InputStream it( source );

    Variant var;
    while( parse_line_( it, var, {}, false )) {
        result.push_back( var );
    }
    return result;
}

std::vector<Variant> VariantPileupReader::read(
    std::shared_ptr< utils::BaseInputSource > source,
    std::vector<size_t> const& sample_indices
) const {
    std::vector<Variant> result;
    utils::InputStream it( source );

    // Convert the list of indices to a bool vec that tells which samples we want to process.
    auto const sample_filter = utils::make_bool_vector_from_indices( sample_indices );

    Variant var;
    while( parse_line_( it, var, sample_filter, true )) {
        result.push_back( var );
    }
    return result;
}

std::vector<Variant> VariantPileupReader::read(
    std::shared_ptr< utils::BaseInputSource > source,
    std::vector<bool> const& sample_filter
) const {
    std::vector<Variant> result;
    utils::InputStream it( source );

    Variant var;
    while( parse_line_( it, var, sample_filter, true )) {
        result.push_back( var );
    }
    return result;
}

bool VariantPileupReader::parse_line(
    utils::InputStream& input_stream,
    Variant& variant
) const {
    return parse_line_( input_stream, variant, {}, false );
}

bool VariantPileupReader::parse_line(
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

bool VariantPileupReader::parse_line_(
    utils::InputStream&      input_stream,
    Variant&                 variant,
    std::vector<bool> const& sample_filter,
    bool                     use_sample_filter
) const {
    // Shorthand.
    auto& it = input_stream;

    // If we reached the end of the input stream, reset the variant.
    // See SimplePileupReader for details.
    if( !it ) {
        variant = Variant();
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
    variant.chromosome = utils::read_while( it, utils::is_graph );
    assert( !it || !utils::is_graph( *it ));

    // Read position.
    next_field_( it );
    variant.position = utils::parse_unsigned_integer<size_t>( it );
    assert( !it || !utils::is_digit( *it ));

    // Read reference base.
    next_field_( it );
    auto const rb = utils::to_upper( *it );
    if( rb != 'A' && rb != 'C' && rb != 'G' && rb != 'T' && rb != 'N' ) {
        throw std::runtime_error(
            "Malformed pileup " + it.source_name() + " at " + it.at() +
            ": Invalid reference base that is not in [ACGTN]"
        );
    }
    variant.reference_base = rb;
    ++it;

    // Read the samples. We switch once for the first line, and thereafter check that we read the
    // same number of samples each time.
    if( variant.samples.empty() ) {
        size_t src_index = 0;
        while( it && *it != '\n' ) {
            if( !use_sample_filter || ( src_index < sample_filter.size() && sample_filter[src_index] )) {
                variant.samples.emplace_back();
                process_sample_( it, variant, variant.samples.back() );
            } else {
                skip_sample_( it );
            }
            ++src_index;
        }
    } else {
        // Here we need two indices, one over the samples in the file (source),
        // and one for the samples that we are writing in our Variant (destination).
        size_t src_index = 0;
        size_t dst_index = 0;
        while( it && *it != '\n' ) {
            if( dst_index >= variant.samples.size() ) {
                throw std::runtime_error(
                    "Malformed pileup " + it.source_name() + " at " + it.at() +
                    ": Line with different number of samples."
                );
            }
            if( !use_sample_filter || ( src_index < sample_filter.size() && sample_filter[src_index] )) {
                assert( dst_index < variant.samples.size() );
                process_sample_( it, variant, variant.samples[dst_index] );
                ++dst_index;
            } else {
                skip_sample_( it );
            }
            ++src_index;
        }
        if( dst_index != variant.samples.size() ) {
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

void VariantPileupReader::process_sample_(
    utils::InputStream& input_stream,
    Variant const&      variant,
    BaseCounts&         base_counts
) const {
    // Shorthand.
    auto& it = input_stream;

    // Reset the sample base counts and the buffer (which does not reallicate, good for us!).
    base_counts = BaseCounts();
    base_buffer_.clear();

    // Read the total read count / coverage, which we could use to reserve some space in the buffer,
    // but don't because after a few lines, the buffer will be large enough anyway,
    // so let's safe the extra effort.
    next_field_( it );
    auto const read_coverage = utils::parse_unsigned_integer<size_t>( it );
    assert( !it || !utils::is_digit( *it ));

    // Read the nucleotides, skipping everything that we don't want. We need to store these
    // in a string first, as we might want to do quality checks. Bit unfortunate, and maybe there
    // is a smart way to avoid this for cases without quality string (without code duplication).
    // Goot enough for now though.
    next_field_( it );
    // base_buffer_.reserve( read_coverage );
    while( it && utils::is_graph( *it )) {
        auto const c = *it;
        switch( c ) {
            case '+':
            case '-': {
                // See SimplePileupReader for details on this part.
                static const std::string allowed_codes = "ACGTN*#";

                // First, we need to get how many chars there are in this indel.
                ++it;
                size_t const indel_cnt = utils::parse_unsigned_integer<size_t>( it );

                // Then, we skip that many chars, making sure that all is in order.
                for( size_t i = 0; i < indel_cnt; ++i ) {
                    if( !it || !std::strchr( allowed_codes.c_str(), utils::to_upper( *it ))) {
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
            case '.':
            case ',': {
                // Same treatment for '.' and ',' here. Typically, pileup wants '.' to be upper
                // case, and ',' to be lower case, but here, it does not make a difference.
                base_buffer_ += variant.reference_base ;
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
            if( score >= min_phred_score_ ) {
                tally_base_( it, base_counts, base_buffer_[pos] );
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
            tally_base_( it, base_counts, c );
        }
    }
    assert( !it || !utils::is_graph( *it ) );

    // Also check if we want to read the ancestral base, if present.
    if( with_ancestral_base_ ) {
        throw std::runtime_error(
            "VariantPileupReader currently does not implement to read (m)pileup files "
            "with ancestral bases."
        );
    }

    // Final file sanity checks.
    if( it && !( utils::is_blank( *it ) || utils::is_newline( *it ))) {
        throw std::runtime_error(
            "Malformed pileup " + it.source_name() + " at " + it.at() + ": Invalid characters."
        );
    }
}

// -------------------------------------------------------------------------
//     Skip Sample
// -------------------------------------------------------------------------

void VariantPileupReader::skip_sample_(
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

void VariantPileupReader::next_field_( utils::InputStream& input_stream ) const
{
    // There needs to be at last some whitespace that separates the field. Affirm that,
    // then skip it until we are at the content of the next field.
    utils::affirm_char_or_throw( input_stream, utils::is_blank );
    utils::skip_while( input_stream, utils::is_blank );
    assert( !input_stream || !utils::is_blank( *input_stream ));
}

// -------------------------------------------------------------------------
//     Base Counts Tally
// -------------------------------------------------------------------------

void VariantPileupReader::tally_base_(
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

} // namespace population
} // namespace genesis
