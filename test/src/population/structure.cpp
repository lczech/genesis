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

#include "genesis/population/base_counts.hpp"
#include "genesis/population/formats/simple_pileup_input_iterator.hpp"
#include "genesis/population/formats/simple_pileup_reader.hpp"
#include "genesis/population/formats/sync_reader.hpp"
#include "genesis/population/functions/diversity.hpp"
#include "genesis/population/functions/structure.hpp"
#include "genesis/population/window/sliding_window_generator.hpp"
#include "genesis/population/window/window.hpp"

using namespace genesis::population;
using namespace genesis::utils;

TEST( Structure, FstPool )
{
    // Equivalent PoPoolation call for conventional F_ST:
    // perl fst-sliding.pl --input p1_p2.sync --output p1_p2.fst_conventional --suppress-noninformative --min-count 6 --min-coverage 50 --max-coverage 200 --min-covered-fraction 1 --window-size 100 --step-size 100 --pool-size 500 > log_conventional.txt

    // Equivalent PoPoolation call for Karlsson F_ST:
    // perl fst-sliding.pl --input p1_p2.sync --karlsson-fst --output p1_p2.fst_karlsson --suppress-noninformative --min-count 6 --min-coverage 50 --max-coverage 200 --min-covered-fraction 1 --window-size 100 --step-size 100 --pool-size 500 > log_karlsson.txt

    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/p1_p2.sync.gz";

    // Settings
    PoolDiversitySettings settings;
    settings.min_allele_count = 6;
    settings.min_coverage = 50;
    settings.max_coverage = 200;
    settings.min_coverage_fraction = 1.0;
    settings.window_width = 100;
    settings.window_stride = 100;
    settings.poolsize = 500;
    settings.min_phred_score = 20;

    // Expected values.
    std::vector<double> exp_conventional = {{
        0.01533591, 0.01340363, 0.01554609, 0.01454173, 0.01317223, 0.01554917,
        0.01202964, 0.01316962, 0.01317223, 0.01316962, 0.01778599, 0.01554609,
        0.01554917, 0.00732000, 0.014416005, // <-- slighly changed due to rounding
        0.01554609, 0.01690169, 0.01183717, 0.01316962, 0.02049937, 0.01584036,
        0.014527005, // <-- slighly changed due to rounding
        0.01316962, 0.01196782, 0.01676964, 0.01210121, 0.01690169, 0.01554609,
        0.01601910, 0.019098585 // <-- not computed by PoPoolation (incomplete window)
    }};
    std::vector<double> exp_karlsson = {{
        0.02042334, 0.01646975, 0.02125242, 0.01905364, 0.01652186, 0.02120793,
        0.014834166, // <-- slighly changed due to rounding
        0.01646975, 0.01652186, 0.01646975, 0.02709343, 0.02125242, 0.02120793,
        0.004709437, // <-- slighly changed due to rounding
        0.01905562, 0.02125242, 0.02347631, 0.01483574, 0.01646975, 0.03265680,
        0.02125242, 0.01905562, 0.01646975, 0.01444498, 0.02410646, 0.01405358,
        0.02347631, 0.02125242, 0.02240403,
        0.027800744 // <-- not computed by PoPoolation (incomplete window)
    }};

    // Prepare the window.
    size_t value_count = 0;
    using WindowGen = SlidingWindowGenerator<std::vector<BaseCounts>>;
    WindowGen window_gen( WindowType::kInterval, settings.window_width, settings.window_stride );
    window_gen.anchor_type( WindowAnchorType::kIntervalMidpoint );
    window_gen.add_emission_plugin( [&]( WindowGen::Window const& window ) {
        if( window.entry_count() == 0 ) {
            return;
        }
        // LOG_DBG << "position " << window.first_position();
        // LOG_DBG << "value_count " << value_count;

        // Get the two populations from the range.
        // Unfortunately, we need to versions of this, one that just gives the counts,
        // and one that filters min counts, as PoPoolation differs in their implementation.
        auto pop1 = make_transform_range(
            []( WindowGen::Window::Entry const& entry ) -> BaseCounts const& {
                // LOG_DBG1 << "pop1 " << entry.position << " " << entry.data[0];
                return entry.data[0];
            },
            window
        );
        auto pop2 = make_transform_range(
            []( WindowGen::Window::Entry const& entry ) -> BaseCounts const& {
                // LOG_DBG1 << "pop2 " << entry.position << " " << entry.data[1];
                return entry.data[1];
            },
            window
        );
        auto pop1_filt = make_transform_range(
            [&]( WindowGen::Window::Entry const& entry ) {
                auto copy = entry.data[0];
                transform_by_min_count( copy, settings.min_allele_count );
                return copy;
            },
            window
        );
        auto pop2_filt = make_transform_range(
            [&]( WindowGen::Window::Entry const& entry ) {
                auto copy = entry.data[1];
                transform_by_min_count( copy, settings.min_allele_count );
                return copy;
            },
            window
        );

        // Compute the statistics
        // LOG_DBG << "f_st_conventional_pool";
        auto const fst_conv = f_st_conventional_pool(
            settings.poolsize, settings.poolsize,
            pop1.begin(), pop1.end(),
            pop2.begin(), pop2.end()
        );
        // LOG_DBG << "f_st_asymptotically_unbiased";
        auto const fst_asym_unbiased = f_st_asymptotically_unbiased(
            pop1_filt.begin(), pop1_filt.end(),
            pop2_filt.begin(), pop2_filt.end()
        );

        // Compare statistics
        EXPECT_FLOAT_EQ( exp_conventional[value_count], fst_conv );
        EXPECT_FLOAT_EQ( exp_karlsson[value_count], fst_asym_unbiased );
        ++value_count;
    });

    // Process the file.
    auto insource = from_file( infile );
    InputStream instream( insource );
    auto reader = SyncReader();
    Variant sample_set;
    while( reader.parse_line( instream, sample_set )) {
        EXPECT_EQ( 2, sample_set.samples.size() );

        // transform_by_min_count( sample_set.samples[0], settings.min_allele_count );
        // transform_by_min_count( sample_set.samples[1], settings.min_allele_count );

        if( status(
                merge( sample_set.samples ),
                settings.min_coverage, settings.max_coverage, settings.min_allele_count
            ).is_biallelic
        ) {
            // LOG_DBG << "enq " <<  sample_set.position << " " << merge( sample_set.samples );
            window_gen.enqueue( sample_set.chromosome, sample_set.position, sample_set.samples );
        }
    }
}

