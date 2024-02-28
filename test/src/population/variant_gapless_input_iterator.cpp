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

#include "genesis/population/functions/functions.hpp"
#include "genesis/population/functions/genome_locus.hpp"
#include "genesis/population/genome_region.hpp"
#include "genesis/population/streams/variant_gapless_input_stream.hpp"
#include "genesis/population/streams/variant_input_stream.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/sequence/functions/dict.hpp"
#include "genesis/utils/containers/generic_input_stream.hpp"
#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/math/bitvector.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/random.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <ctime>
#include <set>
#include <map>
#include <vector>

using namespace genesis::population;
using namespace genesis::sequence;
using namespace genesis::utils;

// =================================================================================================
//     Basic Tests
// =================================================================================================

Variant test_gapless_input_stream_make_variant_( std::string const& chr, size_t pos )
{
    Variant var;
    var.chromosome = chr;
    var.position = pos;

    // Set the samples to known numbers for testing.
    // This sums to 10 counts per sample, and 30 in total for three samples.
    var.samples.resize( 3 );
    for( size_t s = 0; s < 3; ++s ) {
        var.samples[s].a_count = 1;
        var.samples[s].c_count = 2;
        var.samples[s].g_count = 3;
        var.samples[s].t_count = 4;
    }
    return var;
}

void test_gapless_input_stream_basic_(
    std::vector<Variant> const& vars,
    size_t exp_positions,
    std::shared_ptr<ReferenceGenome> ref_genome = std::shared_ptr<ReferenceGenome>{}
) {
    // Make the basis iterators that we want. The underlying data is coming from the given vector;
    // then, wrap this in the gapless iterator that we want to test.
    auto var_it = make_variant_input_stream_from_vector( vars );
    auto gapless_it = VariantGaplessInputStream( var_it );
    gapless_it.reference_genome( ref_genome );

    // Now we could already iterate over the gapless iterator with
    //     for( auto const& var : gapless_it )
    // but we want to test our wrapper function for a Generic Input Stream as well, so we wrap again.
    auto lambda_it = make_variant_input_stream_from_variant_gapless_input_stream( gapless_it );

    // Simply test that we get the expected number of variants in the iteration.
    size_t cnt = 0;
    for( auto const& var : lambda_it ) {
        (void) var;
        ++cnt;
    }
    EXPECT_EQ( exp_positions, cnt );
}

TEST( GaplessInputStream, Basics )
{
    // Empty input
    {
        std::vector<Variant> vars;
        test_gapless_input_stream_basic_( vars, 0 );
    }

    // Single at first position
    {
        std::vector<Variant> vars;
        vars.push_back( test_gapless_input_stream_make_variant_( "A", 1 ));
        test_gapless_input_stream_basic_( vars, 1 );
    }

    // Single at second position
    {
        std::vector<Variant> vars;
        vars.push_back( test_gapless_input_stream_make_variant_( "A", 2 ));
        test_gapless_input_stream_basic_( vars, 2 );
    }

    // Make a ref genome to be used.
    auto ref_genome = std::make_shared<ReferenceGenome>();
    ref_genome->add( Sequence( "A", "ACGT" ));
    ref_genome->add( Sequence( "B", "ACGT" ));

    // Empty input, but ref genome
    {
        std::vector<Variant> vars;
        test_gapless_input_stream_basic_( vars, 8, ref_genome );
    }

    // Ref genome with extra chromosomes.
    {
        std::vector<Variant> vars;
        vars.push_back( test_gapless_input_stream_make_variant_( "A", 2 ));
        test_gapless_input_stream_basic_( vars, 8, ref_genome );
    }
}

// =================================================================================================
//     Random Tests
// =================================================================================================

