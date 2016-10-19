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
#include "lib/placement/function/epca.hpp"
#include "lib/placement/function/measures.hpp"
#include "lib/placement/sample.hpp"
#include "lib/placement/sample_set.hpp"

#include "lib/utils/core/fs.hpp"

using namespace genesis;
using namespace genesis::placement;

TEST( SampleMeasures, EdgePCA )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // // Input files.
    // std::string infile_lhs = environment->data_dir + "placement/test_a.jplace";
    // std::string infile_rhs = environment->data_dir + "placement/test_b.jplace";
    //
    // // Read files.
    // Sample smp_lhs;
    // Sample smp_rhs;
    // EXPECT_NO_THROW( JplaceReader().from_file(infile_lhs, smp_lhs) );
    // EXPECT_NO_THROW( JplaceReader().from_file(infile_rhs, smp_rhs) );
    //
    // SampleSet set;
    // set.add(smp_lhs);
    // set.add(smp_rhs);

    // std::string indir = "/home/lucas/Projects/bacterial_vaginosis/03_epa_magny/orig_queries_jplace/";
    // auto files = utils::dir_list_files( indir, ".*\\.jplace" );
    //
    // LOG_DBG << "reading " << files.size() << " files";
    // SampleSet set;
    // for( auto file : files ) {
    //     // LOG_DBG << file;
    //     Sample smp;
    //     JplaceReader().from_file( indir + file, smp );
    //     set.add(smp);
    // }

    // auto vec_lhs = epca_imbalance_vector( smp_lhs );
    // auto vec_rhs = epca_imbalance_vector( smp_rhs );

    // LOG_INFO << "lhs";
    // for( size_t i = 0; i < vec_lhs.size(); ++i ) {
    //     LOG_INFO << "    " << i << " " << vec_lhs[i];
    // }
    // LOG_INFO << "rhs";
    // for( size_t i = 0; i < vec_rhs.size(); ++i ) {
    //     LOG_INFO << "    " << i << " " << vec_rhs[i];
    // }

    LOG_DBG << "Finished reading. Starting EPCA";

    // epca( set );
}