FstAN make_fstan( size_t a1, size_t n1, size_t a2, size_t n2 )
{
    FstAN res;
    res.a_1 = a1;
    res.n_1 = n1;
    res.a_2 = a2;
    res.n_2 = n2;
    return res;
}

BaseCounts make_fstan_base_counts( size_t a, size_t c, size_t g, size_t t, size_t n, size_t d )
{
    // Helper function, because BaseCounts does not have a value initializer contructor.
    BaseCounts result;
    result.a_count = a;
    result.c_count = c;
    result.g_count = g;
    result.t_count = t;
    result.n_count = n;
    result.d_count = d;
    return result;
}

TEST( Structure, FstAN )
{
    // Some good test cases
    EXPECT_EQ(
        make_fstan( 10, 15, 20, 25 ),
        f_st_asymptotically_unbiased_a1n1a2n2(
            make_fstan_base_counts( 10, 5, 0, 0, 0, 0 ),
            make_fstan_base_counts( 20, 5, 0, 0, 0, 0 )
        )
    );
    EXPECT_EQ(
        make_fstan( 10, 15, 20, 25 ),
        f_st_asymptotically_unbiased_a1n1a2n2(
            make_fstan_base_counts( 10, 0, 0, 5, 0, 0 ),
            make_fstan_base_counts( 20, 0, 0, 5, 0, 0 )
        )
    );
    EXPECT_EQ(
        make_fstan( 10, 15, 20, 25 ),
        f_st_asymptotically_unbiased_a1n1a2n2(
            make_fstan_base_counts( 0, 10, 5, 0, 0, 0 ),
            make_fstan_base_counts( 0, 20, 5, 0, 0, 0 )
        )
    );
    EXPECT_EQ(
        make_fstan( 10, 20, 10, 20 ),
        f_st_asymptotically_unbiased_a1n1a2n2(
            make_fstan_base_counts( 0, 10, 0, 10, 0, 0 ),
            make_fstan_base_counts( 0, 10, 0, 10, 0, 0 )
        )
    );
    EXPECT_EQ(
        make_fstan( 20, 30, 30, 70 ),
        f_st_asymptotically_unbiased_a1n1a2n2(
            make_fstan_base_counts( 0, 0, 10, 20, 0, 0 ),
            make_fstan_base_counts( 0, 0, 40, 30, 0, 0 )
        )
    );
    EXPECT_EQ(
        make_fstan( 10, 10, 10, 10 ),
        f_st_asymptotically_unbiased_a1n1a2n2(
            make_fstan_base_counts( 0, 0, 0, 10, 10, 10 ),
            make_fstan_base_counts( 0, 0, 0, 10, 10, 10 )
        )
    );

    // Error cases: more than two values --> not biallelic
    EXPECT_ANY_THROW(
        f_st_asymptotically_unbiased_a1n1a2n2(
            make_fstan_base_counts( 1, 10, 5, 0, 0, 0 ),
            make_fstan_base_counts( 0, 20, 5, 0, 0, 0 )
        )
    );
    EXPECT_ANY_THROW(
        f_st_asymptotically_unbiased_a1n1a2n2(
            make_fstan_base_counts( 0, 10, 5, 0, 0, 0 ),
            make_fstan_base_counts( 1, 20, 5, 0, 0, 0 )
        )
    );
    EXPECT_ANY_THROW(
        f_st_asymptotically_unbiased_a1n1a2n2(
            make_fstan_base_counts( 1, 10, 5, 1, 0, 0 ),
            make_fstan_base_counts( 1, 20, 5, 1, 0, 0 )
        )
    );
}
