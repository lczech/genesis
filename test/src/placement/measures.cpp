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
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "common.hpp"

#include <memory>

#include "lib/placement/function/measures.hpp"
#include "lib/placement/io/jplace_reader.hpp"
#include "lib/placement/sample.hpp"

using namespace genesis;
using namespace genesis::placement;

TEST(SampleMeasures, EarthMoversDistance)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read file.
    std::string infile_lhs = environment->data_dir + "placement/test_a.jplace";
    std::string infile_rhs = environment->data_dir + "placement/test_b.jplace";

    Sample smp_lhs;
    Sample smp_rhs;
    EXPECT_NO_THROW( JplaceReader().from_file(infile_lhs, smp_lhs) );
    EXPECT_NO_THROW( JplaceReader().from_file(infile_rhs, smp_rhs) );

    LOG_DBG << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    LOG_DBG << earth_movers_distance( smp_lhs, smp_rhs, false );
    LOG_DBG << earth_movers_distance( smp_lhs, smp_rhs, true );
    LOG_DBG << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    LOG_DBG << earth_movers_distance_new( smp_lhs, smp_rhs );
    LOG_DBG << earth_movers_distance_new( smp_lhs, smp_rhs, true );
    LOG_DBG << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
}
