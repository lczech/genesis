/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include "lib/utils/io/nexus/document.hpp"
#include "lib/utils/io/nexus/taxa.hpp"
#include "lib/utils/io/nexus/trees.hpp"
#include "lib/utils/io/nexus/writer.hpp"

#include <sstream>

using namespace genesis;
using namespace genesis::utils;

TEST(Nexus, Writer)
{
    auto doc = NexusDocument();

    // TODO create a factory method for each class, just for simplicity.
    // TODO make all functions comparing block names case insensitive!

    auto trees = make_unique<NexusTrees>();
    trees->add_tree( "life", "(human, mouse, rat);" );
    doc.set_block( std::move(trees) );

    auto taxa = make_unique<NexusTaxa>();
    taxa->add_taxa({ "human", "mouse", "rat" });
    doc.set_block( std::move(taxa) );

    EXPECT_TRUE( doc.has_block("TREES") );
    EXPECT_TRUE( doc.has_block("TAXA") );
    EXPECT_FALSE( doc.has_block("stargazer") );

    std::ostringstream buffer;

    auto writer = NexusWriter();
    writer.to_stream( doc, buffer );
    std::cout << buffer.str();
}
