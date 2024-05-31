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

#include "genesis/population/format/bed_reader.hpp"

#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Reading
// =================================================================================================

std::vector<BedReader::Feature> BedReader::read(
    std::shared_ptr< utils::BaseInputSource > source
) const {
    std::vector<BedReader::Feature> result;
    read_( source, [&]( Feature&& feat ){
        result.push_back( std::move( feat ));
    });
    return result;
}

GenomeLocusSet BedReader::read_as_genome_locus_set(
    std::shared_ptr< utils::BaseInputSource > source
) const {
    GenomeLocusSet result;
    read_( source, [&]( Feature&& feat ){
        result.add( feat.chrom, feat.chrom_start, feat.chrom_end );
    });
    return result;
}

GenomeRegionList BedReader::read_as_genome_region_list(
    std::shared_ptr< utils::BaseInputSource > source,
    bool merge
) const {
    GenomeRegionList result;
    read_as_genome_region_list( source, result, merge );
    return result;
}

void BedReader::read_as_genome_region_list(
    std::shared_ptr< utils::BaseInputSource > source,
    GenomeRegionList& target,
    bool merge
) const {
    read_( source, [&]( Feature&& feat ){
        target.add( feat.chrom, feat.chrom_start, feat.chrom_end, merge );
    });
}

// =================================================================================================
//     Internal Helpers
// =================================================================================================

void BedReader::read_(
    std::shared_ptr< utils::BaseInputSource > source,
    std::function<void(Feature&&)> callback
) const {
    utils::InputStream it( source );

    // We use an internal reading function that takes care of checking that the number of columns
    // is constant throughout the input file. This avoids code duplication for these checkes
    // in the two public read functions.

    Feature feat;
    size_t expected_columns = 0;
    size_t found_columns;
    while(( found_columns = parse_line_( it, feat ) )) {
        if( expected_columns == 0 ) {
            expected_columns = found_columns;
        } else if( expected_columns != found_columns ) {
            // Called with the stream at the next line already. Need to compensate for this.
            assert( it.line() > 0 );
            throw std::runtime_error(
                "Inconsistent number of columns in BED input. Expected " +
                std::to_string( expected_columns ) + " based on first row, but found " +
                std::to_string( found_columns ) + " in line " + std::to_string( it.line() - 1 )
            );
        }
        assert( found_columns == expected_columns );
        callback( std::move( feat ));
        feat = Feature();
    }
}

