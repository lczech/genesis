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

#include "genesis/population/sample_counts.hpp"
#include "genesis/population/format/simple_pileup_input_stream.hpp"
#include "genesis/population/format/simple_pileup_reader.hpp"
#include "genesis/population/format/sync_reader.hpp"
#include "genesis/population/stream/variant_input_stream.hpp"
#include "genesis/population/stream/variant_input_stream_sources.hpp"
#include "genesis/population/stream/variant_input_stream_adapters.hpp"
#include "genesis/population/filter/sample_counts_filter_numerical.hpp"
#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/filter/variant_filter_numerical.hpp"
#include "genesis/population/filter/variant_filter.hpp"
#include "genesis/population/function/fst_pool_functions.hpp"
#include "genesis/population/function/fst_pool_processor.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/population/window/interval_window_stream.hpp"
#include "genesis/population/window/sliding_window_generator.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/utils/containers/transform_iterator.hpp"
#include "genesis/utils/core/options.hpp"
#include "genesis/utils/math/random.hpp"

using namespace genesis::population;
using namespace genesis::utils;

// =================================================================================================
//     Generator (deprecated)
// =================================================================================================

TEST( FST, FstPoolGenerator )
{
    // Equivalent PoPoolation call for conventional F_ST:
    // perl fst-sliding.pl --input p1_p2.sync --output p1_p2.fst_conventional --suppress-noninformative --min-count 6 --min-coverage 50 --max-coverage 200 --min-covered-fraction 1 --window-size 100 --step-size 100 --pool-size 500 > log_conventional.txt

    // Equivalent PoPoolation call for Karlsson F_ST:
    // perl fst-sliding.pl --input p1_p2.sync --karlsson-fst --output p1_p2.fst_karlsson --suppress-noninformative --min-count 6 --min-coverage 50 --max-coverage 200 --min-covered-fraction 1 --window-size 100 --step-size 100 --pool-size 500 > log_karlsson.txt

    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/p1_p2.sync.gz";

    // Settings
    size_t const poolsize = 500;
    size_t const min_allele_count = 6;
    size_t const min_read_depth = 50;
    size_t const max_read_depth = 200;
    size_t const window_width = 100;
    size_t const window_stride = 100;
    // settings.min_read_depth_fraction = 1.0;
    // settings.window_width = 100;
    // settings.window_stride = 100;
    // settings.min_phred_score = 20;

    // Expected values.
    std::vector<double> const exp_kofler = {{
        0.01533591, 0.01340363, 0.01554609, 0.01454173, 0.01317223, 0.01554917,
        0.01202964, 0.01316962, 0.01317223, 0.01316962, 0.01778599, 0.01554609,
        0.01554917, 0.00732000, 0.014416005, // <-- slighly changed due to rounding
        0.01554609, 0.01690169, 0.01183717, 0.01316962, 0.02049937, 0.01584036,
        0.014527005, // <-- slighly changed due to rounding
        0.01316962, 0.01196782, 0.01676964, 0.01210121, 0.01690169, 0.01554609,
        0.01601910, 0.019098585 // <-- not computed by PoPoolation (incomplete window)
    }};
    std::vector<double> const exp_karlsson = {{
        0.02042334, 0.01646975, 0.02125242, 0.01905364, 0.01652186, 0.02120793,
        0.014834166, // <-- slighly changed due to rounding
        0.01646975, 0.01652186, 0.01646975, 0.02709343, 0.02125242, 0.02120793,
        0.004709437, // <-- slighly changed due to rounding
        0.01905562, 0.02125242, 0.02347631, 0.01483574, 0.01646975, 0.03265680,
        0.02125242, 0.01905562, 0.01646975, 0.01444498, 0.02410646, 0.01405358,
        0.02347631, 0.02125242, 0.02240403,
        0.027800744 // <-- not computed by PoPoolation (incomplete window)
    }};
    std::vector<double> const exp_unbiased_nei = {{
        0.00931612172, 0.00730230879, 0.00973944028, 0.00861753481, 0.00732880303, 0.0097167155,
        0.00647155422, 0.00730230879, 0.00732880303, 0.00730230879, 0.0127319208,  0.00973944028,
        0.0097167155,  0.00135927851, 0.00861854607, 0.00973944028, 0.0108767017,  0.00647235444,
        0.00730230879, 0.0155986994,  0.00973944028, 0.00861854607, 0.00730230879, 0.00627407827,
        0.0111994211,  0.00607555685, 0.0108767017,  0.00973944028, 0.0103280389,  0.0130954999
    }};
    std::vector<double> const exp_unbiased_hudson = {{
        0.0184602654, 0.0144987433,  0.019290997,  0.0170878148, 0.0145509649, 0.019246419,
        0.012859885,  0.0144987433,  0.0145509649, 0.0144987433, 0.0251437138, 0.019290997,
        0.019246419,  0.00271486675, 0.0170898029, 0.019290997,  0.0215193438, 0.0128614649,
        0.0144987433, 0.0307182342,  0.019290997,  0.0170898029, 0.0144987433, 0.0124699193,
        0.0221507664, 0.0120777347,  0.0215193438, 0.019290997,  0.0204449219, 0.025852449
    }};

    // Prepare the window.
    size_t cnt = 0;
    using WindowGen = SlidingWindowGenerator<std::vector<SampleCounts>>;
    WindowGen window_gen( SlidingWindowType::kInterval, window_width, window_stride );
    // window_gen.anchor_type( WindowAnchorType::kIntervalMidpoint );
    window_gen.add_emission_plugin( [&]( WindowGen::Window const& window ) {
        if( window.entry_count() == 0 ) {
            return;
        }
        // LOG_DBG << "position " << window.first_position();
        // LOG_DBG << "cnt " << cnt;

        // Get the two populations from the range.
        // Unfortunately, we need to versions of this, one that just gives the counts,
        // and one that filters min counts, as PoPoolation differs in their implementation.
        auto pop1 = make_transform_range(
            []( WindowGen::Window::Entry const& entry ) -> SampleCounts const& {
                // LOG_DBG1 << "pop1 " << entry.position << " " << entry.data[0];
                return entry.data[0];
            },
            window
        );
        auto pop2 = make_transform_range(
            []( WindowGen::Window::Entry const& entry ) -> SampleCounts const& {
                // LOG_DBG1 << "pop2 " << entry.position << " " << entry.data[1];
                return entry.data[1];
            },
            window
        );
        auto pop1_filt = make_transform_range(
            [&]( WindowGen::Window::Entry const& entry ) {
                auto copy = entry.data[0];
                transform_zero_out_by_min_count( copy, min_allele_count );
                return copy;
            },
            window
        );
        auto pop2_filt = make_transform_range(
            [&]( WindowGen::Window::Entry const& entry ) {
                auto copy = entry.data[1];
                transform_zero_out_by_min_count( copy, min_allele_count );
                return copy;
            },
            window
        );

        // Compute the statistics
        // LOG_DBG << "f_st_pool_kofler";
        auto const fst_conv = f_st_pool_kofler(
            poolsize, poolsize,
            pop1.begin(), pop1.end(),
            pop2.begin(), pop2.end()
        );
        // LOG_DBG << "f_st_pool_karlsson";
        auto const fst_asym_unbiased = f_st_pool_karlsson(
            pop1_filt.begin(), pop1_filt.end(),
            pop2_filt.begin(), pop2_filt.end()
        );
        // LOG_DBG << "f_st_pool_unbiased";
        auto const fst_unbiased = f_st_pool_unbiased(
            poolsize, poolsize,
            pop1_filt.begin(), pop1_filt.end(),
            pop2_filt.begin(), pop2_filt.end()
        );

        // LOG_DBG << std::setprecision(9)
        //         << "kofler " << fst_conv
        //         << " karlsson " << fst_asym_unbiased
        //         << " unbiased " << fst_unbiased.first << " & " << fst_unbiased.second
        // ;

        // Compare statistics
        EXPECT_FLOAT_EQ( exp_kofler[cnt], fst_conv );
        EXPECT_FLOAT_EQ( exp_karlsson[cnt], fst_asym_unbiased );
        EXPECT_FLOAT_EQ( exp_unbiased_nei[cnt], fst_unbiased.first );
        EXPECT_FLOAT_EQ( exp_unbiased_hudson[cnt], fst_unbiased.second );
        ++cnt;
    });

    // Process the file.
    auto insource = from_file( infile );
    InputStream instream( insource );
    auto reader = SyncReader();
    Variant sample_set;
    while( reader.parse_line( instream, sample_set )) {
        EXPECT_EQ( 2, sample_set.samples.size() );

        // transform_by_min_count( sample_set.samples[0], min_allele_count );
        // transform_by_min_count( sample_set.samples[1], min_allele_count );

        // Ugly relic of many refactorings to do it this way... but good enough for now.
        auto merged = merge( sample_set.samples, SampleCountsFilterPolicy::kOnlyPassing );
        SampleCountsFilterNumericalParams filter;
        filter.min_count = min_allele_count;
        filter.min_read_depth = min_read_depth;
        filter.max_read_depth = max_read_depth;
        filter.only_biallelic_snps = true;
        if( apply_sample_counts_filter_numerical( merged, filter )) {
            window_gen.enqueue( sample_set.chromosome, sample_set.position, sample_set.samples );
        }

        // Old way of checking the status directly
        // if( status(
        //         merge( sample_set.samples ),
        //         min_read_depth, max_read_depth, min_allele_count
        //     ).is_biallelic
        // ) {
        //     // LOG_DBG << "enq " <<  sample_set.position << " " << merge( sample_set.samples );
        //     window_gen.enqueue( sample_set.chromosome, sample_set.position, sample_set.samples );
        // }
    }
}

