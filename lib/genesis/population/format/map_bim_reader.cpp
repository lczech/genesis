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
 * @ingroup population
 */

#include "genesis/population/format/map_bim_reader.hpp"

#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/convert.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Reading
// =================================================================================================

std::vector<MapBimReader::Feature> MapBimReader::read(
    std::shared_ptr< utils::BaseInputSource > source
) const {
    std::vector<MapBimReader::Feature> result;
    read_( source, [&]( Feature&& feat ){
        result.push_back( std::move( feat ));
    });
    return result;
}

GenomeLocusSet MapBimReader::read_as_genome_locus_set(
    std::shared_ptr< utils::BaseInputSource > source
) const {
    GenomeLocusSet result;
    read_( source, [&]( Feature&& feat ){
        result.add( feat.chromosome, feat.coordinate, feat.coordinate );
    });
    return result;
}

GenomeRegionList MapBimReader::read_as_genome_region_list(
    std::shared_ptr< utils::BaseInputSource > source,
    bool merge
) const {
    GenomeRegionList result;
    read_as_genome_region_list( source, result, merge );
    return result;
}

void MapBimReader::read_as_genome_region_list(
    std::shared_ptr< utils::BaseInputSource > source,
    GenomeRegionList& target,
    bool merge
) const {
    read_( source, [&]( Feature&& feat ){
        target.add( feat.chromosome, feat.coordinate, feat.coordinate, merge );
    });
}

// =================================================================================================
//     Internal Helpers
// =================================================================================================

void MapBimReader::read_(
    std::shared_ptr< utils::BaseInputSource > source,
    std::function<void(Feature&&)> callback
) const {
    utils::InputStream it( source );

    // We use an internal reading function that takes care of checking that the number of columns
    // is constant throughout the input file. This avoids code duplication for these checkes
    // in the two public read functions. However, this is kind of code duplication from the bed
    // reader... not much to do about this right now. The formats are distinct enough that
    // refactoring this into some common reader functionality is not worth it at this time.

    Feature feat;
    std::vector<std::string> buffer;
    size_t expected_columns = 0;
    size_t found_columns;
    while(( found_columns = parse_line_( it, feat, buffer ) )) {
        if( expected_columns == 0 ) {
            expected_columns = found_columns;
        } else if( expected_columns != found_columns ) {
            // Called with the stream at the next line already. Need to compensate for this.
            assert( it.line() > 0 );
            throw std::runtime_error(
                "Inconsistent number of columns in map/bim input. Expected " +
                std::to_string( expected_columns ) + " based on first row, but found " +
                std::to_string( found_columns ) + " in line " + std::to_string( it.line() - 1 )
            );
        }
        assert( found_columns == expected_columns );

        // Throw on coordinate 0 as a weird special case.
        if( feat.coordinate == 0 ) {
            // Same line number adjustment as above.
            assert( it.line() > 0 );
            throw std::runtime_error(
                "Invalid base-pair coordinate 0 in map/bim input in line " +
                std::to_string( it.line() - 1 )
            );
        }

        // Here we apply the skip_negative_coordinates_ setting.
        if( !( feat.coordinate < 0 && skip_negative_coordinates_ )) {
            callback( std::move( feat ));
        }
        feat = Feature();
    }
}

