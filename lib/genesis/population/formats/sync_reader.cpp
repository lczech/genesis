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

#include "genesis/population/formats/sync_reader.hpp"

#include "genesis/utils/io/char.hpp"
#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/io/scanner.hpp"

#include <cassert>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Reading & Parsing
// =================================================================================================

std::vector<Variant> SyncReader::read(
    std::shared_ptr< utils::BaseInputSource > source
) const {
    std::vector<Variant> result;
    utils::InputStream it( source );

    Variant sample_set;
    while( parse_line_( it, sample_set, {}, false )) {
        result.push_back( std::move( sample_set ));
        sample_set = Variant{};
    }
    return result;
}

std::vector<Variant> SyncReader::read(
    std::shared_ptr< utils::BaseInputSource > source,
    std::vector<bool> const&                  sample_filter
) const {
    std::vector<Variant> result;
    utils::InputStream it( source );

    Variant sample_set;
    while( parse_line_( it, sample_set, sample_filter, true )) {
        result.push_back( std::move( sample_set ));
        sample_set = Variant{};
    }
    return result;
}

bool SyncReader::parse_line(
    utils::InputStream& input_stream,
    Variant&            sample_set
) const {
    return parse_line_( input_stream, sample_set, {}, false );
}

bool SyncReader::parse_line(
    utils::InputStream&      input_stream,
    Variant&                 sample_set,
    std::vector<bool> const& sample_filter
) const {
    return parse_line_( input_stream, sample_set, sample_filter, true );
}

// =================================================================================================
//     Internal Parsing
// =================================================================================================

bool SyncReader::parse_line_(
    utils::InputStream&      input_stream,
    Variant&                 sample_set,
    std::vector<bool> const& sample_filter,
    bool                     use_sample_filter
) const {
    using namespace genesis::utils;

    // Shorthand.
    auto& it = input_stream;
    if( !it ) {
        return false;
    }

    // Read fixed columns for chromosome and position.
    sample_set.chromosome = utils::read_until( it, []( char c ){ return c == '\t' || c == '\n'; });
    it.read_char_or_throw( '\t' );
    sample_set.position = it.parse_unsigned_integer<size_t>();
    it.read_char_or_throw( '\t' );
    if( !it || *it == '\n' ) {
        throw std::runtime_error(
            std::string("In ") + it.source_name() + ": Unexpected end of line at " + it.at()
        );
    }

    // Read and check fixed column for the refererence base.
    auto const rb = to_upper( *it );
    if( rb != 'A' && rb != 'C' && rb != 'G' && rb != 'T' && rb != 'N' && rb != '.' && rb != '*' ) {
        throw std::runtime_error(
            std::string("In ") + it.source_name() + ": Invalid reference base char " +
            char_to_hex(rb) + " at " + it.at()
        );
    }
    sample_set.reference_base = rb;
    ++it;

    // Read the samples. We switch once for the first line, and thereafter check that we read the
    // same number of samples each time.
    if( sample_set.samples.empty() ) {
        size_t src_index = 0;
        while( it && *it != '\n' ) {
            if( !use_sample_filter || ( src_index < sample_filter.size() && sample_filter[src_index] )) {
                sample_set.samples.emplace_back();
                parse_sample_( it, sample_set.samples.back() );
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
            // If the numbers do not match, go straight to the error check and throw.
            if( dst_index >= sample_set.samples.size() ) {
                break;
            }

            // Parse or skip, depending on filter.
            if( !use_sample_filter || ( src_index < sample_filter.size() && sample_filter[src_index] )) {
                assert( dst_index < sample_set.samples.size() );
                parse_sample_( it, sample_set.samples[dst_index] );
                ++dst_index;
            } else {
                skip_sample_( it );
            }
            ++src_index;
        }

        // Need to have the exact size of samples in the line.
        if( dst_index != sample_set.samples.size() ) {
            throw std::runtime_error(
                "Malformed sync " + it.source_name() + " at " + it.at() +
                ": Line with different number of samples."
            );
        }
    }

    assert( !it || *it == '\n' );
    ++it;
    return true;
}

void SyncReader::parse_sample_(
    utils::InputStream& input_stream,
    BaseCounts&         sample
) const {
    using namespace genesis::utils;
    auto& it = input_stream;
    it.read_char_or_throw( '\t' );

    // The allele frequencies are stored in the order `A:T:C:G:N:del`,
    // see https://sourceforge.net/p/popoolation2/wiki/Tutorial/
    sample.a_count = it.parse_unsigned_integer<size_t>();
    it.read_char_or_throw( ':' );
    sample.t_count = it.parse_unsigned_integer<size_t>();
    it.read_char_or_throw( ':' );
    sample.c_count = it.parse_unsigned_integer<size_t>();
    it.read_char_or_throw( ':' );
    sample.g_count = it.parse_unsigned_integer<size_t>();
    it.read_char_or_throw( ':' );
    sample.n_count = it.parse_unsigned_integer<size_t>();
    it.read_char_or_throw( ':' );
    sample.d_count = it.parse_unsigned_integer<size_t>();
}

void SyncReader::skip_sample_(
    utils::InputStream& input_stream
) const {
    using namespace genesis::utils;

    // The skip functions are slow, because they need char by char access to the input stream.
    // Need to fix this at some point. For now, just read into an unused dummy.
    BaseCounts dummy;
    parse_sample_( input_stream, dummy );

    // Simply skip everything.
    // input_stream.read_char_or_throw( '\t' );
    // skip_while( input_stream, is_digit );
    // input_stream.read_char_or_throw( ':' );
    // skip_while( input_stream, is_digit );
    // input_stream.read_char_or_throw( ':' );
    // skip_while( input_stream, is_digit );
    // input_stream.read_char_or_throw( ':' );
    // skip_while( input_stream, is_digit );
    // input_stream.read_char_or_throw( ':' );
    // skip_while( input_stream, is_digit );
    // input_stream.read_char_or_throw( ':' );
    // skip_while( input_stream, is_digit );
}

} // namespace population
} // namespace genesis
