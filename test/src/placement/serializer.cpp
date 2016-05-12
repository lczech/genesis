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

#include <cstdio>
#include <memory>
#include <string>

#include "lib/placement/formats/jplace_reader.hpp"
#include "lib/placement/formats/serializer.hpp"
#include "lib/placement/function/functions.hpp"
#include "lib/placement/function/helper.hpp"
#include "lib/placement/sample.hpp"

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
