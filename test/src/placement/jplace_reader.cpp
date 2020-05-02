/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include <string>

#include "genesis/placement/formats/jplace_reader.hpp"
#include "genesis/placement/function/functions.hpp"
#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/sample.hpp"

using namespace genesis;
using namespace genesis::placement;
using namespace genesis::utils;

TEST( JplaceReader, FromFile )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "placement/test_a.jplace";

    Sample smp = JplaceReader().read( from_file( infile ));
    EXPECT_EQ( 5, total_placement_count(smp) );
    EXPECT_EQ( 5, total_name_count(smp) );
    EXPECT_TRUE( validate(smp, true, false) );
    EXPECT_TRUE( has_correct_edge_nums(smp.tree()) );
}

TEST( JplaceReader, FromFileGzip )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "placement/test_a.jplace.gz";

    Sample smp = JplaceReader().read( from_file( infile ));
    EXPECT_EQ( 5, total_placement_count(smp) );
    EXPECT_EQ( 5, total_name_count(smp) );
    EXPECT_TRUE( validate(smp, true, false) );
    EXPECT_TRUE( has_correct_edge_nums(smp.tree()) );
}

TEST( JplaceReader, FromFiles )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    auto const indir = environment->data_dir + "placement/";
    auto const infiles = std::vector<std::string>{
        indir + "test_a.jplace", indir + "test_b.jplace", indir + "test_c.jplace"
    };

    SampleSet const smps = JplaceReader().read( from_files( infiles ));
    EXPECT_EQ( 3, smps.size() );
    EXPECT_EQ( 5, total_placement_count(smps[0]) );
    EXPECT_EQ( 8, total_name_count(smps[2]) );
    EXPECT_EQ( "test_b", smps.name_at(1) );
}

TEST( JplaceReader, FromFilesGzip )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    auto const indir = environment->data_dir + "placement/";
    auto const infiles = std::vector<std::string>{
        indir + "test_a.jplace.gz", indir + "test_b.jplace.gz", indir + "test_c.jplace.gz"
    };

    SampleSet const smps = JplaceReader().read( from_files( infiles ));
    EXPECT_EQ( 3, smps.size() );
    EXPECT_EQ( 5, total_placement_count(smps[0]) );
    EXPECT_EQ( 8, total_name_count(smps[2]) );
    EXPECT_EQ( "test_b", smps.name_at(1) );
}

TEST( JplaceReader, Version1 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "placement/version_1.jplace";

    Sample smp = JplaceReader().read( from_file( infile ));
    EXPECT_EQ( 9, total_placement_count(smp) );
    EXPECT_EQ( 8, total_name_count(smp) );
    EXPECT_TRUE( validate(smp, true, false) );
    EXPECT_TRUE( has_correct_edge_nums(smp.tree()) );
}

TEST( JplaceReader, Version2 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "placement/version_2.jplace";

    Sample smp = JplaceReader().read( from_file( infile ));
    EXPECT_EQ( 9, total_placement_count(smp) );
    EXPECT_EQ( 8, total_name_count(smp) );
    EXPECT_TRUE( validate(smp, true, false) );
    EXPECT_TRUE( has_correct_edge_nums(smp.tree()) );
}

TEST( JplaceReader, Version3 )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    std::string infile = environment->data_dir + "placement/version_3.jplace";

    Sample smp = JplaceReader().read( from_file( infile ));
    EXPECT_EQ( 9, total_placement_count(smp) );
    EXPECT_EQ( 8, total_name_count(smp) );
    EXPECT_TRUE( validate(smp, true, false) );
    EXPECT_TRUE( has_correct_edge_nums(smp.tree()) );
}

// TEST( JplaceReader, Speed )
// {
//     std::string inputfile = "/home/lucas/Projects/data/for_testing/jplace/sample_0_all_big.jplace";
//     auto reader = JplaceReader();
//     Sample smp;
//     reader.from_file( inputfile, smp );
//     LOG_DBG << "size  " << smp.size();
// }
