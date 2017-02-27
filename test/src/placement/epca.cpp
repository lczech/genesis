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
#include "genesis/placement/function/epca.hpp"
#include "genesis/placement/function/measures.hpp"
#include "genesis/placement/sample.hpp"
#include "genesis/placement/sample_set.hpp"

#include "genesis/tree/function/functions.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/formats/csv/reader.hpp"
#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/math/matrix/operators.hpp"
#include "genesis/utils/math/matrix/operators.hpp"
#include "genesis/utils/math/matrix/pca.hpp"
#include "genesis/utils/math/matrix/statistics.hpp"
#include "genesis/utils/text/string.hpp"

using namespace genesis;
using namespace genesis::placement;

/*

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

    std::string indir = "/home/lucas/Projects/bacterial_vaginosis/03_epa_magny/orig_queries_jplace/";
    auto files = utils::dir_list_files( indir, ".*\\.jplace" );

    // auto imbalance_matrix = utils::Matrix<double>();

    LOG_DBG << "reading " << files.size() << " files";
    SampleSet set;
    for( size_t s = 0; s < files.size(); ++s ) {
        auto file = files[s];

        LOG_DBG1 << file;
        Sample smp;
        smp = JplaceReader().from_file( indir + file );
        set.add(smp);

        // if( imbalance_matrix.rows() == 0 ) {
        //     imbalance_matrix = utils::Matrix<double>( files.size(), smp.tree().edge_count() );
        //
        //     LOG_INFO << "tree edge count " << smp.tree().edge_count();
        //     LOG_INFO << "tree leaf count " << tree::leaf_node_count( smp.tree() );
        //     LOG_INFO << "tree innr count " << tree::inner_node_count( smp.tree() );
        // }
        //
        // auto imbalance_vec = epca_imbalance_vector( smp );
        // if( imbalance_vec.size() != smp.tree().edge_count() ) {
        //     throw std::runtime_error( "bad imb vec size." );
        // }
        //
        // for( size_t e = 0; e < smp.tree().edge_count(); ++e ) {
        //     imbalance_matrix( s, e ) = imbalance_vec[ e ];
        // }
    }

    // LOG_DBG << "filtering matrix";
    //
    // utils::file_write(
    //     utils::to_string( imbalance_matrix ), "/home/lucas/tmp/bv_epca/imbalance_unfiltered.csv"
    // );
    //
    // epca_filter_constant_columns( imbalance_matrix );
    //
    // utils::file_write(
    //     utils::to_string( imbalance_matrix ), "/home/lucas/tmp/bv_epca/imbalance_filtered.csv"
    // );

    LOG_DBG << "running pca";



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

    // LOG_DBG << "Finished reading. Starting EPCA";

    epca( set );

    // auto pca = utils::principal_component_analysis( imbalance_matrix, 3, utils::PcaStandardization::kCovariance );
    // // auto pca = utils::principal_component_analysis( imbalance_matrix, 5 );
    //
    // printf("\nEigenvalues:\n");
    // for( auto val : pca.eigenvalues ) {
    //     printf("%18.5f\n", val );
    // }
    //
    // printf("\nEigenvectors:\n");
    // for( size_t r = 0; r < 20; ++r ) {
    // // for( size_t r = 0; r < pca.eigenvectors.rows(); ++r ) {
    //     for( size_t c = 0; c < pca.eigenvectors.cols(); ++c ) {
    //         printf("%12.4f", pca.eigenvectors(r,c));
    //     }
    //     printf("\n");
    // }
    // printf("... %u rows in total\n", static_cast<unsigned int>(pca.eigenvectors.rows()));
    //
    // printf("\nProjections of row-points on first 3 prin. comps.:\n");
    // for (size_t i = 0; i < 10; i++) {
    // // for (size_t i = 0; i < pca.projection.rows(); i++) {
    //     for (size_t j = 0; j < pca.projection.cols(); j++)  {
    //         printf("%12.4f", pca.projection(i,j));
    //     }
    //     printf("\n");
    // }
    // printf("... %u rows in total\n", static_cast<unsigned int>(pca.projection.rows()));
    //
    // utils::file_write( utils::to_string( pca.projection ), "/home/lucas/tmp/bv_epca/my.proj" );

}

//*/
/*

TEST( SampleMeasures, GuppyPCA )
{
    auto        reader = utils::CsvReader();
    std::string infile = "/home/lucas/tmp/bv_epca/orig_queries_jplace_.edgediff.csv";
    auto        table  = reader.from_file( infile );
    auto data = utils::Matrix<double>( table.size(), table[0].size() -1 );
    for( size_t i = 0; i < table.size(); ++i ) {
        for( size_t j = 1; j < table[0].size(); ++j ) {
            data( i, j ) = stod( table[i][j] );
        }
    }

    auto pca = utils::principal_component_analysis( data, 5, utils::PcaStandardization::kCovariance );
    // auto pca = utils::principal_component_analysis( imbalance_matrix, 5 );

    printf("\nEigenvalues:\n");
    for( auto val : pca.eigenvalues ) {
        printf("%18.5f\n", val );
    }

    printf("\nEigenvectors:\n");
    for( size_t r = 0; r < 20; ++r ) {
    // for( size_t r = 0; r < pca.eigenvectors.rows(); ++r ) {
        for( size_t c = 0; c < pca.eigenvectors.cols(); ++c ) {
            printf("%12.4f", pca.eigenvectors(r,c));
        }
        printf("\n");
    }
    printf("... %u rows in total\n", static_cast<unsigned int>(pca.eigenvectors.rows()));

    printf("\nProjections of row-points on first 3 prin. comps.:\n");
    for (size_t i = 0; i < 10; i++) {
    // for (size_t i = 0; i < pca.projection.rows(); i++) {
        for (size_t j = 0; j < pca.projection.cols(); j++)  {
            printf("%12.4f", pca.projection(i,j));
        }
        printf("\n");
    }
    printf("... %u rows in total\n", static_cast<unsigned int>(pca.projection.rows()));

    utils::file_write( utils::to_string( pca.projection ), "/home/lucas/tmp/bv_epca/myguppy.proj" );
}

//*/
