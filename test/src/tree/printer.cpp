/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <string>

#include "lib/tree/default/newick_processor.hpp"
#include "lib/tree/io/newick/processor.hpp"
#include "lib/tree/printer/compact.hpp"
#include "lib/tree/printer/detailed.hpp"
#include "lib/tree/tree.hpp"

using namespace genesis;
using namespace tree;

TEST(TreePrinter, Compact)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    DefaultTree tree;
    EXPECT_TRUE( DefaultTreeNewickProcessor().from_string( input, tree ));

    auto tp = PrinterCompact();
    // LOG_INFO << tp.print(tree);
}

TEST(TreePrinter, Detailed)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    DefaultTree tree;
    EXPECT_TRUE( DefaultTreeNewickProcessor().from_string( input, tree ));

    auto tp = PrinterDetailed();
    // LOG_INFO << tp.print(tree);
}
