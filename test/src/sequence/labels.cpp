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

#include "genesis/sequence/functions/functions.hpp"
#include "genesis/sequence/functions/labels.hpp"
#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/formats/fasta_reader.hpp"

#include <string>

using namespace genesis::sequence;

TEST( SequenceSet, FilterByLabelList )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    SequenceSet sset;
    FastaReader().from_file(infile, sset);

    // Some basic assertments.
    EXPECT_EQ( 10, sset.size() );
    EXPECT_EQ( 4600, total_length( sset ));

    // List of all labels in the SequenceSet that we want to keep.
    std::unordered_set<std::string> good_labels = {
        "Di106BGTue",
        "Di145BGTue",
        "Di307XishTrBotG",
        "cs009BGTue",
        "he005BGTue",
        "he112BGTue",
        "ne201NEStates"
    };

    // List of all labels in the SequenceSet that we want to remove.
    std::unordered_set<std::string> bad_labels = {
        "cs103MorArb",
        "he111BGTue",
        "ne203NEStates"
    };

    // Filter by removing the sequences with bad labels, then check if only the good ones are left.
    filter_by_label_list( sset, bad_labels );
    EXPECT_EQ( 7, sset.size() );
    EXPECT_EQ( good_labels, labels( sset ) );

    // Filter by removing the good labels, too. The set should be empty then.
    filter_by_label_list( sset, good_labels );
    EXPECT_EQ( 0, sset.size() );
    EXPECT_TRUE( sset.empty() );
}
