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

#include "genesis/population/format/gff_reader.hpp"

#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/text/char.hpp"

#include "genesis/utils/core/logging.hpp"

#include <cassert>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Reading
// =================================================================================================

std::vector<GffReader::Feature> GffReader::read(
    std::shared_ptr< utils::BaseInputSource > source
) const {
    std::vector<GffReader::Feature> result;
    utils::InputStream it( source );
    Feature feat;
    while( parse_line( it, feat ) ) {
        result.push_back( std::move( feat ));
    }
    return result;
}

GenomeLocusSet GffReader::read_as_genome_locus_set(
    std::shared_ptr< utils::BaseInputSource > source
) const {
    GenomeLocusSet result;
    utils::InputStream it( source );
    Feature feat;
    while( parse_line( it, feat ) ) {
        result.add( feat.seqname, feat.start, feat.end );
    }
    return result;
}

GenomeRegionList GffReader::read_as_genome_region_list(
    std::shared_ptr< utils::BaseInputSource > source,
    bool merge
) const {
    GenomeRegionList result;
    read_as_genome_region_list( source, result, merge );
    return result;
}

void GffReader::read_as_genome_region_list(
    std::shared_ptr< utils::BaseInputSource > source,
    GenomeRegionList& target,
    bool merge
) const {
    utils::InputStream it( source );
    Feature feat;
    while( parse_line( it, feat ) ) {
        target.add( feat.seqname, feat.start, feat.end, merge );
    }
}

// =================================================================================================
//     Parsing
// =================================================================================================

bool GffReader::parse_line(
    utils::InputStream& input_stream,
    GffReader::Feature& feature
) const {
    // Shorthand.
    auto& it = input_stream;
    if( !it ) {
        return false;
    }

    // In the following, whenever we use one of the utils stream reading functions, those
    // also check for end of stream and throw whenever their expected format is not found
    // (e.g., when parsing a number, there has to be a number - empty/end of stream will throw).
    // So, in all these cases, we do not need additional checks here; we hence only add checks
    // for end of stream etc when we read the chars ourselves here (e.g., for the strand).

    // Track lines are ignored. We look for the word "track " with a space.
    // If found, we skip the line.
    while( true ) {
        auto const buff = it.buffer();
        if(
            ( buff.second >= 6 && strncmp( buff.first, "track ",   6 ) == 0 ) ||
            ( buff.second >= 8 && strncmp( buff.first, "browser ", 8 ) == 0 ) ||
            ( *it == '#' || *it == '\n' )
        ) {
            it.get_line();
        } else {
            break;
        }
    }

    // Read seqname, source, and feature.
    // We use \n as stopping criterion here as well, so that in case of an error,
    // we at least report the error in the correct line.
    feature.seqname = utils::read_while( it, []( char c ){ return c != '\t' && c != '\n'; });
    utils::read_char_or_throw( it, '\t' );
    feature.source  = utils::read_while( it, []( char c ){ return c != '\t' && c != '\n'; });
    utils::read_char_or_throw( it, '\t' );
    feature.feature = utils::read_while( it, []( char c ){ return c != '\t' && c != '\n'; });
    utils::read_char_or_throw( it, '\t' );

    // Read start and end
    feature.start = utils::parse_unsigned_integer<size_t>( it );
    utils::read_char_or_throw( it, '\t' );
    feature.end   = utils::parse_unsigned_integer<size_t>( it );
    utils::read_char_or_throw( it, '\t' );

    // Read score, allowing for it to be empty
    if( it && *it == '.' ) {
        feature.score = std::numeric_limits<double>::quiet_NaN();
        ++it;
    } else {
        feature.score = utils::parse_float<double>( it );
    }
    utils::read_char_or_throw( it, '\t' );

    // Read strand
    if( !it ) {
        throw std::runtime_error(
            std::string("In ") + it.source_name() + ": Unexpected end of input at " + it.at()
        );
    }
    feature.strand = *it;
    ++it;
    utils::read_char_or_throw( it, '\t' );

    // Read frame, allowing for it to be empty
    if( it && *it == '.' ) {
        feature.frame = -1;
        ++it;
    } else {
        feature.frame = utils::parse_unsigned_integer<signed char>( it );
    }

    // There might be no attributes, and the line might end early.
    // Otherwise, there needs to be a tab befor the attributes.
    if( !it || *it == '\n' ) {
        ++it;
        return true;
    }
    utils::read_char_or_throw( it, '\t' );

    // Read attributes. GFF2, GFF3, and GTF are slightly different, it seems... Bioinformatics...
    // We just read all of them into a single string.
    feature.attributes_group = it.get_line();

    // Old code that works for GFF3 and GTF, but not for GFF2. Kept for future refinement.
    // GFF2 has just a simple string at the end, while GFF3 has `=` between key and value,
    // and GTF uses space, and with a semicolon at the end. We here allow for all these variants.
    // GFF: `hid=trf; hstart=1; hend=21`
    // GTF: `gene_id "ENSG00000223972"; gene_name "DDX11L1";`
    // while( it && *it != '\n' ) {
    //     // Read key
    //     utils::skip_while( it, ' ' );
    //     std::string key = utils::read_while( it, []( char c ){
    //         return c != '=' && c != ' ' && c != '\n';
    //     });
    //     utils::read_char_or_throw( it, []( char c ){ return c == '=' || c == ' '; });
    //     if( !it || *it == '\n' ) {
    //         throw std::runtime_error(
    //             std::string("In ") + it.source_name() +
    //             ": Unexpected end of line after attribute key at " + it.at()
    //         );
    //     }
    //
    //     // Read value
    //     std::string value;
    //     assert( it );
    //     if( *it == '"' ) {
    //         value = utils::parse_quoted_string( it );
    //     } else {
    //         value = utils::read_while( it, []( char c ){ return c != ';' && c != '\n'; });
    //     }
    //
    //     // Store key value pair
    //     feature.attributes.emplace_back( std::move( key ), std::move( value ));
    //
    //     // The attributes end with a closing `;`, which we just want to skip
    //     if( it && *it == ';' ) {
    //         ++it;
    //     }
    //     utils::skip_while( it, ' ' );
    // }
    // assert( !it || *it == '\n' );
    // ++it;

    return true;
}

} // namespace population
} // namespace genesis
