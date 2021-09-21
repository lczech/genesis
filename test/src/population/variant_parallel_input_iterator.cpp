/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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

#include "genesis/population/formats/variant_parallel_input_iterator.hpp"
#include "genesis/utils/core/algorithm.hpp"

#include <set>

using namespace genesis::population;
using namespace genesis::utils;

void test_parallel_input_iterator_(
    VariantParallelInputIterator::ContributionType p_sel,
    VariantParallelInputIterator::ContributionType s_sel,
    VariantParallelInputIterator::ContributionType v_sel,
    std::set<size_t> expected_positions,
    std::set<size_t> additional_loci = {}
) {
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;
    std::string const p_infile = environment->data_dir + "population/parallel.pileup";
    std::string const s_infile = environment->data_dir + "population/parallel.sync";
    std::string const v_infile = environment->data_dir + "population/parallel.vcf";

    // Init with the desired settings
    auto pit = VariantParallelInputIterator();
    pit.add_variant_input_iterator(
        make_variant_input_iterator_from_pileup_file( p_infile ),
        p_sel
    );
    pit.add_variant_input_iterator(
        make_variant_input_iterator_from_sync_file( s_infile ),
        s_sel
    );
    pit.add_variant_input_iterator(
        make_variant_input_iterator_from_vcf_file( v_infile ),
        v_sel
    );

    // Add the addtional loci, if present. If all of the input sources are following, but
    // we have additional carrying loci, we do only visit those, and hence need to clear the
    // list of expected postions from the intersection of the input sources first.
    if(
        p_sel == VariantParallelInputIterator::ContributionType::kFollowing &&
        s_sel == VariantParallelInputIterator::ContributionType::kFollowing &&
        v_sel == VariantParallelInputIterator::ContributionType::kFollowing &&
        ! additional_loci.empty()
    ) {
        expected_positions.clear();
    }
    for( auto const& al : additional_loci ) {
        pit.add_carrying_locus( GenomeLocus( "XYZ", al ));
        expected_positions.insert( al );
    }

    std::set<size_t> found_positions;
    for( auto it = pit.begin(); it != pit.end(); ++it ) {
        // LOG_DBG2 << it.locus().to_string();
        found_positions.insert( it.locus().position );

        // Make sure that all inputs have either no data, or the correct number of samples.
        EXPECT_TRUE( ! it.variants()[0] || it.variants()[0]->samples.size() == 1 );
        EXPECT_TRUE( ! it.variants()[1] || it.variants()[1]->samples.size() == 2 );
        EXPECT_TRUE( ! it.variants()[2] || it.variants()[2]->samples.size() == 3 );

        // Same with a differnt way of access.
        EXPECT_TRUE( ! it.variant_at(0) || it.variant_at(0)->samples.size() == 1 );
        EXPECT_TRUE( ! it.variant_at(1) || it.variant_at(1)->samples.size() == 2 );
        EXPECT_TRUE( ! it.variant_at(2) || it.variant_at(2)->samples.size() == 3 );

        // // Yet another way. Here we test that if the iterator is type following,
        // // we must have data, as otherwise we would not have stopped here.
        // // Nope, not true, all can have all.
        // if( p_sel == VariantParallelInputIterator::ContributionType::kFollowing ) {
        //     EXPECT_TRUE( it.variant_at(0) );
        // }
        // if( s_sel == VariantParallelInputIterator::ContributionType::kFollowing ) {
        //     EXPECT_TRUE( it.variant_at(1) );
        // }
        // if( v_sel == VariantParallelInputIterator::ContributionType::kFollowing ) {
        //     EXPECT_TRUE( it.variant_at(2) );
        // }

        // Traverse the data. Test from the class documentation.
        for( auto const& var : *it ) {
            if( var ) {
                // Dummy test
                EXPECT_TRUE( var->samples.size() <= 3 );
            }
        }

        // Also test this using the merged variants.
        // We need to ignore alt bases here, as not all input sources have them.
        // We also test with move as the very last thing, to make sure that this
        // does not give any weird segfaults.
        EXPECT_EQ( 6, it.joined_variant().samples.size() );
        EXPECT_EQ( 6, it.joined_variant( false, true, true ).samples.size() );
    }
    EXPECT_EQ( expected_positions, found_positions );
}

