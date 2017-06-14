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

#include "genesis/tree/mass_tree/functions.hpp"
#include "genesis/utils/math/common.hpp"

#include <vector>

using namespace genesis;
using namespace tree;

TEST( MassTree, Binify )
{
    size_t const number_of_bins = 6;
    double const bl = 3.6;

    std::vector<double> bins = {
        0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.9, 0.9, 0.9, 1.5,
        1.5, 1.5, 2.1, 2.1, 2.1, 2.7, 2.7, 2.7, 3.3, 3.3, 3.3, 3.3, 3.3
    };

    // Copied here from lib/genesis/tree/mass_tree/functions.cpp
    // Ugly, but I did not want to make it a stand alone function.
    auto get_bin_pos = [ number_of_bins ]( double pos, double bl )
    {
        auto const nb = static_cast<double>( number_of_bins );

        // Scale pos to be in interval [ 0.0, nb )
        auto const pn = std::min( std::max( 0.0, pos / bl * nb ), std::nextafter( nb, 0.0 ));

        // Floor it to get to exact bin, then scale back, and add half the interval,
        // so that we end up at the mid point of the interval.
        return ( std::floor( pn ) * bl / nb ) + ( bl / nb / 2.0 );
    };

    for( int i = -6; i < 20; ++i ) {
        auto const pos = static_cast<double>( i ) / 5.0;
        auto const bin = get_bin_pos( pos, bl );
        auto const exp = bins[ i + 6 ];
        EXPECT_TRUE( utils::almost_equal_relative( exp, bin ));

        // LOG_DBG << "i = " << i << "\tpos = " << pos << " \tbin = " << bin;
    }
}