std::shared_ptr<ReferenceGenome> test_gapless_input_stream_make_reference_genome_(
    size_t num_reg_chrs, size_t num_ext_chrs
) {
    auto result = std::make_shared<ReferenceGenome>();

    // Helper to make a chr with 10 random positions in ACGT
    auto add_chr_ = [&]( std::string const& chr_name ){
        std::string seq;
        for( size_t i = 0; i < 10; ++i ) {
            switch( permuted_congruential_generator( 0, 3 )) {
                case 0: {
                    seq += 'A';
                    break;
                }
                case 1: {
                    seq += 'C';
                    break;
                }
                case 2: {
                    seq += 'G';
                    break;
                }
                case 3: {
                    seq += 'T';
                    break;
                }
                default: {
                    EXPECT_TRUE( false );
                }
            }
        }
        EXPECT_EQ( 10, seq.size() );
        result->add( Sequence( chr_name, seq ));
    };

    // Make the chromosomes. We name the regular ones A, B, C, and the extra ones D, E, F.
    for( size_t i = 0; i < num_reg_chrs; ++i ) {
        add_chr_( std::string( 1, 'A' + i ));
    }
    for( size_t i = 0; i < num_ext_chrs; ++i ) {
        add_chr_( std::string( 1, 'D' + i ));
    }
    EXPECT_EQ( num_reg_chrs + num_ext_chrs, result->size() );

    return result;
}

std::shared_ptr<SequenceDict> test_gapless_input_stream_make_sequence_dict_(
    std::shared_ptr<ReferenceGenome> ref_genome
) {
    auto result = std::make_shared<SequenceDict>(
        reference_genome_to_dict( *ref_genome )
    );
    EXPECT_EQ( ref_genome->size(), result->size() );
    return result;
}

std::map<std::string, Bitvector> test_gapless_input_stream_make_variant_bitvectors_(
    size_t num_reg_chrs
) {
    // Make random bitvectors for each chromosome, with random positions being set
    // that will indicate wheather we generate a variant there, or leave it missing.
    // We always use 10 positions, and fill anything from 1 to 10 of them.
    // Having these bitvectors makes it easy later on to test that the right positions
    // are appearing in the iterator.
    std::map<std::string, Bitvector> result;
    for( size_t i = 0; i < num_reg_chrs; ++i ) {
        auto bv = Bitvector( 10 );
        auto const filled = permuted_congruential_generator( 1, 10 );
        auto const selection = select_without_replacement( filled, 10 );
        for( auto s : selection ) {
            bv.set(s);
        }
        EXPECT_EQ( filled, bv.count() );
        result[std::string( 1, 'A' + i )] = bv;
    }
    return result;
}

std::vector<Variant> test_gapless_input_stream_make_variants_(
    std::map<std::string, Bitvector> const& bitvectors,
    std::shared_ptr<ReferenceGenome> ref_genome
) {
    // Iterate the bitvectors and create variants where they are true,
    // using the ref bases of the given genome
    std::vector<Variant> result;
    size_t total_variants = 0;
    for( auto const& bv : bitvectors ) {
        total_variants += bv.second.count();

        for( size_t i = 0; i < bv.second.size(); ++i ) {
            if( !bv.second.get(i) ) {
                continue;
            }

            auto var = test_gapless_input_stream_make_variant_( bv.first, i + 1 );
            var.reference_base = ref_genome->get_base( var.chromosome, var.position );
            result.push_back( var );
        }
    }
    EXPECT_EQ( total_variants, result.size() );
    return result;
}

size_t find_position_past_last_true_( Bitvector const& bv )
{
    // Simple helper to find the position index past the last one that is set to true.
    // For instance bitvector `001100` has the last true at position 3 (0-based),
    // so then the function returns 4. For an all-false bitvector, the function hence returns 0.
    for( size_t i = 0; i < bv.size(); ++i ) {
        auto const r = bv.size() - 1 - i;
        if( bv.get(r) ) {
            return r + 1;
        }
    }
    return 0;
}

