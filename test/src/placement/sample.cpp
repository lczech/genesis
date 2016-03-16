/**
 * @brief Testing Sample class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <memory>

#include "lib/placement/functions.hpp"
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

    Sample map(tree);
    EXPECT_EQ   (0, map.placement_count());
    EXPECT_TRUE (map.validate(true, false));
}

// =================================================================================================
//     Merging Duplicates
// =================================================================================================

void test_sample_stats (
    const Sample& map,
    const size_t expected_pquery_size,
    const size_t expected_placement_size,
    const size_t expected_name_size
) {
    EXPECT_TRUE (map.validate(true, false));

    EXPECT_EQ (expected_pquery_size,    map.pquery_size());
    EXPECT_EQ (expected_placement_size, map.placement_count());

    size_t name_count = 0;
    for (auto& pqry : map.pqueries()) {
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
    Sample map;
    EXPECT_NO_THROW( JplaceReader().from_file(infile, map) );

    // Check before merging.
    test_sample_stats(map, 7, 8, 7);

    // Run the function of interest!
    merge_duplicates(map);

    // Check after merging.
    test_sample_stats(map, 3, 7, 3);
}

TEST(Sample, MergeDuplicatesTransitive)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read file.
    std::string infile = environment->data_dir + "placement/duplicates_b.jplace";
    Sample map;
    EXPECT_NO_THROW( JplaceReader().from_file(infile, map) );

    // Check before merging.
    test_sample_stats(map, 7, 10, 11);

    // Run the function of interest!
    merge_duplicates(map);

    // Check after merging.
    test_sample_stats(map, 1, 4, 4);
}
