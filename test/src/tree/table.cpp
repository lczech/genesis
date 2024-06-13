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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief Testing Newick class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <string>
#include <vector>

#include "genesis/tree/common_tree/functions.hpp"
#include "genesis/tree/common_tree/newick_reader.hpp"
#include "genesis/tree/common_tree/newick_writer.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/formats/newick/writer.hpp"
#include "genesis/tree/formats/table/reader.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/printer/compact.hpp"
#include "genesis/tree/printer/compact.hpp"
#include "genesis/tree/printer/detailed.hpp"
#include "genesis/tree/printer/table.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/utils/math/random.hpp"

using namespace genesis;
using namespace genesis::tree;
using namespace genesis::utils;

TEST( TreeTable, Reading )
{
    // Test tree:
    // R
    //   A
    //   B
    //   C
    //     D
    //       G
    //       H
    //     E
    //     F

    // Children and parents in random order
    auto const children = std::vector<std::string>{ "E", "B", "F", "G", "C", "D", "A", "H" };
    auto const parents  = std::vector<std::string>{ "C", "R", "C", "D", "R", "C", "R", "D" };

    auto const tree = make_tree_from_parents_table( children, parents );
    EXPECT_TRUE( validate_topology( tree ));

    // LOG_DBG << PrinterCompact().print( tree );
    // LOG_DBG << PrinterDetailed().print( tree );

}

// =================================================================================================
//     Random Fuzzy
// =================================================================================================

std::string test_generate_random_newick_tree_( size_t num_leaves )
{
    std::string const node_prefix = "n_";

    // Make a list of leaf nodes
    std::vector<std::string> nodes;
    size_t node_cnt = 0;
    for( ; node_cnt < num_leaves; ++node_cnt ) {
        nodes.push_back( node_prefix + std::to_string( node_cnt ));
    }

    // Combine the leaves into subtres
    while( nodes.size() > 1 ) {
        // Select two random nodes
        size_t a = permuted_congruential_generator( nodes.size() - 1 );
        size_t b = permuted_congruential_generator( nodes.size() - 1 );

        // If they are not the same, we can make a subtree out of them
        if( a != b ) {

            // Make the new node
            std::string const new_node = node_prefix + std::to_string( node_cnt );
            std::string const new_branch = "(" + nodes[a] + "," + nodes[b] + ")" + new_node;
            ++node_cnt;

            // Remove the previous subtrees and add the new one.
            if( a < b ) {
                nodes.erase( nodes.begin() + b );
                nodes.erase( nodes.begin() + a );
            } else {
                nodes.erase( nodes.begin() + a );
                nodes.erase( nodes.begin() + b );
            }
            nodes.push_back( new_branch );
        }
    }

    return nodes[0] + ";";
}

struct TreeTableTestData
{
    std::vector<std::string> children_names;
    std::vector<std::string> parent_names;
    size_t num_nodes;
};

TreeTableTestData tree_to_table_to_test_data_( Tree const& tree )
{
    TreeTableTestData result;
    for( auto const& node : tree.nodes() ) {
        if( is_root( node )) {
            continue;
        }
        auto const& c_name = node.data<CommonNodeData>().name;
        auto const& p_name = node.link().outer().node().data<CommonNodeData>().name;
        // LOG_DBG << "at " << c_name << " to " << p_name;

        result.children_names.push_back( c_name );
        result.parent_names.push_back( p_name );
    }

    result.num_nodes = tree.node_count();
    return result;
}

TreeTableTestData test_create_random_tree_name_data_()
{
    // Make a random newick tree.
    auto const num_leaves = permuted_congruential_generator( 2, 100 );
    auto const newick = test_generate_random_newick_tree_( num_leaves );
    LOG_DBG << newick;

    // Turn it into a pair of parent child lists.
    auto const tree = CommonTreeNewickReader().read( utils::from_string( newick ));
    return tree_to_table_to_test_data_( tree );
}

void test_tree_table_fuzzy_run_()
{
    auto const data = test_create_random_tree_name_data_();

    // Parse as table tree
    auto const tree = make_tree_from_parents_table( data.children_names, data.parent_names );
    EXPECT_TRUE( validate_topology( tree ));
    EXPECT_EQ( data.num_nodes, tree.node_count() );
    EXPECT_EQ(
        "n_" + std::to_string( data.num_nodes - 1 ),
        tree.root_node().data<CommonNodeData>().name
    );

    // Write back to newick
    // auto writer = CommonTreeNewickWriter();
    // writer.enable_branch_lengths( false );
    // writer.trailing_new_line( false );
    // std::string output = writer.to_string( tree );
    // LOG_DBG << output;
}

TEST( TreeTable, Random )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // For the duration of the test, we deactivate debug logging.
    // But if needed, comment this line out, and each test will report its input.
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    size_t num_tests = 500;
    for( size_t i = 0; i < num_tests; ++i ) {
        LOG_DBG << "=================================";
        LOG_DBG << "Test " << i;
        test_tree_table_fuzzy_run_();
    }
}
