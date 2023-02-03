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

#include "genesis/sequence/formats/fasta_reader.hpp"
#include "genesis/sequence/sequence_dict.hpp"
#include "genesis/sequence/functions/dict.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/io/gzip_input_source.hpp"

#include <fstream>
#include <string>

using namespace genesis;
using namespace genesis::sequence;

TEST( SequenceDict, FastaReader )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read sequence dict file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    auto const dict = FastaReader().read_dict( utils::from_file( infile ));

    // Check data.
    ASSERT_EQ(           10, dict.size() );
    EXPECT_EQ( "Di106BGTue", dict[0].name );
    EXPECT_EQ(          460, dict[0].length );
}

TEST( SequenceDict, FastaConvert )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read sequence dict file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    auto const seqs = FastaReader().read( utils::from_file( infile ));
    auto const dict = sequence_set_to_dict( seqs );

    // Check data.
    ASSERT_EQ(           10, dict.size() );
    EXPECT_EQ( "Di106BGTue", dict[0].name );
    EXPECT_EQ(          460, dict[0].length );
    EXPECT_TRUE( verify( dict, seqs, false ));
    EXPECT_TRUE( verify( dict, seqs, true ));
}
