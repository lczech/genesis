/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <memory>

#include "lib/placement/function/helper.hpp"
#include "lib/placement/function/functions.hpp"
#include "lib/placement/function/operators.hpp"
#include "lib/placement/io/jplace_reader.hpp"
#include "lib/placement/io/newick_reader.hpp"
#include "lib/placement/sample.hpp"
#include "lib/tree/io/newick/reader.hpp"

using namespace genesis;
using namespace genesis::placement;

TEST(SampleFunctions, Filter)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read file.
    std::string infile = environment->data_dir + "placement/duplicates_b.jplace";
    Sample smp;
    EXPECT_NO_THROW( JplaceReader().from_file(infile, smp) );

    // Check before merging.
    EXPECT_EQ( 10, total_placement_count(smp) );

    // Filter everything below a threshold and check result.
    filter_min_weight_threshold( smp, 0.5 );
    EXPECT_EQ( 8, total_placement_count(smp) );

    // Re-read the file.
    EXPECT_NO_THROW( JplaceReader().from_file(infile, smp) );

    // Filter max number of placements and check result.
    filter_n_max_weight_placements( smp, 1 );
    EXPECT_EQ( 7, total_placement_count(smp) );

    // Re-read the file.
    EXPECT_NO_THROW( JplaceReader().from_file(infile, smp) );

    // Filter max number of placements and check result.
    filter_min_accumulated_weight( smp, 0.6 );
    EXPECT_EQ( 8, total_placement_count(smp) );
}
