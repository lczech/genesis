/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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

#include "genesis/placement/formats/jplace_reader.hpp"
#include "genesis/placement/function/operators.hpp"
#include "genesis/placement/sample.hpp"

#include "genesis/tree/common_tree/newick_reader.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/mass_tree/balances.hpp"
#include "genesis/tree/mass_tree/functions.hpp"
#include "genesis/tree/mass_tree/phylo_ilr.hpp"

#include "genesis/utils/containers/matrix/operators.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/text/string.hpp"

#include <vector>

using namespace genesis;
using namespace placement;
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
    auto get_bin_pos = []( double pos, double bl )
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

TEST( MassTree, PhylogeneticILR )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read sample
    std::string const infile = environment->data_dir + "placement/rooted.jplace";
    auto const smp = JplaceReader().read( utils::from_file( infile ));

    // Prepare data. We deactivate taxon weighing.
    auto const tree = convert_sample_to_mass_tree( smp, false ).first;
    BalanceSettings bs;
    bs.tendency = BalanceSettings::WeightTendency::kNone;
    bs.norm     = BalanceSettings::WeightNorm::kNone;
    bs.pseudo_count_summand_all = 0.65;
    auto const data = mass_balance_data( tree, bs );

    EXPECT_TRUE( identical_topology( data.tree, tree ));
    EXPECT_TRUE( data.taxon_weights.empty() );
    EXPECT_EQ( 1, data.edge_masses.rows() );
    EXPECT_EQ( data.tree.edge_count(), data.edge_masses.cols() );

    // Calculate balances. The tree has four inner nodes with balances != 0.0
    // At the root however, both subtrees have exactly the same number of branches,
    // and there is a bijective mapping between the branches so that each pair of branches
    // has the same mass. In other words: the set of masses per branch in both subtrees of the root
    // is identical (although the positions of these masses in the subtrees differ).
    // Hence, the geom mean is the same, hence the balance is 0 for the root (node 0, first entry).
    // We did this to test this interesting special case.
    auto const bals = phylogenetic_ilr_transform( data );

    // Test
    auto const exp = std::vector<double>({
        0.0, 0.414042973972463, 0.335016128179832, 0.0, 0.0, 0.0,
        0.819730425503731, 0.335016128179832, 0.0, 0.0, 0.0
    });
    // EXPECT_EQ( exp, bals );

    EXPECT_EQ( 1, bals.rows() );
    EXPECT_EQ( exp.size(), bals.cols() );
    for( size_t i = 0; i < bals.cols(); ++i ) {
        // std::cout << std::setprecision (18) << bals[i] << "\n";
        // EXPECT_DOUBLE_EQ( exp[i], bals[i] );

        // We get slighly different results because of compiler floating point optimizations.
        // Let's say that 10 digits precision are good enough ;-)
        EXPECT_TRUE( utils::almost_equal_relative( exp[i], bals( 0, i ), 1e-10 ));

        std::cout << "exp " << exp[i] << " bals " << bals( 0, i ) << "\n";
    }
}