// =================================================================================================
//     Stream
// =================================================================================================

TEST( FST, FstPoolIterator )
{
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/p1_p2.sync.gz";

    // Settings
    size_t const poolsize = 500;
    size_t const min_allele_count = 6;
    size_t const min_read_depth = 50;
    size_t const max_read_depth = 200;
    size_t const window_width = 100;
    size_t const window_stride = 100;
    // settings.min_read_depth_fraction = 1.0;
    // settings.window_width = 100;
    // settings.window_stride = 100;
    // settings.min_phred_score = 20;

    // Expected values.
    std::vector<double> const exp_kofler = {{
        0.01533591, 0.01340363, 0.01554609, 0.01454173, 0.01317223, 0.01554917,
        0.01202964, 0.01316962, 0.01317223, 0.01316962, 0.01778599, 0.01554609,
        0.01554917, 0.00732000, 0.014416005, // <-- slighly changed due to rounding
        0.01554609, 0.01690169, 0.01183717, 0.01316962, 0.02049937, 0.01584036,
        0.014527005, // <-- slighly changed due to rounding
        0.01316962, 0.01196782, 0.01676964, 0.01210121, 0.01690169, 0.01554609,
        0.01601910, 0.019098585 // <-- not computed by PoPoolation (incomplete window)
    }};
    std::vector<double> const exp_karlsson = {{
        0.02042334, 0.01646975, 0.02125242, 0.01905364, 0.01652186, 0.02120793,
        0.014834166, // <-- slighly changed due to rounding
        0.01646975, 0.01652186, 0.01646975, 0.02709343, 0.02125242, 0.02120793,
        0.004709437, // <-- slighly changed due to rounding
        0.01905562, 0.02125242, 0.02347631, 0.01483574, 0.01646975, 0.03265680,
        0.02125242, 0.01905562, 0.01646975, 0.01444498, 0.02410646, 0.01405358,
        0.02347631, 0.02125242, 0.02240403,
        0.027800744 // <-- not computed by PoPoolation (incomplete window)
    }};
    std::vector<double> const exp_unbiased_nei = {{
        0.00931612172, 0.00730230879, 0.00973944028, 0.00861753481, 0.00732880303, 0.0097167155,
        0.00647155422, 0.00730230879, 0.00732880303, 0.00730230879, 0.0127319208,  0.00973944028,
        0.0097167155,  0.00135927851, 0.00861854607, 0.00973944028, 0.0108767017,  0.00647235444,
        0.00730230879, 0.0155986994,  0.00973944028, 0.00861854607, 0.00730230879, 0.00627407827,
        0.0111994211,  0.00607555685, 0.0108767017,  0.00973944028, 0.0103280389,  0.0130954999
    }};
    std::vector<double> const exp_unbiased_hudson = {{
        0.0184602654, 0.0144987433,  0.019290997,  0.0170878148, 0.0145509649, 0.019246419,
        0.012859885,  0.0144987433,  0.0145509649, 0.0144987433, 0.0251437138, 0.019290997,
        0.019246419,  0.00271486675, 0.0170898029, 0.019290997,  0.0215193438, 0.0128614649,
        0.0144987433, 0.0307182342,  0.019290997,  0.0170898029, 0.0144987433, 0.0124699193,
        0.0221507664, 0.0120777347,  0.0215193438, 0.019290997,  0.0204449219, 0.025852449
    }};

    using VariantWindow = Window<genesis::population::Variant>;

    // Make a Generic Input Stream over the data stream.
    auto data_gen = make_variant_input_stream_from_sync_file( infile );
    data_gen.add_filter([&]( Variant const& variant ){
        // transform_by_min_count( sample_set.samples[0], min_allele_count );
        // transform_by_min_count( sample_set.samples[1], min_allele_count );

        // Ugly relic of many refactorings to do it this way... but good enough for now.
        auto merged = merge( variant.samples, SampleCountsFilterPolicy::kOnlyPassing );
        SampleCountsFilterNumericalParams filter;
        filter.min_count = min_allele_count;
        filter.min_read_depth = min_read_depth;
        filter.max_read_depth = max_read_depth;
        filter.only_biallelic_snps = true;
        return apply_sample_counts_filter_numerical( merged, filter );

        // Old way of checking the status directly
        // return status(
        //     merge( variant.samples ),
        //     min_read_depth, max_read_depth, min_allele_count
        // ).is_biallelic;
    });

    // Create a window iterator based on the Generic Input Stream.
    auto win_it = make_default_interval_window_stream(
        data_gen.begin(), data_gen.end(), window_width, window_stride
    );

    // Process the file.
    size_t cnt = 0;
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
        auto const& window = *it;

        if( window.entry_count() == 0 ) {
            continue;
        }
        // LOG_DBG << "position " << window.first_position();
        // LOG_DBG << "cnt " << cnt;
        // EXPECT_EQ( 2, window.samples.size() );

        // Get the two populations from the range.
        // Unfortunately, we need to versions of this, one that just gives the counts,
        // and one that filters min counts, as PoPoolation differs in their implementation.
        auto pop1 = make_transform_range(
            []( VariantWindow::Entry const& entry ) -> SampleCounts const& {
                // LOG_DBG1 << "pop1 " << entry.position << " " << entry.data[0];
                return entry.data.samples[0];
            },
            window
        );
        auto pop2 = make_transform_range(
            []( VariantWindow::Entry const& entry ) -> SampleCounts const& {
                // LOG_DBG1 << "pop2 " << entry.position << " " << entry.data.samples[1];
                return entry.data.samples[1];
            },
            window
        );
        auto pop1_filt = make_transform_range(
            [&]( VariantWindow::Entry const& entry ) {
                auto copy = entry.data.samples[0];
                transform_zero_out_by_min_count( copy, min_allele_count );
                return copy;
            },
            window
        );
        auto pop2_filt = make_transform_range(
            [&]( VariantWindow::Entry const& entry ) {
                auto copy = entry.data.samples[1];
                transform_zero_out_by_min_count( copy, min_allele_count );
                return copy;
            },
            window
        );

        // Compute the statistics
        // LOG_DBG << "f_st_pool_kofler";
        auto const fst_conv = f_st_pool_kofler(
            poolsize, poolsize,
            pop1.begin(), pop1.end(),
            pop2.begin(), pop2.end()
        );
        // LOG_DBG << "f_st_pool_karlsson";
        auto const fst_asym_unbiased = f_st_pool_karlsson(
            pop1_filt.begin(), pop1_filt.end(),
            pop2_filt.begin(), pop2_filt.end()
        );
        // LOG_DBG << "f_st_pool_unbiased";
        auto const fst_unbiased = f_st_pool_unbiased(
            poolsize, poolsize,
            pop1_filt.begin(), pop1_filt.end(),
            pop2_filt.begin(), pop2_filt.end()
        );

        // LOG_DBG << std::setprecision(9)
        //         << "kofler " << fst_conv
        //         << " karlsson " << fst_asym_unbiased
        //         << " unbiased " << fst_unbiased.first << " & " << fst_unbiased.second
        // ;

        // Compare statistics
        EXPECT_FLOAT_EQ( exp_kofler[cnt], fst_conv );
        EXPECT_FLOAT_EQ( exp_karlsson[cnt], fst_asym_unbiased );
        EXPECT_FLOAT_EQ( exp_unbiased_nei[cnt], fst_unbiased.first );
        EXPECT_FLOAT_EQ( exp_unbiased_hudson[cnt], fst_unbiased.second );
        ++cnt;
    }
}

