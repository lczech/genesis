/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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

using namespace genesis;
using namespace genesis::sequence;

TEST( SequenceSet, FilterByLabelList )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Load sequence file.
    std::string infile = environment->data_dir + "sequence/dna_10.fasta";
    SequenceSet sset;
    FastaReader().read( utils::from_file(infile), sset);

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

TEST( Sequence, GuessAbundances )
{
    std::pair<std::string, size_t> const good = { "abc", 123 };

    EXPECT_EQ( good, guess_sequence_abundance( "abc_123" ));
    EXPECT_EQ( good, guess_sequence_abundance( "abc;size=123;" ));
    EXPECT_EQ( good, guess_sequence_abundance( "abc;size=123" ));
    EXPECT_EQ( good, guess_sequence_abundance( "abc;key=value;size=123;foo=bar;" ));
    // EXPECT_EQ( good, guess_sequence_abundance( "abc_size=123_" ));
    // EXPECT_EQ( good, guess_sequence_abundance( "abcsize=123" ));
    // EXPECT_EQ( good, guess_sequence_abundance( "abc;size=123x" ));

    EXPECT_EQ(
        std::make_pair( std::string{ "abc_size=123_" }, size_t{1} ),
        guess_sequence_abundance( "abc_size=123_" )
    );
    EXPECT_EQ(
        std::make_pair( std::string{ "abcsize=123" }, size_t{1} ),
        guess_sequence_abundance( "abcsize=123" )
    );
    EXPECT_EQ(
        std::make_pair( std::string{ "abc" }, size_t{1} ),
        guess_sequence_abundance( "abc;size=123x" )
    );

    EXPECT_EQ(
        std::make_pair( std::string{ "abc_" },       size_t{1} ),
        guess_sequence_abundance( "abc_" )
    );
    EXPECT_EQ(
        std::make_pair( std::string{ "abc;size=" },  size_t{1} ),
        guess_sequence_abundance( "abc;size=" )
    );
    EXPECT_EQ(
        std::make_pair( std::string{ "abc_123x" },   size_t{1} ),
        guess_sequence_abundance( "abc_123x" )
    );
    EXPECT_EQ(
        std::make_pair( std::string{ "abc_x" },      size_t{1} ),
        guess_sequence_abundance( "abc_x" )
    );
    EXPECT_EQ(
        std::make_pair( std::string{ "abc" }, size_t{1} ),
        guess_sequence_abundance( "abc;size=x" )
    );
}

TEST( Sequence, LabelAttributes )
{
    LabelAttributes exp1;
    exp1.label = "bla";
    exp1.attributes = { {"size", "123"}, {"weight", "100"} };
    auto const act1 = label_attributes( "bla;size=123;weight=100;" );
    EXPECT_EQ( exp1.label, act1.label );
    EXPECT_EQ( exp1.attributes, act1.attributes );

    LabelAttributes exp2;
    exp2.label = "bla";
    exp2.attributes = {};
    auto const act2 = label_attributes( "bla;" );
    EXPECT_EQ( exp2.label, act2.label );
    EXPECT_EQ( exp2.attributes, act2.attributes );

    EXPECT_ANY_THROW( label_attributes( "bla;foo" ) );
}
