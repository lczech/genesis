/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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

std::vector<PoolSampleSet> SyncReader::read(
    std::shared_ptr< utils::BaseInputSource > source
) const {
    std::vector<PoolSampleSet> result;
    utils::InputStream it( source );

    PoolSampleSet sample_set;
    while( parse_line( it, sample_set ) ) {
        result.push_back( std::move( sample_set ));
    }
    return result;
}

bool SyncReader::parse_line(
    utils::InputStream& input_stream,
    PoolSampleSet& sample_set
) const {
    // Shorthand.
    auto& it = input_stream;
    if( !it ) {
        return false;
    }

    // Read fixed columns
    sample_set.chromosome = utils::read_until( it, []( char c ){ return c == '\t' || c == '\n'; });
    utils::read_char_or_throw( it, '\t' );
    sample_set.position = utils::parse_unsigned_integer<size_t>( it );
    utils::read_char_or_throw( it, '\t' );
    if( !it || *it == '\n' ) {
        throw std::runtime_error(
            std::string("In ") + it.source_name() + ": Unexpected end of line at " + it.at()
        );
    }
    sample_set.reference_base = *it;
    ++it;

    // Read the samples. We switch once for the first line, and thereafter check that we read the
    // same number of samples each time.
    if( sample_set.samples.empty() ) {
        while( it && *it != '\n' ) {
            sample_set.samples.emplace_back();
            parse_sample_( it, sample_set.samples.back() );
        }
    } else {
        size_t index = 0;
        while( it && *it != '\n' ) {
            if( index >= sample_set.samples.size() ) {
                throw std::runtime_error(
                    "Malformed sync " + it.source_name() + " at " + it.at() +
                    ": Line with different number of samples."
                );
            }
            parse_sample_( it, sample_set.samples[ index ]);
            ++index;
        }
        if( index != sample_set.samples.size() ) {
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
    PoolSample&         sample
) const {
    using namespace genesis::utils;
    read_char_or_throw( input_stream, '\t' );

    // The allele frequencies are stored in the order `A:T:C:G:N:del`,
    // see https://sourceforge.net/p/popoolation2/wiki/Tutorial/
    sample.a_count = parse_unsigned_integer<size_t>( input_stream );
    read_char_or_throw( input_stream, ':' );
    sample.t_count = parse_unsigned_integer<size_t>( input_stream );
    read_char_or_throw( input_stream, ':' );
    sample.c_count = parse_unsigned_integer<size_t>( input_stream );
    read_char_or_throw( input_stream, ':' );
    sample.g_count = parse_unsigned_integer<size_t>( input_stream );
    read_char_or_throw( input_stream, ':' );
    sample.n_count = parse_unsigned_integer<size_t>( input_stream );
    read_char_or_throw( input_stream, ':' );
    sample.d_count = parse_unsigned_integer<size_t>( input_stream );
}

} // namespace population
} // namespace genesis
