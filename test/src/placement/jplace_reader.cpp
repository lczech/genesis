/**
 * @brief Testing JplaceProcessor class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <string>

#include "lib/placement/io/jplace_reader.hpp"
#include "lib/placement/function/helper.hpp"
#include "lib/placement/function/functions.hpp"
#include "lib/placement/sample.hpp"

using namespace genesis;
using namespace genesis::placement;

TEST(JplaceProcessor, FromFile)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "placement/test_a.jplace";

    Sample smp;
    EXPECT_NO_THROW( JplaceReader().from_file(infile, smp) );
    EXPECT_EQ  ( 5, total_placement_count(smp) );
    EXPECT_TRUE( validate(smp, true, false) );
    EXPECT_TRUE( has_correct_edge_nums(smp.tree()) );
}
