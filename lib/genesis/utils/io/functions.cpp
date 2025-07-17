/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
 * @ingroup utils
 */

#include <genesis/utils/io/functions.hpp>

#include <genesis/utils/io/input_buffer.hpp>
#include <genesis/utils/io/input_stream.hpp>

#include <cassert>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace utils {
namespace io {

// =================================================================================================
//     Read Input Source
// =================================================================================================

std::string read_input_source( std::shared_ptr<BaseInputSource> source )
{
    std::string result;

    // We buffer into a char array, and copy from there.
    // Can surely be optimized, but good enough for now.
    // We use the same block lenght for the buffer here
    // as the underlying input buffer, hopefully for speed.
    auto ib = InputBuffer( source );
    char cb[InputBuffer::BlockLength];
    while( true ) {
        // Read a block and append it to the result.
        auto cnt = ib.read( cb, InputBuffer::BlockLength );
        result.append( cb, cnt );

        // If we did not get a full block, the input is done.
        if( cnt != InputBuffer::BlockLength ) {
            break;
        }
    }
    return result;
}

std::vector<std::string> read_input_source_lines( std::shared_ptr<BaseInputSource> source )
{
    std::vector<std::string> result;
    genesis::utils::io::InputStream it( source );
    while( it ) {
        result.push_back( it.get_line() );
    }
    return result;
}

} // namespace io
} // namespace utils
} // namespace genesis
