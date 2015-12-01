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

TEST(Nexus, Writer)
{
    auto doc = nexus::Document();

    auto trees = nexus::Trees();
    trees.add_tree( "life", "(human, mouse, rat);" );
    doc.set_block_trees( trees );

    auto taxa = nexus::Taxa();
    taxa.add_taxa({ "human", "mouse", "rat" });
    doc.set_block_taxa( taxa );

    std::ostringstream buffer;

    auto writer = nexus::Writer();
    writer.to_stream( doc, buffer );
    std::cout << buffer.str();
}