void test_parallel_input_iterator_all_(
    std::set<size_t> additional_loci = {}
) {
    //  Key to which input file has which positions
    //  Pos   P   S   V
    //  5     x       x
    //  8     x
    //  10    x   x   x
    //  12        x
    //  15    x       x
    //  17    x   x
    //  20            x
    //  22    x
    //  25        x   x
    //  28    x   x
    //  30        x

    // Test out all compbinations of carrying and following iterators.

    // LOG_DBG1 << "UUU";
    test_parallel_input_iterator_(
        VariantParallelInputIterator::ContributionType::kCarrying,
        VariantParallelInputIterator::ContributionType::kCarrying,
        VariantParallelInputIterator::ContributionType::kCarrying,
        std::set<size_t>{ 5, 8, 10, 12, 15, 17, 20, 22, 25, 28, 30 },
        additional_loci
    );
    // LOG_DBG1 << "UUI";
    test_parallel_input_iterator_(
        VariantParallelInputIterator::ContributionType::kCarrying,
        VariantParallelInputIterator::ContributionType::kCarrying,
        VariantParallelInputIterator::ContributionType::kFollowing,
        std::set<size_t>{ 5, 8, 10, 12, 15, 17, 22, 25, 28, 30 },
        additional_loci
    );
    // LOG_DBG1 << "UIU";
    test_parallel_input_iterator_(
        VariantParallelInputIterator::ContributionType::kCarrying,
        VariantParallelInputIterator::ContributionType::kFollowing,
        VariantParallelInputIterator::ContributionType::kCarrying,
        std::set<size_t>{ 5, 8, 10, 15, 17, 20, 22, 25, 28 },
        additional_loci
    );
    // LOG_DBG1 << "IUU";
    test_parallel_input_iterator_(
        VariantParallelInputIterator::ContributionType::kFollowing,
        VariantParallelInputIterator::ContributionType::kCarrying,
        VariantParallelInputIterator::ContributionType::kCarrying,
        std::set<size_t>{ 5, 10, 12, 15, 17, 20, 25, 28, 30 },
        additional_loci
    );
    // LOG_DBG1 << "UII";
    test_parallel_input_iterator_(
        VariantParallelInputIterator::ContributionType::kCarrying,
        VariantParallelInputIterator::ContributionType::kFollowing,
        VariantParallelInputIterator::ContributionType::kFollowing,
        std::set<size_t>{ 5, 8, 10, 15, 17, 22, 28 },
        additional_loci
    );
    // LOG_DBG1 << "IUI";
    test_parallel_input_iterator_(
        VariantParallelInputIterator::ContributionType::kFollowing,
        VariantParallelInputIterator::ContributionType::kCarrying,
        VariantParallelInputIterator::ContributionType::kFollowing,
        std::set<size_t>{ 10, 12, 17, 25, 28, 30 },
        additional_loci
    );
    // LOG_DBG1 << "IIU";
    test_parallel_input_iterator_(
        VariantParallelInputIterator::ContributionType::kFollowing,
        VariantParallelInputIterator::ContributionType::kFollowing,
        VariantParallelInputIterator::ContributionType::kCarrying,
        std::set<size_t>{ 5, 10, 15, 20, 25 },
        additional_loci
    );
    // LOG_DBG1 << "III";
    test_parallel_input_iterator_(
        VariantParallelInputIterator::ContributionType::kFollowing,
        VariantParallelInputIterator::ContributionType::kFollowing,
        VariantParallelInputIterator::ContributionType::kFollowing,
        std::set<size_t>{ 10 },
        additional_loci
    );
}

TEST( Variant, ParallelInputIterator )
{
    // Test without additional loci.
    // LOG_DBG << "Normal";
    test_parallel_input_iterator_all_();

    // Test with different positions and numbers of additional loci.
    // LOG_DBG << "1";
    test_parallel_input_iterator_all_({ 1 });
    // LOG_DBG << "15";
    test_parallel_input_iterator_all_({ 15 });
    // LOG_DBG << "16";
    test_parallel_input_iterator_all_({ 16 });
    // LOG_DBG << "32";
    test_parallel_input_iterator_all_({ 32 });
    // LOG_DBG << "1, 15, 32";
    test_parallel_input_iterator_all_({ 1, 15, 32 });
    // LOG_DBG << "15, 32";
    test_parallel_input_iterator_all_({ 15, 32 });
}
