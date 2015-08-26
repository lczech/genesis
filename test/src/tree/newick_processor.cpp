/**
 * @brief Testing Newick class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <string>

#include "lib/tree/default_tree_newick_adapter.hpp"
#include "lib/tree/newick_processor.hpp"
#include "lib/tree/tree.hpp"

using namespace genesis;

TEST(NewickProcessor, FromAndToString)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    DefaultTree tree;
    EXPECT_TRUE(DefaultTreeNewickProcessor().from_string(input, tree));
    std::string output = DefaultTreeNewickProcessor().to_string(tree);

	EXPECT_EQ(input, output);
}

TEST(NewickProcessor, NewickVariants)
{
    DefaultTree tree;

    // No nodes are named.
    EXPECT_TRUE( DefaultTreeNewickProcessor().from_string(
        "(,,(,));",
        tree
    ));
    EXPECT_TRUE(tree.validate());

    // Leaf nodes are named.
    EXPECT_TRUE( DefaultTreeNewickProcessor().from_string(
        "(A,B,(C,D));",
        tree
    ));
    EXPECT_TRUE(tree.validate());

    // All nodes are named.
    EXPECT_TRUE( DefaultTreeNewickProcessor().from_string(
        "(A,B,(C,D)E)F;",
        tree
    ));
    EXPECT_TRUE(tree.validate());

    // All but root node have a distance to parent.
    EXPECT_TRUE( DefaultTreeNewickProcessor().from_string(
        "(:0.1,:0.2,(:0.3,:0.4):0.5);",
        tree
    ));
    EXPECT_TRUE(tree.validate());

    // All have a distance to parent.
    EXPECT_TRUE( DefaultTreeNewickProcessor().from_string(
        "(:0.1,:0.2,(:0.3,:0.4):0.5):0.0;",
        tree
    ));
    EXPECT_TRUE(tree.validate());

    // Distances and leaf names (popular).
    EXPECT_TRUE( DefaultTreeNewickProcessor().from_string(
        "(A:0.1,B:0.2,(C:0.3,D:0.4):0.5);",
        tree
    ));
    EXPECT_TRUE(tree.validate());

    // Distances and all names.
    EXPECT_TRUE( DefaultTreeNewickProcessor().from_string(
        "(A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;",
        tree
    ));
    EXPECT_TRUE(tree.validate());

    // A tree rooted on a leaf node (rare).
    EXPECT_TRUE( DefaultTreeNewickProcessor().from_string(
        "((B:0.2,(C:0.3,D:0.4)E:0.5)F:0.1)A;",
        tree
    ));
    EXPECT_TRUE(tree.validate());

    // All mixed, with comments and tags.
    EXPECT_TRUE( DefaultTreeNewickProcessor().from_string(
        "( ( Ant:0.2{0}, [a comment] 'Bee':0.09{1} )Inner:0.7{2}, Coyote:0.5{3} ){4};",
        tree
    ));
    EXPECT_TRUE(tree.validate());
}