TEST( FST, FstPoolProcessor )
{
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/p1_p2.sync.gz";

    // Make an FST processor for the two samples.
    std::vector<size_t> const poolsizes{ 100, 100 };
    auto processor = make_fst_pool_processor<FstPoolCalculatorUnbiased>(
        poolsizes, WindowAveragePolicy::kSum
    );
    ASSERT_EQ( 1, processor.size() );
    processor.thread_pool( Options::get().global_thread_pool() );
    processor.threading_threshold( 0 );

    // Make a Generic Input Stream over the data stream, and go through
    auto data_gen = make_variant_input_stream_from_sync_file( infile );
    for( auto const& variant : data_gen ) {
        processor.process( variant );
    }

    // Get the non window averaged result.
    auto const result = processor.get_result();
    EXPECT_EQ( 1, result.size() );

    // Th FST value changed here since the introduction of the proper window normalization,
    // since pi total is computed at the end of each window now, instead of being summed
    // during the window. This is a "ratio of averages" vs "average of ratios" difference,
    // which now changes the values of our result...
    // EXPECT_FLOAT_EQ( -0.0041116024, result[0] );
    EXPECT_FLOAT_EQ( -0.00035794353, result[0] );

    // Also test the involved pi values for consistency.
    // The commented value below is the old pi total value before the redesign.
    auto const vecs = processor.get_pi_vectors();
    EXPECT_EQ( 1, std::get<0>( vecs ).size() );
    EXPECT_EQ( 1, std::get<1>( vecs ).size() );
    EXPECT_EQ( 1, std::get<2>( vecs ).size() );
    EXPECT_FLOAT_EQ( 133.798915747651, std::get<0>( vecs )[0] );
    EXPECT_FLOAT_EQ( 133.703165107056, std::get<1>( vecs )[0] );
    EXPECT_FLOAT_EQ( 133.751040427354, std::get<2>( vecs )[0] );
    // EXPECT_FLOAT_EQ( 133.251040427354, std::get<2>( vecs )[0] );

}

