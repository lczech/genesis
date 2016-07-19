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

#include "lib/placement/formats/jplace_reader.hpp"
#include "lib/placement/function/measures.hpp"
#include "lib/placement/sample.hpp"

using namespace genesis;
using namespace genesis::placement;

TEST(SampleMeasures, EarthMoversDistance)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Input files.
    std::string infile_lhs = environment->data_dir + "placement/test_a.jplace";
    std::string infile_rhs = environment->data_dir + "placement/test_b.jplace";

    // Read files.
    Sample smp_lhs;
    Sample smp_rhs;
    EXPECT_NO_THROW( JplaceReader().from_file(infile_lhs, smp_lhs) );
    EXPECT_NO_THROW( JplaceReader().from_file(infile_rhs, smp_rhs) );

    // Distnaces and symmetric cases.
    EXPECT_FLOAT_EQ( 2.435, earth_movers_distance( smp_lhs, smp_rhs, false ));
    EXPECT_FLOAT_EQ( 2.435, earth_movers_distance( smp_rhs, smp_lhs, false ));
    EXPECT_FLOAT_EQ( 3.185, earth_movers_distance( smp_lhs, smp_rhs, true  ));
    EXPECT_FLOAT_EQ( 3.185, earth_movers_distance( smp_rhs, smp_lhs, true  ));

    // Self-distances.
    EXPECT_FLOAT_EQ( 0.0, earth_movers_distance( smp_lhs, smp_lhs, false ));
    EXPECT_FLOAT_EQ( 0.0, earth_movers_distance( smp_rhs, smp_rhs, false ));
    EXPECT_FLOAT_EQ( 0.6, earth_movers_distance( smp_lhs, smp_lhs, true ));
    EXPECT_FLOAT_EQ( 0.9, earth_movers_distance( smp_rhs, smp_rhs, true ));
}

TEST( SampleMeasures, NodeHistogramDistance )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Input files.
    std::string infile_lhs = environment->data_dir + "placement/test_a.jplace";
    std::string infile_rhs = environment->data_dir + "placement/test_b.jplace";

    // Read files.
    Sample smp_lhs;
    Sample smp_rhs;
    EXPECT_NO_THROW( JplaceReader().from_file(infile_lhs, smp_lhs) );
    EXPECT_NO_THROW( JplaceReader().from_file(infile_rhs, smp_rhs) );

    // Distance and symmetric case.
    EXPECT_FLOAT_EQ( 1.3575, node_histogram_distance( smp_lhs, smp_rhs, 10 ));
    EXPECT_FLOAT_EQ( 1.3575, node_histogram_distance( smp_rhs, smp_lhs, 10 ));

    // Self-distances.
    EXPECT_FLOAT_EQ( 0.0, node_histogram_distance( smp_lhs, smp_lhs ));
    EXPECT_FLOAT_EQ( 0.0, node_histogram_distance( smp_rhs, smp_rhs ));
}
