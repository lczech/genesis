/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/sequence/kmer/kmer.hpp"
#include "genesis/sequence/kmer/function.hpp"
#include "genesis/utils/math/random.hpp"

#include <fstream>
#include <string>

using namespace genesis;
using namespace genesis::sequence;
using namespace genesis::utils;

TEST( Kmer, Basics )
{
    Kmer<KmerTagDefault>::set_k( 7 );

    // GATACAC = 0b 01 00 01 00 11 00 10 = 0x1132
    auto const k1 = Kmer<KmerTagDefault>( 0x1132 );
    auto const k2 = reverse_complement( k1 );

    // LOG_DBG << "k1 " << k1 << " from " << k1.value;
    // LOG_DBG << "k2 " << k2 << " from " << k2.value;

    // Basic string operators
    EXPECT_EQ( k1, kmer_from_string<KmerTagDefault>( "GATACAC" ));
    EXPECT_EQ( k2, kmer_from_string<KmerTagDefault>( "GTGTATC" ));
    EXPECT_EQ( "GATACAC", to_string( k1 ));
    EXPECT_EQ( "GTGTATC", to_string( k2 ));

    // Canonical
    EXPECT_EQ( "GATACAC", to_string( canonical_representation( k1 )));
    EXPECT_EQ( "GATACAC", to_string( canonical_representation( k2 )));
}

TEST( Kmer, CanonicalRepresentation )
{
    // We make the canonical representation based on the high bits, so that's kinda reverse.
    // Test that we still get what we expect. Needs to be the case, as we are comparing a string
    // to its reverse-ish, so the direction of comparison should not matter.

    for( size_t k = 1; k < 10; ++k ) {
        // LOG_DBG << "at " << k;
        Kmer<KmerTagDefault>::reset_k( k );
        for( size_t i = 0; i < number_of_kmers( k ); ++i ) {

            // Test that the canonical representation is the same for the kmer and its rc.
            auto const km = Kmer<KmerTagDefault>( i );
            auto const rc = reverse_complement( km );
            // LOG_DBG1 << km << " <-> " << rc << " --> " << canonical_representation( km );
            EXPECT_EQ( canonical_representation( km ), canonical_representation( rc ));

            // Test that the canonical representation follows lexicographical ordering,
            // by actually sorting the string representation.
            auto const kms = to_string( km );
            auto const rcs = to_string( rc );
            auto const crs = kms < rcs ? kms : rcs;
            EXPECT_EQ( crs, to_string( canonical_representation( km )));
            EXPECT_EQ( crs, to_string( canonical_representation( rc )));
        }
    }
}
