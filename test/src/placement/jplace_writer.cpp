/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include <cstdio>
#include <string>

#include "genesis/placement/formats/jplace_reader.hpp"
#include "genesis/placement/formats/jplace_writer.hpp"
#include "genesis/placement/function/functions.hpp"
#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/sample.hpp"

#include "genesis/utils/core/fs.hpp"

using namespace genesis;
using namespace genesis::placement;
using namespace genesis::utils;

TEST( JplaceWriter, ToTarget )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "placement/test_a.jplace";
    std::string tmpfile = environment->data_dir + "placement/test_a.tmp.jplace";

    Sample smp = JplaceReader().read( from_file( infile ));
    EXPECT_EQ( 5, total_placement_count( smp ));

    JplaceWriter().write( smp, to_file( tmpfile ));

    std::string target;
    JplaceWriter().write( smp, to_string( target ));
    auto const read_again = file_read( tmpfile );

    EXPECT_TRUE( !target.empty() );
    EXPECT_EQ( read_again, target );
    ASSERT_EQ (0, std::remove(tmpfile.c_str()));

    auto const smp2 = JplaceReader().read( from_string( target ));
    EXPECT_EQ( 5, total_placement_count( smp2 ));
}
