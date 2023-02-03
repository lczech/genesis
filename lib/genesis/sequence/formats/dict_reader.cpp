/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
 * @ingroup sequence
 */

#include "genesis/sequence/formats/dict_reader.hpp"

#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/convert.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Reading
// =================================================================================================

SequenceDict DictReader::read( std::shared_ptr< utils::BaseInputSource > source ) const
{
    // Prepare the result and the input stream.
    SequenceDict result;
    utils::InputStream it( source );

    // Read lines while there is data.
    // We don't need to be super efficient here, dict files typically only contain a few dozen lines.
    size_t line_cnt = 0;
    while( it ) {
        ++line_cnt;

        // Get the line and split it on tabs; do some basic format sanity checks.
        auto const line = utils::split( it.get_line(), "\t" );
        if( line.size() == 0 ) {
            continue;
        }
        if( line[0].length() != 3 || line[0][0] != '@' ) {
            throw std::runtime_error(
                "Invalid sequence dict file: Line " + std::to_string( line_cnt ) +
                " does not start with a header record type code '@XX', but with '" + line[0] + "'."
            );
        }

        // Now we know that we have a valid line. Let's see if it's one that we are intersted in.
        assert( line.size() > 0 );
        assert( line[0].size() == 3 );
        assert( line[0][0] == '@' );
        if( line[0] != "@SQ" ) {
            continue;
        }

        // Now we can go through the fields of the line, and find the TAG:VALUE pairs that we want.
        std::string sn;
        size_t ln = 0;
        for( size_t i = 1; i < line.size(); ++i ) {
            auto const& field = line[i];
            if( field.size() < 3 || field[2] != ':' ) {
                throw std::runtime_error(
                    "Invalid sequence dict file: Line " + std::to_string( line_cnt ) +
                    " contains an @SQ record that is not of the form 'TAG:VALUE', "
                    "but '" + field + "'."
                );
            }
            assert( field.size() >=3 );
            assert( field[2] == ':' );

            // Get the two fields we are itnerested in.
            if( utils::starts_with( field, "SN:" )) {
                sn = field.substr( 3 );
            }
            if( utils::starts_with( field, "LN:" )) {
                if( ! utils::is_convertible_to_unsigned_integer( field.substr( 3 ) )) {
                    throw std::runtime_error(
                        "Invalid sequence dict file: Line " + std::to_string( line_cnt ) +
                        " contains an @SQ record with a field for the sequence length LN"
                        " whose VALUE is not a number, but '" + field.substr( 3 ) + "'."
                    );
                }
                ln = utils::convert_to_unsigned_integer( field.substr( 3 ));
            }
        }

        // We are a bit pedantic here, and throw in case of empty results.
        // Technically, those could actually occur in the data, but that would be some weird edge
        // case anyway, that would lead to other problems downstream.
        if( sn.empty() || ln == 0 ) {
            throw std::runtime_error(
                "Invalid sequence dict file: Line " + std::to_string( line_cnt ) +
                " contains an @SQ record with no valid SN or LN fields."
            );
        }

        // Now we are good; add to the list.
        result.add( sn, ln );
    }

    return result;
}


} // namespace sequence
} // namespace genesis