size_t MapBimReader::parse_line_(
    utils::InputStream& input_stream,
    MapBimReader::Feature& feature,
    std::vector<std::string>& buffer
) const {
    // Setup.
    size_t found_columns = 0;
    auto& it = input_stream;
    if( !it ) {
        return found_columns;
    }

    // Unfortunately, the format has an optional column in the middle, so we need to know the final
    // number of coumns first before we can start parsing them into specific data types...
    // So, first, we need to parse all values of the line into the buffer.
    while( it && *it != '\n' ) {
        // Make sure that the buffer is large enough.
        // This does multiple consecutive resizes for the first line, when we do not yet know
        // how many columns there are in the file, but then none after
        // (unless there is a mismatch in number of columns, which is an exception anyway).
        if( buffer.size() < found_columns + 1 ) {
            buffer.resize( found_columns + 1 );
        }

        // Read into the buffer until the next tab or end of line.
        buffer[found_columns] = utils::read_while( it, []( char c ){
            return c != '\t' && c != '\n';
        });
        if( buffer[found_columns].empty() ) {
            throw std::runtime_error( "Invalid empty entry of map/bim input at " + it.at() );
        }
        ++found_columns;

        // Skip the separator, if there is one; in that case, we also expect that data follows.
        if( it && *it == '\t' ) {
            ++it;
            if( ! it || *it == '\n' ) {
                throw std::runtime_error( "Unexpected end of map/bim input at " + it.at() );
            }
        }
    }
    assert( found_columns <= buffer.size() );

    // We are done with the line, move to the next, if there is one.
    if( it ) {
        assert( *it == '\n' );
        ++it;
    }

    // If we have fewer columns that the buffer, that means the buffer was resized in a
    // previous iteration to some larger value, which means that the current line is too short.
    // We can skip the parsing here, as this will lead to an exception in read_() anyway.
    if( found_columns < buffer.size() ) {
        return found_columns;
    }
    assert( found_columns == buffer.size() );

    // Validity check.
    if( found_columns < 3 || found_columns > 6 ) {
        throw std::runtime_error(
            "Invalid number of columns (" + std::to_string(found_columns) +
            " found, but 3-6 expected) of map/bim input at " + it.at()
        );
    }

    // The first two columns are always there. We already checked above that there are 3-6 entries
    // in the buffer, so we only assert here.
    assert( found_columns > 2 );
    assert( buffer.size() > 2 );
    assert( found_columns == buffer.size() );
    feature.chromosome = buffer[0];
    feature.variant_id = buffer[1];

    // Helper functions to avoid code repetition for the value parsing.
    auto get_position_ = [&]( std::string const& value ){
        double result;
        if( ! utils::convert_to_double( value, result )) {
            throw std::runtime_error(
                "Invalid map/bim input with (centi)morgan position that is not a numeric value (\"" +
                value + "\") at " + it.at()
            );
        }
        return result;
    };
    auto get_coordinate_ = [&]( std::string const& value ){
        long long result;
        if( ! utils::convert_to_signed_integer( value, result )) {
            throw std::runtime_error(
                "Invalid map/bim input with base pair coordinate that is not a numeric value (\"" +
                value + "\") at " + it.at()
            );
        }
        return result;
    };
    auto get_allele_ = [&]( std::string const& value ){
        if( value.size() != 1 ) {
            throw std::runtime_error(
                "Invalid map/bim input with allele that is not a single char (\"" +
                value + "\") at " + it.at()
            );
        }
        assert( value.size() == 1 );
        return value[0];
    };

    // Now we know how many columns we have read, and can parse them adequately.
    // We only do the variable columns here, and the first two, that are always present, later.
    assert( found_columns == buffer.size() );
    assert( found_columns >= 3 && found_columns <= 6 );
    switch( found_columns ) {
        case 3: {
            // map file without (centi)morgan position, just coordinate.
            feature.coordinate = get_coordinate_( buffer[2] );
            break;
        }
        case 4: {
            // map file with (centi)morgan position.
            feature.position = get_position_( buffer[2] );
            feature.coordinate = get_coordinate_( buffer[3] );
            break;
        }
        case 5: {
            // bim file without (centi)morgan position, just coordinate, and alleles.
            feature.coordinate = get_coordinate_( buffer[2] );
            feature.allele_1 = get_allele_( buffer[3] );
            feature.allele_2 = get_allele_( buffer[4] );
            break;
        }
        case 6: {
            // bim file with (centi)morgan position.
            feature.position = get_position_( buffer[2] );
            feature.coordinate = get_coordinate_( buffer[3] );
            feature.allele_1 = get_allele_( buffer[4] );
            feature.allele_2 = get_allele_( buffer[5] );
            break;
        }
        default: {
            // Cannot happen, we checked already.
            assert( false );
        }
    }

    return found_columns;
}

} // namespace population
} // namespace genesis
