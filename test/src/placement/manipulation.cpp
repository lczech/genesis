/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech, Pierre Barbera and HITS gGmbH

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
 * @brief Testing Newick class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <string>

#include "genesis/placement/function/manipulation.hpp"

#include "genesis/placement/sample.hpp"
#include "genesis/placement/formats/jplace_reader.hpp"
#include "genesis/placement/function/helper.hpp"

#include "genesis/tree/function/manipulation.hpp"
#include "genesis/tree/common_tree/functions.hpp"
#include "genesis/tree/common_tree/newick_reader.hpp"
#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/iterator/levelorder.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/utils/text/string.hpp"

using namespace genesis;
using namespace placement;
using namespace tree;

// =================================================================================================
//     Reroot
// =================================================================================================

static void TestPlacementReroot(
    size_t reroot_edge_id,
    std::string const& check_string,
    std::vector<double> const& proximal_lengths
) {
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Input data.
    std::string infile = environment->data_dir + "placement/unrooted.jplace";
    Sample sample = JplaceReader().read( utils::from_file( infile ));
    auto& tree = sample.tree();
    EXPECT_TRUE( has_correct_edge_nums( tree ));

    auto edge_map = edge_num_to_edge_map( sample );
    auto edge_ptr = edge_map.at( reroot_edge_id );
    ASSERT_NE( nullptr, edge_ptr );

    // reroot
    make_rooted( sample, *edge_ptr );
    EXPECT_TRUE( validate_topology( sample.tree() ));
    EXPECT_TRUE( has_correct_edge_nums( sample.tree() ));

    std::string result = "";
    for (auto& pq : sample) {
        result += pq.name_at(0).name;
        for (auto& p : pq.placements()) {
            result += std::to_string( p.edge_num() );
        }
        result += " ";
    }

    // Check if the edge_nums reordering outcome is correct.
    EXPECT_EQ( check_string, utils::trim( result )) << " with start edge " << reroot_edge_id;

    // check if proximal lengths were adjusted correctly
    size_t i = 0;
    for (auto& pq : sample) {
        for (auto& p : pq.placements()) {
            EXPECT_DOUBLE_EQ( proximal_lengths[i], p.proximal_length )
                << " with placement " << pq.name_at(0).name << " and reroot edge " << reroot_edge_id
            ;
            ++i;
        }
    }
}

TEST( PlacementManipulation, Reroot )
{
    // Test normal rerooting.
    TestPlacementReroot( 0, "X759 Y10 Z4", {0.3, 0.6, 0.9, 0.1, 0.7, 1.0} );
    TestPlacementReroot( 1, "X536 Y09 Z7", {0.3, 0.6, 0.9, 0.1, 0.7, 0.0} );
    TestPlacementReroot( 2, "X314 Y96 Z5", {0.3, 0.6, 0.9, 0.1, 0.7, 0.0} );
    TestPlacementReroot( 3, "X203 Y75 Z4", {0.3, 0.6, 0.9, 0.1, 0.7, 0.0} );
    TestPlacementReroot( 4, "X203 Y75 Z4", {0.3, 0.6, 0.9, 0.1, 0.7, 0.0} );
    TestPlacementReroot( 5, "X203 Y65 Z9", {0.3, 0.6, 0.9, 0.1, 0.7, 1.0} );
    TestPlacementReroot( 6, "X791 Y32 Z6", {0.7, 0.6, 0.9, 0.1, 0.7, 1.0} );
    TestPlacementReroot( 7, "X670 Y21 Z5", {0.7, 0.6, 0.9, 0.1, 0.7, 1.0} );
    TestPlacementReroot( 8, "X970 Y21 Z5", {0.3, 0.6, 0.9, 0.1, 0.7, 1.0} );

}
