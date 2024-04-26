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

#include "genesis/population/format/genome_region_reader.hpp"

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

GenomeLocusSet GenomeRegionReader::read_as_genome_locus_set(
    std::shared_ptr< utils::BaseInputSource > source
) const {
    GenomeLocusSet result;
    read_( source, [&]( GenomeRegion const& region ){
        result.add( region );
    });
    return result;
}

GenomeRegionList GenomeRegionReader::read_as_genome_region_list(
    std::shared_ptr< utils::BaseInputSource > source,
    bool merge
) const {
    GenomeRegionList result;
    read_as_genome_region_list( source, result, merge );
    return result;
}

void GenomeRegionReader::read_as_genome_region_list(
    std::shared_ptr< utils::BaseInputSource > source,
    GenomeRegionList& target,
    bool merge
) const {
    read_( source, [&]( GenomeRegion const& region ){
        target.add( region, merge );
    });
}

// =================================================================================================
//     Internal Helpers
// =================================================================================================

void GenomeRegionReader::read_(
    std::shared_ptr< utils::BaseInputSource > source,
    std::function<void( GenomeRegion const& region )> callback
) const {
    using namespace genesis::utils;
    InputStream it( source );

    // Helper function to throw on error without copies of the same error message each time.
    auto throw_invalid_arg_ = [&]( InputStream const& it ){
        throw std::invalid_argument(
            "Invalid genomic region in " + it.source_name() + " at " + it.at()
        );
    };

    // Read the file; each loop iteration handles one line.
    while( it ) {
        GenomeRegion region;

        // Read the chromosome until the first delimiter is found.
        // The is_print function is false for tabs, but then we want to stop anyway, so that's okay.
        region.chromosome = read_while( it, []( char c ){
            return is_print(c) && c != ':' && c != ' ' && c != '\t' && c != '\n';
        });
        if( region.chromosome.empty() || region.chromosome == "-" || region.chromosome == ".." ) {
            throw_invalid_arg_( it );
        }

        // We only allow certain combinations of delimiters, `:` with `-` or `..`,
        // or white space, but not mixed. So here, we take note of which delimiter we found first.
        bool colon = true;
        if( !it || *it == '\n' ) {
            // No information on positions, so mark it as using the whole chromosome.
            region.start = 0;
            region.end = 0;
            callback( region );

            // End of the line, move to next. Also works on past the end, no special case needed.
            ++it;
            continue;
        } else if( *it == ':' ) {
            colon = true;
        } else if( *it == ' ' || *it == '\t' ) {
            colon = false;
        } else {
            throw_invalid_arg_( it );
        }
        ++it;

        // Now we expect to read the start position. Throws when there is no number.
        region.start = parse_unsigned_integer<size_t>( it );

        if( !it || *it == '\n' ) {
            // We are at the end of the line already - no end position specified.
            // That means, it's a single position.
            region.end = region.start;
        } else {
            // End position is specified.
            // Check that the correct delimiters are present, and skip them.
            if( *it == '-' || *it == '.' ) {
                if( ! colon ) {
                    throw_invalid_arg_( it );
                }
                if( *it == '.' ) {
                    ++it;
                    if( !it || *it != '.' ) {
                        throw_invalid_arg_( it );
                    }
                }
            } else if( *it == ' ' || *it == '\t' ) {
                if( colon ) {
                    throw_invalid_arg_( it );
                }
            } else {
                throw_invalid_arg_( it );
            }
            ++it;

            // Now read the end position. Throws when there is no number.
            region.end = parse_unsigned_integer<size_t>( it );
        }

        // After the above, we must be at the end of the line, otherwise that's an error.
        if( it && *it != '\n' ) {
            throw_invalid_arg_( it );
        }

        // Fix coordinates as needed.
        if( zero_based_ ) {
            ++region.start;
            ++region.end;
        }
        if( end_exclusive_ ) {
            if( region.end == 0 ) {
                throw_invalid_arg_( it );
            }
            --region.end;
        }

        // Validity check.
        if( ! region.specified() ) {
            throw_invalid_arg_( it );
        }

        // Call the callback, e.g., to add the region to the target list.
        callback( region );

        // Move to next line.
        assert( !it || *it == '\n'  );
        ++it;
    }
}

} // namespace population
} // namespace genesis
