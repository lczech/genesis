/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
#include "genesis/placement/function/cog.hpp"
#include "genesis/placement/function/emd.hpp"
#include "genesis/placement/function/measures.hpp"
#include "genesis/placement/function/nhd.hpp"
#include "genesis/placement/sample.hpp"
#include "genesis/placement/sample_set.hpp"
#include "genesis/utils/containers/matrix.hpp"

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
    EXPECT_FLOAT_EQ( 2.8458333, earth_movers_distance( smp_lhs, smp_rhs, 1.0, false ));
    EXPECT_FLOAT_EQ( 2.8458333, earth_movers_distance( smp_rhs, smp_lhs, 1.0, false ));
    EXPECT_FLOAT_EQ( 3.4916666, earth_movers_distance( smp_lhs, smp_rhs, 1.0, true  ));
    EXPECT_FLOAT_EQ( 3.4916666, earth_movers_distance( smp_rhs, smp_lhs, 1.0, true  ));

    // Self-distances.
    EXPECT_FLOAT_EQ( 0.0, earth_movers_distance( smp_lhs, smp_lhs, 1.0, false ));
    EXPECT_FLOAT_EQ( 0.0, earth_movers_distance( smp_rhs, smp_rhs, 1.0, false ));
    EXPECT_FLOAT_EQ( 0.52499998, earth_movers_distance( smp_lhs, smp_lhs, 1.0, true ));
    EXPECT_FLOAT_EQ( 0.76666665, earth_movers_distance( smp_rhs, smp_rhs, 1.0, true ));

    // Set-version of the EMD.
    SampleSet set;
    set.add( smp_lhs );
    set.add( smp_rhs );

    auto set_emd_o =  earth_movers_distance( set, 1.0, false );
    auto set_emd_p =  earth_movers_distance( set, 1.0, true );

    EXPECT_FLOAT_EQ( 0.0,       set_emd_o( 0, 0 ) );
    EXPECT_FLOAT_EQ( 2.8458333, set_emd_o( 0, 1 ) );
    EXPECT_FLOAT_EQ( 2.8458333, set_emd_o( 1, 0 ) );
    EXPECT_FLOAT_EQ( 0.0,       set_emd_o( 1, 1 ) );

    EXPECT_FLOAT_EQ( 0.52499998, set_emd_p( 0, 0 ) );
    EXPECT_FLOAT_EQ( 3.4916666,  set_emd_p( 0, 1 ) );
    EXPECT_FLOAT_EQ( 3.4916666,  set_emd_p( 1, 0 ) );
    EXPECT_FLOAT_EQ( 0.7666666,  set_emd_p( 1, 1 ) );
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
    EXPECT_FLOAT_EQ( 1.9533334, node_histogram_distance( smp_lhs, smp_rhs, 10 ));
    EXPECT_FLOAT_EQ( 1.9533334, node_histogram_distance( smp_rhs, smp_lhs, 10 ));

    // Self-distances.
    EXPECT_FLOAT_EQ( 0.0, node_histogram_distance( smp_lhs, smp_lhs ));
    EXPECT_FLOAT_EQ( 0.0, node_histogram_distance( smp_rhs, smp_rhs ));

    // Use SampleSet functions.
    SampleSet set;
    set.add( smp_lhs );
    set.add( smp_rhs );

    auto const nhd_mat = node_histogram_distance( set, 10 );
    EXPECT_EQ( 4, nhd_mat.size() );

    // Check matrix with negative histogram axis.
    EXPECT_FLOAT_EQ( 0.0,    nhd_mat( 0, 0 ));
    EXPECT_FLOAT_EQ( 1.9533334, nhd_mat( 1, 0 ));
    EXPECT_FLOAT_EQ( 1.9533334, nhd_mat( 0, 1 ));
    EXPECT_FLOAT_EQ( 0.0,    nhd_mat( 1, 1 ));
}
