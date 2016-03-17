/**
 * @brief Testing Sample class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <cstdio>
#include <memory>
#include <string>

#include "lib/placement/function/functions.hpp"
#include "lib/placement/function/helper.hpp"
#include "lib/placement/io/jplace_reader.hpp"
#include "lib/placement/io/serializer.hpp"
#include "lib/placement/sample.hpp"
#include "lib/tree/io/newick/processor.hpp"

using namespace genesis;
using namespace genesis::placement;

TEST(SampleSerializer, SaveAndLoad)
{
    // Skip test if no data directory availabe.
    NEEDS_TEST_DATA;

    // In and out files.
    std::string infile  = environment->data_dir + "placement/test_a.jplace";
    std::string tmpfile = environment->data_dir + "placement/test_a.bplace";

    // Prepare a Sample with data.
    Sample smp_save;
    EXPECT_NO_THROW (JplaceReader().from_file(infile, smp_save));
    EXPECT_EQ   (5, total_placement_count(smp_save));
    EXPECT_TRUE (validate(smp_save, true, false));

    // Save it to a file.
    EXPECT_NO_THROW( SampleSerializer::save(smp_save, tmpfile) );

    // Load again.
    Sample smp_load;
    EXPECT_NO_THROW( SampleSerializer::load(tmpfile, smp_load) );

    // Check for correctly read data.
    EXPECT_EQ   (5, total_placement_count(smp_load));
    EXPECT_TRUE (validate(smp_load, true, false));

    // Make sure the file is deleted.
    ASSERT_EQ (0, std::remove(tmpfile.c_str()));
}
