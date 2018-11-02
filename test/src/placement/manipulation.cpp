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
#include "genesis/tree/default/functions.hpp"
#include "genesis/tree/default/newick_reader.hpp"
#include "genesis/tree/default/tree.hpp"
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



static void TestPlacementReroot(size_t reroot_edge_id,
                                std::string const& check_string,
                                std::vector<double> const& proximal_lengths )
{
    std::string input(
        "{\n"
        "    \"tree\": \"(A:1.0{0},(B:1.0{1},(C:1.0{2},D:1.0{3})E:1.0{4})F:1.0{5},(G:1.0{6},H:1.0{7})I:1.0{8})R:1.0;\",\n"
        "    \"placements\":\n"
        "    [\n"
        "    {\"p\": [\n"
        "      [8, -332481.006816, 0.700564, 0.3, 0.201565],\n"
        "      [6, -332482.554048, 0.149105, 0.6, 0.201505],\n"
        "      [0, -332483.401552, 0.063889, 0.9, 0.172159]\n"
        "      ],\n"
        "    \"n\": [\"X\"]\n"
        "    },\n"
        "    {\"p\": [\n"
        "      [2, -332592.702719, 0.999978, 0.1, 0.816126],\n"
        "      [1, -332592.702719, 0.999978, 0.7, 0.816126]\n"
        "      ],\n"
        "    \"n\": [\"Y\"]\n"
        "    },\n"
        "    {\"p\": [\n"
        "      [5, -332592.702719, 0.999978, 1.0, 0.816126]\n"
        "      ],\n"
        "    \"n\": [\"Z\"]\n"
        "    }\n"
        "    ],\n"
        "  \"metadata\": {\"invocation\": \"\"},\n"
        "  \"version\": 3,\n"
        "  \"fields\": [\"edge_num\", \"likelihood\", \"like_weight_ratio\", \"proximal_length\", \"pendant_length\"]\n"
        "}\n");

    std::string result = "";

    Sample sample = JplaceReader().from_string( input );
    auto& tree = sample.tree();

    if (not has_correct_edge_nums(tree)) {
        throw std::runtime_error{"u wot m8"};
    }

    auto edge_map = edge_num_to_edge_map( sample );

    auto edge_ptr = edge_map.at( reroot_edge_id );
    ASSERT_NE( nullptr, edge_ptr );

    // reroot
    placement::root( sample, *edge_ptr );
    EXPECT_TRUE( validate_topology( tree ));

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
            EXPECT_DOUBLE_EQ( proximal_lengths[i++], p.proximal_length )
                << " with placement " << pq.name_at(0).name << " and reroot edge " << reroot_edge_id;
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

