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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/population/sample_counts.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/sequence/reference_genome.hpp"

using namespace genesis::population;
using namespace genesis::sequence;
using namespace genesis::utils;

TEST( Population, GuessBasesReferenceGenome )
{
    // Basic testing of the ref genome
    ReferenceGenome ref_genome;
    ref_genome.add( Sequence( "1", "AAAANSWAZK" ));
    EXPECT_ANY_THROW( ref_genome.get( "X" ));
    EXPECT_EQ( 'A', ref_genome.get( "1" )[0] );

    // Make some variants
    auto vars = std::vector<Variant>( 10 );

    // Default case, where ref base is most abundant
    vars[0].chromosome = "1";
    vars[0].position = 1;
    vars[0].reference_base = 'A';
    vars[0].samples.emplace_back();
    vars[0].samples[0].a_count = 5;
    vars[0].samples[0].c_count = 3;
    guess_and_set_ref_and_alt_bases( vars[0], ref_genome );
    EXPECT_EQ( 'A', vars[0].reference_base );
    EXPECT_EQ( 'C', vars[0].alternative_base );

    // Default case, but alt base is more abundant than ref
    vars[1].chromosome = "1";
    vars[1].position = 2;
    vars[1].reference_base = 'A';
    vars[1].samples.emplace_back();
    vars[1].samples[0].a_count = 3;
    vars[1].samples[0].c_count = 5;
    guess_and_set_ref_and_alt_bases( vars[1], ref_genome );
    EXPECT_EQ( 'A', vars[1].reference_base );
    EXPECT_EQ( 'C', vars[1].alternative_base );

    // Default case, no ref given, ref most abundant
    vars[2].chromosome = "1";
    vars[2].position = 3;
    vars[2].samples.emplace_back();
    vars[2].samples[0].a_count = 5;
    vars[2].samples[0].c_count = 3;
    guess_and_set_ref_and_alt_bases( vars[2], ref_genome );
    EXPECT_EQ( 'A', vars[2].reference_base );
    EXPECT_EQ( 'C', vars[2].alternative_base );

    // Default case, no ref given, alt most abundant
    vars[3].chromosome = "1";
    vars[3].position = 4;
    vars[3].samples.emplace_back();
    vars[3].samples[0].a_count = 3;
    vars[3].samples[0].c_count = 5;
    guess_and_set_ref_and_alt_bases( vars[3], ref_genome );
    EXPECT_EQ( 'A', vars[3].reference_base );
    EXPECT_EQ( 'C', vars[3].alternative_base );

    // No usable ref in genome
    vars[4].chromosome = "1";
    vars[4].position = 5;
    vars[4].samples.emplace_back();
    vars[4].samples[0].a_count = 3;
    vars[4].samples[0].c_count = 5;
    guess_and_set_ref_and_alt_bases( vars[4], ref_genome );
    EXPECT_EQ( 'C', vars[4].reference_base );
    EXPECT_EQ( 'A', vars[4].alternative_base );

    // Ambiguity for ref in genome
    vars[5].chromosome = "1";
    vars[5].position = 6;
    vars[5].samples.emplace_back();
    vars[5].samples[0].a_count = 3;
    vars[5].samples[0].c_count = 5;
    guess_and_set_ref_and_alt_bases( vars[5], ref_genome );
    EXPECT_EQ( 'C', vars[5].reference_base );
    EXPECT_EQ( 'A', vars[5].alternative_base );

    // Ambiguity for alt in genome
    vars[6].chromosome = "1";
    vars[6].position = 7;
    vars[6].samples.emplace_back();
    vars[6].samples[0].a_count = 3;
    vars[6].samples[0].c_count = 5;
    guess_and_set_ref_and_alt_bases( vars[6], ref_genome );
    EXPECT_EQ( 'C', vars[6].reference_base );
    EXPECT_EQ( 'A', vars[6].alternative_base );

    // Error: wrong ref base
    vars[7].chromosome = "1";
    vars[7].position = 8;
    vars[7].reference_base = 'C';
    EXPECT_ANY_THROW( guess_and_set_ref_and_alt_bases( vars[7], ref_genome ));

    // Error: Invalid ref base in genome
    vars[8].chromosome = "1";
    vars[8].position = 9;
    vars[8].samples.emplace_back();
    vars[8].samples[0].a_count = 3;
    vars[8].samples[0].c_count = 5;
    EXPECT_ANY_THROW( guess_and_set_ref_and_alt_bases( vars[8], ref_genome ));

    // Error: Non containing ambiguity base in genome
    vars[9].chromosome = "1";
    vars[9].position = 10;
    vars[9].samples.emplace_back();
    vars[9].samples[0].a_count = 3;
    vars[9].samples[0].c_count = 5;
    EXPECT_ANY_THROW( guess_and_set_ref_and_alt_bases( vars[9], ref_genome ));
}
