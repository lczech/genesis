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
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/formats/bmp/writer.hpp"
#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/color/color.hpp"

using namespace genesis::utils;

// -------------------------------------------------------------------------
//     BMP Writer
// -------------------------------------------------------------------------

TEST( Bmp, Writer )
{
    // Some weird sizes to test the padding.
    size_t const rows = 185;
    size_t const cols = 213;

    auto bunt = Matrix<Color>( rows, cols );

    for( size_t y = 0; y < rows; ++y ) {
        for( size_t x = 0; x < cols; ++x ) {
            bunt( y, x ) = Color( x / cols, y / rows, 0 );
        }
    }

    std::ostringstream ost;
    auto writer = BmpWriter();
    // writer.to_file( bunt, "/home/lucas/bunt.bmp" );
    writer.write( bunt, to_stream( ost ));
}
