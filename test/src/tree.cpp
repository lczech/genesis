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

TEST(tree_test, newick_processor)
{
    using namespace genesis;

    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    DefaultTree tree;
    NewickProcessor::from_string(input, tree);
    std::string output = NewickProcessor::to_string(tree);

	EXPECT_EQ(input, output);
}

TEST(tree_test, failing)
{
    using namespace genesis;

    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    DefaultTree tree;
    NewickProcessor::from_string(input, tree);
    std::string output = NewickProcessor::to_string(tree);

	EXPECT_EQ("input", output);
}
