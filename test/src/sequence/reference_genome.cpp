/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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

#include "genesis/sequence/reference_genome.hpp"
#include "genesis/sequence/formats/fasta_reader.hpp"

#include <string>

using namespace genesis;
using namespace genesis::sequence;

TEST( ReferenceGenome, Basics )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    auto const rg = FastaReader().read_reference_genome( utils::from_file(infile) );

    EXPECT_EQ( 10, rg.size() );
    EXPECT_TRUE( rg.contains( "Di106BGTue" ));
    EXPECT_EQ( "Di106BGTue", rg.begin()->label() );
    EXPECT_EQ( 460, rg.find( "Di106BGTue" )->length() );
    EXPECT_EQ( rg.end(), rg.find( "X" ) );
}
