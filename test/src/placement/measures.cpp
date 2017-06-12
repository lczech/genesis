/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include <memory>

#include "genesis/placement/formats/jplace_reader.hpp"
#include "genesis/placement/function/measures.hpp"
#include "genesis/placement/sample.hpp"
#include "genesis/placement/sample_set.hpp"
#include "genesis/utils/math/matrix.hpp"

using namespace genesis;
using namespace genesis::placement;

TEST( SampleMeasures, EDPL )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Input files.
    std::string infile = environment->data_dir + "placement/duplicates_b.jplace";

    Sample smpl = JplaceReader().from_file( infile );

    auto edpl_vec = edpl( smpl );
    std::vector<double> expect = { 1.5, 0, 1.218, 0, 0, 1.2, 0 };
    // EXPECT_EQ( expect, edpl_vec );

    EXPECT_EQ( expect.size(), edpl_vec.size() );
    for( size_t i = 0; i < expect.size(); ++i ) {
        EXPECT_FLOAT_EQ( expect[i], edpl_vec[i] );
    }
}

TEST(SampleMeasures, EarthMoversDistance)
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Input files.
    std::string infile_lhs = environment->data_dir + "placement/test_a.jplace";
    std::string infile_rhs = environment->data_dir + "placement/test_b.jplace";

    // Read files.
    Sample smp_lhs = JplaceReader().from_file( infile_lhs );
    Sample smp_rhs = JplaceReader().from_file( infile_rhs );

    // Distances and symmetric cases.
    EXPECT_FLOAT_EQ( 2.435, earth_movers_distance( smp_lhs, smp_rhs, false ));
    EXPECT_FLOAT_EQ( 2.435, earth_movers_distance( smp_rhs, smp_lhs, false ));
    EXPECT_FLOAT_EQ( 3.185, earth_movers_distance( smp_lhs, smp_rhs, true  ));
    EXPECT_FLOAT_EQ( 3.185, earth_movers_distance( smp_rhs, smp_lhs, true  ));

    // Self-distances.
    EXPECT_FLOAT_EQ( 0.0, earth_movers_distance( smp_lhs, smp_lhs, false ));
    EXPECT_FLOAT_EQ( 0.0, earth_movers_distance( smp_rhs, smp_rhs, false ));
    EXPECT_FLOAT_EQ( 0.6, earth_movers_distance( smp_lhs, smp_lhs, true ));
    EXPECT_FLOAT_EQ( 0.9, earth_movers_distance( smp_rhs, smp_rhs, true ));

    // Set-version of the EMD.
    SampleSet set;
    set.add( smp_lhs );
    set.add( smp_rhs );

    auto set_emd_o =  earth_movers_distance( set, false );
    auto set_emd_p =  earth_movers_distance( set, true );

    EXPECT_FLOAT_EQ( 0.0,   set_emd_o( 0, 0 ) );
    EXPECT_FLOAT_EQ( 2.435, set_emd_o( 0, 1 ) );
    EXPECT_FLOAT_EQ( 2.435, set_emd_o( 1, 0 ) );
    EXPECT_FLOAT_EQ( 0.0,   set_emd_o( 1, 1 ) );

    EXPECT_FLOAT_EQ( 0.6,   set_emd_p( 0, 0 ) );
    EXPECT_FLOAT_EQ( 3.185, set_emd_p( 0, 1 ) );
    EXPECT_FLOAT_EQ( 3.185, set_emd_p( 1, 0 ) );
    EXPECT_FLOAT_EQ( 0.9,   set_emd_p( 1, 1 ) );
}

TEST( SampleMeasures, NodeHistogramDistance )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Input files.
    std::string infile_lhs = environment->data_dir + "placement/test_a.jplace";
    std::string infile_rhs = environment->data_dir + "placement/test_b.jplace";

    // Read files.
    Sample smp_lhs = JplaceReader().from_file(infile_lhs);
    Sample smp_rhs = JplaceReader().from_file(infile_rhs);

    // Distance and symmetric case.
    EXPECT_FLOAT_EQ( 1.324, node_histogram_distance( smp_lhs, smp_rhs, 10 ));
    EXPECT_FLOAT_EQ( 1.324, node_histogram_distance( smp_rhs, smp_lhs, 10 ));

    // Self-distances.
    EXPECT_FLOAT_EQ( 0.0, node_histogram_distance( smp_lhs, smp_lhs ));
    EXPECT_FLOAT_EQ( 0.0, node_histogram_distance( smp_rhs, smp_rhs ));
}
