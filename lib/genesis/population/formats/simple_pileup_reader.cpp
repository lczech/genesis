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

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Reading & Parsing
// =================================================================================================

std::vector<SimplePileupReader::Record> SimplePileupReader::read(
    std::shared_ptr< utils::BaseInputSource > source
) const {
    std::vector<SimplePileupReader::Record> result;
    utils::InputStream it( source );

    Record rec;
    while( parse_line_( it, rec, {}, false )) {
        result.push_back( rec );
    }
    return result;
}

std::vector<SimplePileupReader::Record> SimplePileupReader::read(
    std::shared_ptr< utils::BaseInputSource > source,
    std::vector<size_t> const& sample_indices
) const {
    std::vector<SimplePileupReader::Record> result;
    utils::InputStream it( source );

    // Convert the list of indices to a bool vec that tells which samples we want to process.
    auto const sample_filter = make_sample_filter( sample_indices );

    Record rec;
    while( parse_line_( it, rec, sample_filter, true )) {
        result.push_back( rec );
    }
    return result;
}

std::vector<SimplePileupReader::Record> SimplePileupReader::read(
    std::shared_ptr< utils::BaseInputSource > source,
    std::vector<bool> const& sample_filter
) const {
    std::vector<SimplePileupReader::Record> result;
    utils::InputStream it( source );

    Record rec;
    while( parse_line_( it, rec, sample_filter, true )) {
        result.push_back( rec );
    }
    return result;
}

bool SimplePileupReader::parse_line(
    utils::InputStream& input_stream,
    SimplePileupReader::Record& record
) const {
    return parse_line_( input_stream, record, {}, false );
}

bool SimplePileupReader::parse_line(
    utils::InputStream& input_stream,
    SimplePileupReader::Record& record,
    std::vector<bool> const& sample_filter
) const {
    return parse_line_( input_stream, record, sample_filter, true );
}

// =================================================================================================
//     Helper Functions
// =================================================================================================

std::vector<bool> SimplePileupReader::make_sample_filter( std::vector<size_t> const& indices )
{
    // Get the largest element of the vector. If it's empty, we return an empty vector as well.
    auto max_it = std::max_element( indices.begin(), indices.end() );
    if( max_it == indices.end() ) {
        return {};
    }
    size_t max_index = *max_it;

    // Fill a bool vector, setting all positions to true
    // that are indicated by the indices, pun intended.
    auto result = std::vector<bool>( max_index, false );
    for( auto const& idx : indices ) {
        assert( idx < result.size() );
        result[idx] = true;
    }
    return result;
}

// =================================================================================================
//     Internal Members
// =================================================================================================

// -------------------------------------------------------------------------
//     Parse Line
// -------------------------------------------------------------------------

bool SimplePileupReader::parse_line_(
    utils::InputStream&      input_stream,
    Record&                  record,
    std::vector<bool> const& sample_filter,
    bool                     use_sample_filter
) const {
    // Shorthand.
    auto& it = input_stream;

    // If we reached the end of the input stream, reset the record. We do not reset per default,
    // in order to avoid costly re-initialization of the sample vector. But when we finish with
    // an input stream, we want to reset, so that subsequent usage of this reader class does not
    // fail if the pileip file contains a different number of samples.
    // Still, the user will currently get an error when using the same reader instance to
    // simultaneously (interlaced) read from multiple pileup files with differing number of samples
    // into the same record... But who does that?! If you are a user having this issue, please
    // let me know!
    if( !it ) {
        record = Record();
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
    record.chromosome = utils::read_while( it, utils::is_graph );
    assert( !it || !utils::is_graph( *it ));

    // Read position.
    next_field_( it );
    record.position = utils::parse_unsigned_integer<size_t>( it );
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
    record.reference_base = rb;
    ++it;

    // Read the samples. We switch once for the first line, and thereafter check that we read the
    // same number of samples each time.
    if( record.samples.empty() ) {
        size_t src_index = 0;
        while( it && *it != '\n' ) {
            if( !use_sample_filter || ( src_index < sample_filter.size() && sample_filter[src_index] )) {
                record.samples.emplace_back();
                process_sample_( it, record, record.samples.back() );
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
            if( dst_index >= record.samples.size() ) {
                throw std::runtime_error(
                    "Malformed pileup " + it.source_name() + " at " + it.at() +
                    ": Line with different number of samples."
                );
            }
            if( !use_sample_filter || ( src_index < sample_filter.size() && sample_filter[src_index] )) {
                assert( dst_index < record.samples.size() );
                process_sample_( it, record, record.samples[dst_index] );
                ++dst_index;
            } else {
                skip_sample_( it );
            }
            ++src_index;
        }
        if( dst_index != record.samples.size() ) {
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

void SimplePileupReader::process_sample_(
    utils::InputStream& input_stream,
    Record&             record,
    Sample&             sample
) const {
    // Shorthand.
    auto& it = input_stream;

    // Reset the sample.
    sample = Sample();

    // Read the total read count / coverage.
    next_field_( it );
    sample.read_coverage = utils::parse_unsigned_integer<size_t>( it );
    assert( !it || !utils::is_digit( *it ));

    // Read the nucleotides, skipping everything that we don't want. We need to store these
    // in a string first, as we want to do quality checks.
    next_field_( it );
    sample.read_bases.reserve( sample.read_coverage );
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
            case '.': {
                sample.read_bases += utils::to_upper( record.reference_base );
                ++it;
                break;
            }
            case ',': {
                sample.read_bases += utils::to_lower( record.reference_base );
                ++it;
                break;
            }
            default: {
                sample.read_bases += c;
                ++it;
                break;
            }
        }
    }
    assert( !it || !utils::is_graph( *it ) );

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

    // Also check if we want to read the ancestral base, if present.
    if( with_ancestral_base_ ) {
        next_field_( it );
        // We can simply read in the char here. Even if the iterator is at its end, it will
        // simply return a null char, which will trigger the subsequent error check.
        char const c = utils::to_upper( *it );
        if( !it || ( c != 'A' && c != 'C' && c != 'G' && c != 'T' && c != 'N' )) {
            throw std::runtime_error(
                "Malformed pileup " + it.source_name() + " at " + it.at() +
                ": Expecting ancestral base character in [ACGTN]."
            );
        }
        sample.ancestral_base = c;
        ++it;
    }

    // Final file sanity checks.
    if( it && !( utils::is_blank( *it ) || utils::is_newline( *it ))) {
        throw std::runtime_error(
            "Malformed pileup " + it.source_name() + " at " + it.at() +
            ": Invalid characters."
        );
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
    utils::affirm_char_or_throw( input_stream, utils::is_blank );
    utils::skip_while( input_stream, utils::is_blank );
    assert( !input_stream || !utils::is_blank( *input_stream ));
}

} // namespace population
} // namespace genesis