// =================================================================================================
//     Random Fuzzy
// =================================================================================================

 // Declartion here. Defined in random_variants.cpp
std::vector<Variant> test_create_random_variants_();

void test_fst_fuzzy_run_( std::vector<Variant> const& data )
{
    // Make an FST processor
    ASSERT_GT( data.size(), 0 );
    auto const n_samples = data[0].samples.size();
    auto const pool_sizes = std::vector<size_t>( n_samples, 100 );
    auto const window_average_policy = static_cast<WindowAveragePolicy>(
        permuted_congruential_generator( 0, 4 )
    );
    auto processor = make_fst_pool_processor<FstPoolCalculatorUnbiased>(
        pool_sizes, window_average_policy
    );

    // Run the data
    for( auto const& variant : data ) {
        processor.process( variant );
    }

    // Test the result
    auto const pairs = n_samples * ( n_samples - 1 ) / 2;
    // size_t const window_length = data.size();
    auto const result = processor.get_result();
    EXPECT_EQ( pairs, result.size() );
    for( auto const& r : result ) {
        LOG_DBG << r;
    }
}

TEST( FST, RandomFuzzy )
{
    // Random seed. Report it, so that in an error case, we can reproduce.
    auto const seed = ::time(nullptr);
    permuted_congruential_generator_init( seed );
    LOG_INFO << "Seed: " << seed;

    // For the duration of the test, we deactivate debug logging.
    // But if needed, comment this line out, and each test will report its input.
    LOG_SCOPE_LEVEL( genesis::utils::Logging::kInfo );

    size_t num_tests = 5000;
    for( size_t i = 0; i < num_tests; ++i ) {
        LOG_DBG << "=================================";
        LOG_DBG << "Test " << i;
        auto const data = test_create_random_variants_();
        test_fst_fuzzy_run_( data );
    }
}

