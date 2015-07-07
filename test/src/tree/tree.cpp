/**
 * @brief Testing Tree class.
 *
 * @file
 * @ingroup test
 */

#include <gtest/gtest.h>

#include <string>

#include "lib/tree/newick_processor.hpp"
#include "lib/tree/tree.hpp"

using namespace genesis;

TEST(Tree, Nope)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    DefaultTree tree;
    NewickProcessor().from_string(input, tree);
    std::string output = NewickProcessor().to_string(tree);

	EXPECT_EQ(input, output);
}
