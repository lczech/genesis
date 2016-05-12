/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief Testing Sample class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <memory>

#include "lib/placement/formats/jplace_reader.hpp"
#include "lib/placement/formats/newick_reader.hpp"
#include "lib/placement/function/functions.hpp"
#include "lib/placement/function/helper.hpp"
#include "lib/placement/function/operators.hpp"
#include "lib/placement/sample.hpp"
#include "lib/tree/formats/newick/reader.hpp"

using namespace genesis;
using namespace genesis::placement;

TEST(Sample, WithTree)
{
    auto tree = PlacementTree();
    EXPECT_TRUE( PlacementTreeNewickReader().from_string(
        "((B:2.0{0},(D:2.0{1},E:2.0{2})C:2.0{3})A:2.0{4},F:2.0{5},(H:2.0{6},I:2.0{7})G:2.0{8})R:2.0{9};",
        tree
    ));

    Sample smp(tree);
    EXPECT_EQ   (0, total_placement_count(smp));
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
    EXPECT_EQ (expected_placement_size, total_placement_count(smp));

    size_t name_count = 0;
    for (auto& pqry : smp.pqueries()) {
        name_count += pqry.name_size();
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