// =================================================================================================
//     C++14 All Pairs
// =================================================================================================

// Test the C++14 helper functions that compute FST for all pairs of input.
#if __cplusplus >= 201402L

TEST( FST, FstPoolAllPairs )
{
    // See above for details. Here we simply test that the C++14 extension
    // to compute all pairs of FST between samples compiles at all.

    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/p1_p2.sync.gz";

    // Settings
    size_t const window_width = 100;
    size_t const min_allele_count = 6;
    std::vector<size_t> const poolsizes{ 100, 100 };

    // Make a Generic Input Stream over the data stream.
    auto data_gen = make_variant_input_stream_from_sync_file( infile );
    auto sync_begin = data_gen.begin();
    auto sync_end   = data_gen.end();

    // Create a window iterator based on the Generic Input Stream.
    auto win_it = make_default_interval_window_stream(
        sync_begin, sync_end, window_width
    );

    // Use the code simitor to what is documented in compute_pairwise_f_st()
    for( auto const& window : win_it ) {

        // Return the SampleCounts part of the Variants in the window.
        auto sample_counts_range = make_transform_range([&]( Variant const& var )  {
            auto copy = var;
            transform_zero_out_by_min_count( copy, min_allele_count );
            return copy.samples;
        }, window);

        // Call an fst computation on that.
        f_st_pool_kofler( poolsizes, sample_counts_range.begin(), sample_counts_range.end() );
        f_st_pool_karlsson( sample_counts_range.begin(), sample_counts_range.end() );
        f_st_pool_unbiased_nei( poolsizes, sample_counts_range.begin(), sample_counts_range.end() );
        f_st_pool_unbiased_hudson( poolsizes, sample_counts_range.begin(), sample_counts_range.end() );
    }
}

#endif // __cplusplus >= 201402L
