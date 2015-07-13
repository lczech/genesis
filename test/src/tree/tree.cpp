/**
 * @brief Testing Tree class.
 *
 * @file
 * @ingroup test
 */

#include <gtest/gtest.h>

#include <string>

#include "lib/tree/default_tree_newick_adapter.hpp"
#include "lib/tree/newick_processor.hpp"
#include "lib/tree/tree.hpp"

using namespace genesis;

TEST(Tree, Nope)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    DefaultTree tree;
    DefaultTreeNewickProcessor().from_string(input, tree);
    std::string output = DefaultTreeNewickProcessor().to_string(tree);

	EXPECT_EQ(input, output);
}