void test_gapless_input_stream_random_()
{
    // How many chromosomes as part of the iterator and as part of the ref genome / seq dict?
    // We are using up to three chrs, so that we test having a "middle" one, which might be
    // an important transition for the iterator to do correctly.
    auto const num_reg_chrs = permuted_congruential_generator( 1, 3 );
    auto const num_ext_chrs = permuted_congruential_generator( 0, 3 );

    // We now make a ref genome, which we always use to have random sequences for our variants.
    // We also make a seq dict from it, just to have that available as well.
    // We make extras here, just in case to test this, even if they are not used in the end.
    auto const ref_genome = test_gapless_input_stream_make_reference_genome_(
        num_reg_chrs, num_ext_chrs
    );
    auto const seq_dict = test_gapless_input_stream_make_sequence_dict_( ref_genome );

    // Are we actually using a ref genome or seq dict at all?
    bool use_ref_gen = false;
    bool use_seq_dct = false;
    switch( permuted_congruential_generator( 0, 2 )) {
        case 1: {
            use_ref_gen = true;
            break;
        }
        case 2: {
            use_seq_dct = true;
            break;
        }
    }

    // Are we setting the iterate_extra_chromosomes setting?
    // If not, we do add the above ref genome or seq dict,
    // but it shall not be used to iterate the extra chrs.
    bool const extra_chrs = permuted_congruential_generator_bool();

    // Now make variants for all chrs that we want. We do this via a set of bitvectors,
    // randomly set and indicating for which positions we want to have varians, with all
    // others missing, so that we can actually test the behavior of the iterator.
    auto const var_bvs = test_gapless_input_stream_make_variant_bitvectors_( num_reg_chrs );
    auto const vars = test_gapless_input_stream_make_variants_( var_bvs, ref_genome );

    // Debug output
    LOG_DBG << "num_reg_chrs " << num_reg_chrs;
    LOG_DBG << "num_ext_chrs " << num_ext_chrs;
    LOG_DBG << "use_ref_gen " << use_ref_gen;
    LOG_DBG << "use_seq_dct " << use_seq_dct;
    LOG_DBG << "extra_chrs " << extra_chrs;
    if( ref_genome ) {
        LOG_DBG << "ref_genome";
        for( auto const& chr : *ref_genome ) {
            LOG_DBG1 << chr.label() << ":" << chr.length();
        }
    }
    if( seq_dict ) {
        LOG_DBG << "seq_dict";
        for( auto const& chr : *seq_dict ) {
            LOG_DBG1 << chr.name << ":" << chr.length;
        }
    }
    LOG_DBG << "var_bvs";
    for( auto const& fp : var_bvs ) {
        LOG_DBG1 << fp.first << ":" << fp.second.dump();
    }
    LOG_DBG << "vars";
    for( auto const& var : vars ) {
        LOG_DBG1 << var.chromosome << ":" << var.position;
    }

    // Now we are ready for the iteration. Set up everything.
    auto var_it = make_variant_input_stream_from_vector( vars );
    auto gapless_it = VariantGaplessInputStream( var_it );
    if( use_ref_gen ) {
        gapless_it.reference_genome( ref_genome );
    }
    if( use_seq_dct ) {
        gapless_it.sequence_dict( seq_dict );
    }
    gapless_it.iterate_extra_chromosomes( extra_chrs );

    // We have some stuff to keep track of.
    size_t total_variants = 0;
    size_t present_variants = 0;
    size_t missing_variants = 0;
    std::map<std::string, Bitvector> found_positions;
    for( size_t i = 0; i < num_reg_chrs; ++i ) {
        found_positions[ std::string( 1, 'A' + i )] = Bitvector(10);
    }
    if( extra_chrs && ( use_ref_gen || use_seq_dct )) {
        for( size_t i = 0; i < num_ext_chrs; ++i ) {
            found_positions[ std::string( 1, 'D' + i )] = Bitvector(10);
        }
    }

    // Finally, we are running the iteration.
    LOG_DBG << "loop";
    for( auto const& var : gapless_it ) {
        LOG_DBG1 << "at " << var.chromosome << ":" << var.position;
        ++total_variants;

        // Check the variant position
        EXPECT_GE( var.position, 1 );
        EXPECT_LE( var.position, 10 );
        ASSERT_TRUE( found_positions.count(var.chromosome) > 0 );
        EXPECT_FALSE( found_positions[var.chromosome].get( var.position - 1 ));
        if( var.position > 1 ) {
            // The previous one must be true, as we are gapless!
            EXPECT_TRUE( found_positions[var.chromosome].get( var.position - 2 ));
        }
        found_positions[var.chromosome].set( var.position - 1 );

        // Check the content, and check against the bitvector
        EXPECT_EQ( 3, var.samples.size() );
        auto const nt_sum = total_nucleotide_sum(var);
        EXPECT_TRUE( nt_sum == 0 || nt_sum == 30 );
        if( var_bvs.count(var.chromosome) > 0 && var_bvs.at(var.chromosome).get( var.position - 1 )) {
            EXPECT_EQ( 30, nt_sum );
            EXPECT_EQ( ref_genome->get_base( var.chromosome, var.position ), var.reference_base );
        } else {
            EXPECT_EQ( 0, nt_sum );
            if( use_ref_gen ) {
                EXPECT_EQ( ref_genome->get_base( var.chromosome, var.position ), var.reference_base );
            } else {
                EXPECT_EQ( 'N', var.reference_base );
            }
        }
        if( nt_sum == 0 ) {
            ++missing_variants;
        } else {
            ++present_variants;
        }
    }

    // Overall checks
    EXPECT_EQ( num_reg_chrs, var_bvs.size() );
    if( extra_chrs && ( use_ref_gen || use_seq_dct )) {
        EXPECT_EQ( num_reg_chrs + num_ext_chrs, found_positions.size() );
    } else {
        EXPECT_EQ( num_reg_chrs, found_positions.size() );
    }
    EXPECT_EQ( total_variants, present_variants + missing_variants );
    for( auto const& fp : found_positions ) {
        EXPECT_GT( fp.second.count(), 0 );
    }

    // Check found positions
    if( use_ref_gen || use_seq_dct ) {
        // If we have set a ref genome or seq dict, we always iterate until the end of each chr.
        // If also we do extra chrs, we need to account for those as well.
        if( extra_chrs ) {
            EXPECT_EQ( ref_genome->size(), found_positions.size() );
        } else {
            EXPECT_EQ( num_reg_chrs, found_positions.size() );
        }
        if( extra_chrs ) {
            EXPECT_EQ( (num_reg_chrs + num_ext_chrs) * 10, total_variants );
        } else {
            EXPECT_EQ( num_reg_chrs * 10, total_variants );
        }
        for( auto const& fp : found_positions ) {
            EXPECT_EQ( 10, fp.second.count() );
        }
    } else {
        // Without ref genome or seq dict, we only see the positions up until the last in the
        // input. For this, scan the original bitvectors to find the respecitive last pos.
        EXPECT_EQ( var_bvs.size(), found_positions.size() );
        size_t exp_total_variants = 0;
        for( auto const& bv : var_bvs ) {
            auto const past_last = find_position_past_last_true_(bv.second);
            exp_total_variants += past_last;
            EXPECT_TRUE( found_positions.count(bv.first) > 0 );
            EXPECT_EQ( past_last, found_positions[bv.first].count() );

            // Now scan the positions. All before the last need to true, all after false.
            EXPECT_LE( past_last, 10 );
            for( size_t i = 0; i < past_last; ++i ) {
                EXPECT_TRUE( found_positions[bv.first].get(i) );
            }
            for( size_t i = past_last; i < 10; ++i ) {
                EXPECT_FALSE( found_positions[bv.first].get(i) );
            }
        }
        EXPECT_EQ( exp_total_variants, total_variants );
    }

    // Count up all the set positions in the original bitvectors to get the present variant count.
    size_t exp_present_variants = 0;
    for( auto const& bv : var_bvs ) {
        exp_present_variants += bv.second.count();
    }
    EXPECT_EQ( exp_present_variants, present_variants );
}

TEST( GaplessInputStream, Random )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    // auto const seed = ::time(nullptr);
    auto const seed = 1708230970;
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // For the duration of the test, we deactivate debug logging.
    // But if needed, comment this line out, and each test will report its input.
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    // 0.5s runtime, our default for normal tests.
    size_t const max_tests = 5000;

    // Run tests while we have time.
    for( size_t test_num = 0; test_num < max_tests; ++test_num ) {
        LOG_DBG << "=================================";
        LOG_DBG << "Test " << test_num;
        test_gapless_input_stream_random_();
    }
}
