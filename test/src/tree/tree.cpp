/**
 * @brief Testing Tree class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <string>

#include "lib/tree/default_tree_newick_adapter.hpp"
#include "lib/tree/io/newick_processor.hpp"
#include "lib/tree/tree.hpp"

using namespace genesis;

TEST(Tree, Basics)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    DefaultTree tree;
    DefaultTreeNewickProcessor().from_string(input, tree);

    EXPECT_EQ(7, tree.leaf_count());
    EXPECT_EQ(6, tree.inner_count());
    EXPECT_EQ(13, tree.node_count());
    EXPECT_TRUE(tree.is_bifurcating());

    EXPECT_EQ("R", tree.root_node()->data.name);
    EXPECT_TRUE(tree.validate());
}
