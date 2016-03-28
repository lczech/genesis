/**
 * @brief Testing Tree class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <string>

#include "lib/tree/default/newick_reader.hpp"
#include "lib/tree/function/functions.hpp"
#include "lib/tree/io/newick/reader.hpp"
#include "lib/tree/tree.hpp"

using namespace genesis;
using namespace tree;

TEST(Tree, Basics)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    DefaultTree tree;
    DefaultTreeNewickReader().from_string(input, tree);

    EXPECT_EQ(7, leaf_nodes_count( tree ));
    EXPECT_EQ(6, inner_nodes_count( tree ));
    EXPECT_EQ(13, tree.node_count());
    EXPECT_TRUE( is_bifurcating( tree ));

    EXPECT_EQ("R", tree.root_node().data.name);
    EXPECT_TRUE( validate( tree ));

    // LOG_INFO << tree;
}
