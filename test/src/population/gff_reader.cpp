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

#include "genesis/population/formats/gff_reader.hpp"

#include <cmath>
#include <limits>

using namespace genesis::population;
using namespace genesis::utils;

TEST( GffReader, ReadGff )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/sample.gff";

    auto reader = GffReader();
    auto features = reader.read( from_file( infile ));
    ASSERT_EQ( 6, features.size() );

    // Line 1
    EXPECT_EQ( "X",                  features[0].seqname );
    EXPECT_EQ( "Ensembl",            features[0].source );
    EXPECT_EQ( "Repeat",             features[0].feature );
    EXPECT_EQ( 2419108,              features[0].start );
    EXPECT_EQ( 2419128,              features[0].end );
    EXPECT_EQ( 42,                   features[0].score );
    EXPECT_EQ( '.',                  features[0].strand );
    EXPECT_EQ( -1,                   features[0].frame );
    EXPECT_EQ( 3,                    features[0].attributes.size() );
    EXPECT_EQ( "hid",                features[0].attributes[0].first );
    EXPECT_EQ( "trf",                features[0].attributes[0].second );
    EXPECT_EQ( "hstart",             features[0].attributes[1].first );
    EXPECT_EQ( "1",                  features[0].attributes[1].second );
    EXPECT_EQ( "hend",               features[0].attributes[2].first );
    EXPECT_EQ( "21",                 features[0].attributes[2].second );

    // Line 2
    EXPECT_EQ( "X",                  features[1].seqname );
    EXPECT_EQ( "Ensembl",            features[1].source );
    EXPECT_EQ( "Repeat",             features[1].feature );
    EXPECT_EQ( 2419108,              features[1].start );
    EXPECT_EQ( 2419410,              features[1].end );
    EXPECT_EQ( 2502,                 features[1].score );
    EXPECT_EQ( '-',                  features[1].strand );
    EXPECT_EQ( -1,                   features[1].frame );
    EXPECT_EQ( 3,                    features[1].attributes.size() );
    EXPECT_EQ( "hid",                features[1].attributes[0].first );
    EXPECT_EQ( "AluSx",              features[1].attributes[0].second );
    EXPECT_EQ( "hstart",             features[1].attributes[1].first );
    EXPECT_EQ( "1",                  features[1].attributes[1].second );
    EXPECT_EQ( "hend",               features[1].attributes[2].first );
    EXPECT_EQ( "303",                features[1].attributes[2].second );

    // Line 4
    EXPECT_EQ( "X",                  features[3].seqname );
    EXPECT_EQ( "Ensembl",            features[3].source );
    EXPECT_EQ( "Pred.trans.",        features[3].feature );
    EXPECT_EQ( 2416676,              features[3].start );
    EXPECT_EQ( 2418760,              features[3].end );
    EXPECT_EQ( 450.19,               features[3].score );
    EXPECT_EQ( '-',                  features[3].strand );
    EXPECT_EQ( 2,                    features[3].frame );
    EXPECT_EQ( 1,                    features[3].attributes.size() );
    EXPECT_EQ( "genscan",            features[3].attributes[0].first );
    EXPECT_EQ( "GENSCAN00000019335", features[3].attributes[0].second );

    // Line 6
    EXPECT_EQ( "X",                  features[5].seqname );
    EXPECT_EQ( "Ensembl",            features[5].source );
    EXPECT_EQ( "Variation",          features[5].feature );
    EXPECT_EQ( 2413805,              features[5].start );
    EXPECT_EQ( 2413805,              features[5].end );
    EXPECT_TRUE( std::isnan(         features[5].score ));
    EXPECT_EQ( '+',                  features[5].strand );
    EXPECT_EQ( -1,                   features[5].frame );
    EXPECT_EQ( 0,                    features[5].attributes.size() );
}

TEST( GffReader, ReadGtf )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/sample.gtf";

    auto reader = GffReader();
    auto features = reader.read( from_file( infile ));
    ASSERT_EQ( 2, features.size() );

    // Line 1
    EXPECT_EQ( "1",                                  features[0].seqname );
    EXPECT_EQ( "transcribed_unprocessed_pseudogene", features[0].source );
    EXPECT_EQ( "gene",                               features[0].feature );
    EXPECT_EQ( 11869,                                features[0].start );
    EXPECT_EQ( 14409,                                features[0].end );
    EXPECT_TRUE( std::isnan(                         features[0].score ));
    EXPECT_EQ( '+',                                  features[0].strand );
    EXPECT_EQ( -1,                                   features[0].frame );
    EXPECT_EQ( 4,                                    features[0].attributes.size() );
    EXPECT_EQ( "gene_id",                            features[0].attributes[0].first );
    EXPECT_EQ( "ENSG00000223972",                    features[0].attributes[0].second );
    EXPECT_EQ( "gene_name",                          features[0].attributes[1].first );
    EXPECT_EQ( "DDX11L1",                            features[0].attributes[1].second );
    EXPECT_EQ( "gene_source",                        features[0].attributes[2].first );
    EXPECT_EQ( "havana",                             features[0].attributes[2].second );
    EXPECT_EQ( "gene_biotype",                       features[0].attributes[3].first );
    EXPECT_EQ( "transcribed_unprocessed_pseudogene", features[0].attributes[3].second );

    // Line 2
    EXPECT_EQ( "1",                                  features[1].seqname );
    EXPECT_EQ( "processed_transcript",               features[1].source );
    EXPECT_EQ( "transcript",                         features[1].feature );
    EXPECT_EQ( 11869,                                features[1].start );
    EXPECT_EQ( 14409,                                features[1].end );
    EXPECT_TRUE( std::isnan(                         features[1].score ));
    EXPECT_EQ( '+',                                  features[1].strand );
    EXPECT_EQ( -1,                                   features[1].frame );
    EXPECT_EQ( 7,                                    features[1].attributes.size() );
    EXPECT_EQ( "gene_id",                            features[1].attributes[0].first );
    EXPECT_EQ( "ENSG00000223972",                    features[1].attributes[0].second );
    EXPECT_EQ( "transcript_id",                      features[1].attributes[1].first );
    EXPECT_EQ( "ENST00000456328",                    features[1].attributes[1].second );
    EXPECT_EQ( "gene_name",                          features[1].attributes[2].first );
    EXPECT_EQ( "DDX11L1",                            features[1].attributes[2].second );
    EXPECT_EQ( "gene_source",                        features[1].attributes[3].first );
    EXPECT_EQ( "havana",                             features[1].attributes[3].second );
    EXPECT_EQ( "gene_biotype",                       features[1].attributes[4].first );
    EXPECT_EQ( "transcribed_unprocessed_pseudogene", features[1].attributes[4].second );
    EXPECT_EQ( "transcript_name",                    features[1].attributes[5].first );
    EXPECT_EQ( "DDX11L1-002",                        features[1].attributes[5].second );
    EXPECT_EQ( "transcript_source",                  features[1].attributes[6].first );
    EXPECT_EQ( "havana",                             features[1].attributes[6].second );
}
