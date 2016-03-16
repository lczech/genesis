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
#include "lib/tree/printer/table.hpp"
#include "lib/tree/tree.hpp"

using namespace genesis;
using namespace tree;

TEST(TreePrinter, Compact)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    DefaultTree tree;
    EXPECT_TRUE( DefaultTreeNewickProcessor().from_string( input, tree ));

    auto tp = PrinterCompact();
    auto res = tp.print(tree);
}

TEST(TreePrinter, Detailed)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    DefaultTree tree;
    EXPECT_TRUE( DefaultTreeNewickProcessor().from_string( input, tree ));

    auto tp = PrinterDetailed();
    auto res = tp.print(tree);
}

TEST(TreePrinter, Table)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    DefaultTree tree;
    EXPECT_TRUE( DefaultTreeNewickProcessor().from_string( input, tree ));

    auto tp = PrinterTable();
    auto res = tp.print(tree);
}
