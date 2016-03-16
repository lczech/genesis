/**
 * @brief Testing Sample class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <memory>

#include "lib/placement/function/functions.hpp"
#include "lib/placement/function/operators.hpp"
#include "lib/placement/io/jplace_reader.hpp"
#include "lib/placement/io/newick_processor.hpp"
#include "lib/placement/sample.hpp"
#include "lib/tree/io/newick/processor.hpp"

using namespace genesis;
using namespace genesis::placement;

TEST(Sample, WithTree)
{
    auto tree = std::make_shared<PlacementTree>();
    EXPECT_TRUE( PlacementTreeNewickProcessor().from_string(
        "((B:2.0{0},(D:2.0{1},E:2.0{2})C:2.0{3})A:2.0{4},F:2.0{5},(H:2.0{6},I:2.0{7})G:2.0{8})R:2.0{9};",
    *tree));

    Sample smp(tree);
    EXPECT_EQ   (0, smp.placement_count());
    EXPECT_TRUE (validate(smp, true, false));
}

// =================================================================================================
//     Merging Duplicates
// =================================================================================================

void test_sample_stats (
    const Sample& smp,
    const size_t expected_pquery_size,
    const size_t expected_placement_size,
    const size_t expected_name_size
) {
    EXPECT_TRUE (validate(smp, true, false));

    EXPECT_EQ (expected_pquery_size,    smp.pquery_size());
    EXPECT_EQ (expected_placement_size, smp.placement_count());

    size_t name_count = 0;
    for (auto& pqry : smp.pqueries()) {
        name_count += pqry->name_size();
    }
    EXPECT_EQ (expected_name_size, name_count);
}

TEST(Sample, MergeDuplicatesSimple)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read file.
    std::string infile = environment->data_dir + "placement/duplicates_a.jplace";
    Sample smp;
    EXPECT_NO_THROW( JplaceReader().from_file(infile, smp) );

    // Check before merging.
    test_sample_stats(smp, 7, 8, 7);

    // Run the function of interest!
    merge_duplicates(smp);

    // Check after merging.
    test_sample_stats(smp, 3, 7, 3);
}

TEST(Sample, MergeDuplicatesTransitive)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read file.
    std::string infile = environment->data_dir + "placement/duplicates_b.jplace";
    Sample smp;
    EXPECT_NO_THROW( JplaceReader().from_file(infile, smp) );

    // Check before merging.
    test_sample_stats(smp, 7, 10, 11);

    // Run the function of interest!
    merge_duplicates(smp);

    // Check after merging.
    test_sample_stats(smp, 1, 4, 4);
}
