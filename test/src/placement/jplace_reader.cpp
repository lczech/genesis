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
 * @brief Testing JplaceProcessor class.
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <string>

#include "lib/placement/formats/jplace_reader.hpp"
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