size_t BedReader::parse_line_(
    utils::InputStream& input_stream,
    BedReader::Feature& feature
) const {
    // Setup.
    size_t found_columns = 0;
    auto& it = input_stream;
    if( !it ) {
        return found_columns;
    }

    // The BED format unfortunately does not have a proper separation of the header lines,
    // and instead just depends on non-standard keywords in the beginning of lines...
    // Bit weird to parse, but okay. Also, it is unclear if `track` can appear anywhere in a file
    // or just in the beginning... So, we just allow for all of that, and test for such comments
    // in every line.
    bool is_comment = true;
    std::string first_word;
    while( is_comment ) {
        first_word = parse_string_( it );
        if(
            first_word == "browser" ||
            first_word == "track" ||
            utils::starts_with( first_word, "#" )
        ) {
            // Read until the end of the comment line; we just ignore it.
            it.get_line();
        } else {
            is_comment = false;
        }
    }
    if( !it ) {
        // Comments at the end of the file. Just finish here.
        return found_columns;
    }

    // Read chrom and start and end coordinates. These are the mandatory ones in bed.
    // Chrom will simply be the first entry after the header.
    feature.chrom = first_word;
    if( ! next_field_( it, found_columns )) {
        throw std::runtime_error(
            "BED input expected to have three mandatory columns chrom,start,end in the beginning "
            "of the line, but only chrom was found at " + it.at()
        );
    }
    feature.chrom_start = utils::parse_unsigned_integer<size_t>( it ) + 1;
    if( ! next_field_( it, found_columns )) {
        throw std::runtime_error(
            "BED input expected to have three mandatory columns chrom,start,end in the beginning "
            "of the line, but only chrom and start were found at " + it.at()
        );
    }
    feature.chrom_end = utils::parse_unsigned_integer<size_t>( it );

    // From now on we need to check before every field if there is more data...

    // name
    if( ! next_field_( it, found_columns )) {
        return found_columns;
    }
    feature.name = parse_string_( it );

    // score
    if( ! next_field_( it, found_columns )) {
        return found_columns;
    }
    feature.score = utils::parse_unsigned_integer<size_t>( it );
    if( feature.score > 1000 ) {
        throw std::runtime_error( "Invalid score > 1000 in BED input at " + it.at() );
    }

    // strand
    if( ! next_field_( it, found_columns )) {
        return found_columns;
    }
    feature.strand = utils::read_char_or_throw( input_stream, []( char c ){
        return c == '+' || c == '-' || c == '.';
    });

    // thick_start. Need to adjust for 0-based again.
    if( ! next_field_( it, found_columns )) {
        return found_columns;
    }
    feature.thick_start = utils::parse_unsigned_integer<size_t>( it ) + 1;

    // thick_end
    if( ! next_field_( it, found_columns )) {
        return found_columns;
    }
    feature.thick_end = utils::parse_unsigned_integer<size_t>( it );

    // item_rgb
    if( ! next_field_( it, found_columns )) {
        return found_columns;
    }
    feature.item_rgb = parse_string_( it );

    // block_count
    if( ! next_field_( it, found_columns )) {
        return found_columns;
    }
    feature.block_count = utils::parse_unsigned_integer<size_t>( it );

    // block_sizes
    if( ! next_field_( it, found_columns )) {
        return found_columns;
    }
    auto block_sizes = utils::split( parse_string_( it ), "," );
    for( auto const& bs : block_sizes ) {
        if( ! bs.empty() ) {
            feature.block_sizes.push_back( stoull( bs ));
        }
    }
    if( feature.block_sizes.size() != feature.block_count ) {
        throw std::runtime_error(
            "Invalid blockSizes length in BED input. Expected " +
            std::to_string( feature.block_count ) + " based on blockCount, but found " +
            std::to_string( feature.block_sizes.size() ) + " values instead, at " + it.at()
        );
    }

    // block_starts
    if( ! next_field_( it, found_columns )) {
        return found_columns;
    }
    auto block_starts = utils::split( parse_string_( it ), "," );
    for( auto const& bs : block_sizes ) {
        if( ! bs.empty() ) {
            feature.block_starts.push_back( stoull( bs ));
        }
    }
    if( feature.block_starts.size() != feature.block_count ) {
        throw std::runtime_error(
            "Invalid blockStarts length in BED input. Expected " +
            std::to_string( feature.block_count ) + " based on blockCount, but found " +
            std::to_string( feature.block_starts.size() ) + " values instead, at " + it.at()
        );
    }

    // all remaining (unsupported, but ignored) columns
    while( next_field_( it, found_columns ) ) {
        parse_string_( it );
    }

    // next_field_() already takes care of jumping to the next line, if there is one.
    // So, nothing more to do here.
    return found_columns;
}

bool BedReader::next_field_( utils::InputStream& input_stream, size_t& found_columns ) const
{
    // Throws for anything that is not a new line or field delimiter (tab or space).
    // We expect at least one delimiter, and then skip any more. It is not clear from
    // the format if that is the right way, or if empty fields are possible, but that
    // seems how it is supposed to be...
    // Returns true iff a next field was delimited and starts there.
    if( ! input_stream || *input_stream == '\n' ) {
        assert( !input_stream || *input_stream == '\n' );
        ++input_stream;
        return false;
    }
    assert( input_stream && *input_stream != '\n' );
    utils::read_char_or_throw( input_stream, []( char c ){
        return c == '\t' || c == ' ';
    });
    utils::skip_while( input_stream, []( char c ){
        return c == '\t' || c == ' ';
    });
    if( ! input_stream || *input_stream == '\n' ) {
        throw std::runtime_error( "Unexpected end of BED input at " + input_stream.at() );
    }
    ++found_columns;
    return true;
}

std::string BedReader::parse_string_( utils::InputStream& input_stream ) const
{
    // We use \n as stopping criterion here as well, so that in case of an error,
    // we at least report the error in the correct line.
    return utils::read_while( input_stream, []( char c ){
        return c != '\t' && c != ' ' && c != '\n';
    });
}

} // namespace population
} // namespace genesis
