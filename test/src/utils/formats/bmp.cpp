/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/formats/bmp/writer.hpp"
#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/tools/color.hpp"

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
            bunt( y, x ) = Color( 255 * x / cols, 255 * y / rows, 0 );
        }
    }

    std::ostringstream ost;
    auto writer = BmpWriter();
    // writer.to_file( bunt, "/home/lucas/bunt.bmp" );
    writer.to_stream( bunt, ost );
}
