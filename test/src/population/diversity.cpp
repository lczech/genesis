/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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

#include "genesis/population/formats/simple_pileup_common.hpp"
#include "genesis/population/formats/simple_pileup_input_iterator.hpp"
#include "genesis/population/formats/simple_pileup_reader.hpp"
#include "genesis/population/formats/variant_input_iterator.hpp"
#include "genesis/population/functions/diversity.hpp"
#include "genesis/population/window/sliding_interval_window_iterator.hpp"
#include "genesis/population/window/sliding_window_generator.hpp"
#include "genesis/population/window/window.hpp"

using namespace genesis::population;
using namespace genesis::utils;

TEST( Population, StatisticsNBase )
{
    // Run loop for some lower values.
    for( size_t p = 1; p < 50; ++p ) {
        for( size_t c = 1; c < 3 * p; ++c ) {
            EXPECT_FLOAT_EQ( n_base_matrix( c, p ), n_base( c, p ));
        }
    }

    // Also test some higher values. We request the higher coverage values first in order to
    // avoid recomputing the matrix as much as possible.
    EXPECT_FLOAT_EQ( n_base_matrix(  500,  100 ), n_base(  500,  100 ));
    EXPECT_FLOAT_EQ( n_base_matrix(  100,  100 ), n_base(  100,  100 ));
    EXPECT_FLOAT_EQ( n_base_matrix(   50,  100 ), n_base(   50,  100 ));
    EXPECT_FLOAT_EQ( n_base_matrix( 2000, 1000 ), n_base( 2000, 1000 ));
    EXPECT_FLOAT_EQ( n_base_matrix(  500, 1000 ), n_base(  500, 1000 ));
    EXPECT_FLOAT_EQ( n_base_matrix(  100, 1000 ), n_base(  100, 1000 ));
}

// We run the test with two different versions, one following the PoPoolation window intervals
// and ones with proper, reasonable intervals... See below for details.

TEST( Population, DiversityMeasuresGenerator )
{
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/78.pileup.gz";

    // We here compare our results to those obtained with a slightly modified version of PoPoolation.
    // Their window is weird, and uses a half-open `(]` interval, instead of the more reasonable
    // half-open `[)`. For our tests, we fixed that. Furthermore, in order to get comparable
    // (but wrong!) results, we here activate our re-implementation of PoPoolation bugs.

    // Prepare all settings.
    PoolDiversitySettings settings;
    // settings.window_width = 1000;
    // settings.window_stride = 1000;
    // settings.min_phred_score = 20;
    size_t const window_width = 1000;
    size_t const window_stride = 1000;
    size_t const min_phred_score = 20;
    settings.poolsize = 500;
    settings.min_allele_count = 2;
    settings.min_coverage = 4;
    settings.max_coverage = 70;
    // settings.min_coverage_fraction = 0.6;
    settings.with_popoolation_bugs = true;

    // Expected values for SNP count, coverage fraction,
    // theta pi, theta watterson, and tajimas d, as computed by PoPoolation.
    std::vector<size_t> const exp_snp_cnt = {
        2, 7, 21, 4, 9, 25, 3, 20, 20, 7, 11, 19, 13, 19, 17, 9, 29, 11, 19, 12, 6, 2, 10, 9, 7,
        3, 3, 8, 8, 5, 9, 10, 17, 22, 24, 9, 15, 20, 16, 12, 5, 12, 6, 12, 5, 9, 6, 11, 8, 10, 2
    };
    std::vector<double> const exp_cov = {
        0.606, 0.957, 0.988, 0.982, 0.828, 0.995, 0.990, 0.967, 0.933, 0.983, 0.991, 1.000, 0.928,
        0.870, 0.977, 0.837, 0.933, 0.887, 0.993, 0.960, 0.906, 0.922, 0.997, 0.763, 0.929, 0.999,
        0.991, 0.989, 0.864, 0.997, 0.994, 0.818, 0.997, 0.998, 0.994, 0.895, 1.000, 0.958, 0.949,
        0.998, 1.000, 0.997, 0.963, 0.998, 0.964, 0.977, 0.997, 0.986, 0.985, 0.967, 0.142
    };
    std::vector<double> const exp_pi = {
        0.002828868, 0.003551842, 0.008620834, 0.002140255, 0.005933968, 0.016407349, 0.001243119,
        0.011708482, 0.012274668, 0.003780370, 0.005777989, 0.011731806, 0.009463886, 0.013261737,
        0.008956950, 0.005549308, 0.017259885, 0.009433137, 0.009115424, 0.004549953, 0.006191080,
        0.001405973, 0.004827704, 0.007490545, 0.005137985, 0.001001777, 0.000902059, 0.002551349,
        0.004454185, 0.002033991, 0.005578831, 0.007278836, 0.009191179, 0.010598083, 0.013469830,
        0.006877809, 0.007874965, 0.011942693, 0.009170245, 0.005951344, 0.002949180, 0.005685832,
        0.004222115, 0.005529795, 0.003219569, 0.004252115, 0.002881243, 0.006127748, 0.004616422,
        0.006539171, 0.011468875 // that last value is na in PoPoolation due to low coverage fraction
    };
    std::vector<double> const exp_tw = {
        0.002571700, 0.003350224, 0.009166134, 0.001846507, 0.005931611, 0.015788888, 0.001466433,
        0.010343512, 0.012401424, 0.003954569, 0.005865480, 0.012601217, 0.010011943, 0.013263021,
        0.009092277, 0.004937992, 0.015816639, 0.009322914, 0.010141464, 0.005384235, 0.006419515,
        0.001124550, 0.004012873, 0.007664440, 0.005601778, 0.001339554, 0.001333440, 0.003303282,
        0.005329928, 0.002425735, 0.005334840, 0.006631916, 0.008486691, 0.009949509, 0.013824534,
        0.006948406, 0.007770275, 0.011238698, 0.008664908, 0.006488325, 0.002268357, 0.005166702,
        0.003946841, 0.005920627, 0.002920130, 0.004690126, 0.002902389, 0.006156112, 0.004224226,
        0.005681248, 0.011799121 // that last value is na in PoPoolation due to low coverage fraction
    };
    std::vector<double> const exp_td = {
        0.284578106, 0.192582068, -0.194060545, 0.487758589, 0.001285058, 0.127948564, -0.449156140,
        0.430526340, -0.033356987, -0.141493584, -0.048408427, -0.225182587, -0.178297114,
        -0.0003157667, // <-- slightly different from popoolation, due to float rounding output
        -0.048518386, 0.399560639, 0.298089456, 0.038457024, -0.330034295, -0.502740128, -0.114750045,
        0.672276818, 0.655962019, -0.073510112, -0.267143845, -0.737907021, -0.945516711, -0.730646287,
        -0.530233394, -0.507613275, 0.148039558, 0.316135303, 0.270556455, 0.212712696, -0.083788817,
        -0.032937101, 0.043877175, 0.204396945, 0.190020150, -0.268953530, 0.941026912, 0.325990577,
        0.223266340, -0.214392335, 0.324062971, -0.301804585, -0.023170726, -0.014957651, 0.299195764,
        0.489457710, -0.080335423 // that last value is na in PoPoolation due to low coverage fraction
    };

    using VariantWindow = Window<genesis::population::Variant>;

    // Prepare the window.
    size_t iteration_count = 0;
    size_t value_count = 0;
    using WindowGen = SlidingWindowGenerator<Variant>;
    WindowGen window_gen( SlidingWindowType::kInterval, window_width, window_stride );
    // window_gen.anchor_type( WindowAnchorType::kIntervalMidpoint );
    window_gen.add_emission_plugin( [&]( WindowGen::Window const& window ) {

        // We skip the initial part of the windows that we are not interested in.
        if( iteration_count < 7800 ) {
            ++iteration_count;
            return;
        }

        // Select the sample within the current window.
        auto range = make_transform_range(
            []( VariantWindow::Entry const& entry ) -> BaseCounts const& {
                // Cannot use gtest within a lambda...
                // ASSERT_EQ( 1, entry.data.samples.size() );
                if( entry.data.samples.size() != 1 ) {
                    throw std::runtime_error( "Internal error: file has wrong number of samples." );
                }
                return entry.data.samples[0];
            },
            window.begin(), window.end()
        );

        // Compute all statistics and compare them to the expected PoPoolation results.
        auto const stats = pool_diversity_measures( settings, range.begin(), range.end() );

        // LOG_DBG1 << iteration_count << "\t" << value_count << "\t"
        //          << window.first_position() << "\t" << window.last_position() << "\t"
        //          << window.entries().front().position << "\t" << window.entries().back().position << "\t"
        //          << stats.snp_count << "\t" << stats.coverage_count << "\t"
        //          << stats.theta_pi_relative << "\t"
        //          << stats.theta_watterson_relative << "\t"
        //          << stats.tajima_d
        // ;

        // Compare counts
        EXPECT_EQ( exp_snp_cnt[value_count], stats.snp_count );
        // EXPECT_FLOAT_EQ( exp_cov[value_count], stats.coverage_fraction );
        EXPECT_FLOAT_EQ(
            exp_cov[value_count],
            static_cast<double>( stats.coverage_count ) / window_width
        );

        // Compare statistic measures
        EXPECT_FLOAT_EQ( exp_pi[value_count], stats.theta_pi_relative );
        EXPECT_FLOAT_EQ( exp_tw[value_count], stats.theta_watterson_relative );
        EXPECT_FLOAT_EQ( exp_td[value_count], stats.tajima_d );
        ++iteration_count;
        ++value_count;
    });

    // Prepare the reader.
    auto reader = SimplePileupReader();
    reader.quality_encoding( genesis::sequence::QualityEncoding::kIllumina13 );

    // Iterate the data!
    for( auto it = SimplePileupInputIterator<>( from_file( infile ), reader ); it; ++it ) {
        auto const& record = *it;
        ASSERT_EQ( 1, record.samples.size() );

        auto sample = convert_to_variant( record, min_phred_score );
        window_gen.enqueue( record.chromosome, record.position, sample );
    }
    window_gen.finish_chromosome();
}

TEST( Population, DiversityMeasuresIterator )
{
    NEEDS_TEST_DATA;
    std::string const infile = environment->data_dir + "population/78.pileup.gz";

    // Prepare all settings.
    PoolDiversitySettings settings;
    // settings.window_width = 1000;
    // settings.window_stride = 1000;
    // settings.min_phred_score = 20;
    size_t const window_width = 1000;
    size_t const window_stride = 1000;
    size_t const min_phred_score = 20;
    settings.poolsize = 500;
    settings.min_allele_count = 2;
    settings.min_coverage = 4;
    settings.max_coverage = 70;
    // settings.min_coverage_fraction = 0.6;
    settings.with_popoolation_bugs = true;

    // Expected values for SNP count, coverage fraction,
    // theta pi, theta watterson, and tajimas d, as computed by PoPoolation.
    //
    // We have slightly different values for some of the entries, one due to rounding,
    // and several because PoPoolation makes the mistake to start counting at 0, while their
    // pileup input format counts positions starting from 1, so that the border entries between
    // windows end up in wrong windows... We fix this, but that means that some of the values
    // are slightly different. For the previous computation and values, where we followed the
    // approach of PoPoolation, see the above deprecated Generator approach,
    // which has the same off-by-on error.
    //
    std::vector<size_t> const exp_snp_cnt = {
        2, 7, 21, 4, 9, 25, 3, 20, 20, 7, 11, 19, 13, 19, 17, 9, 29, 11, 19, 12, 6, 2, 10, 9, 7,
        3, 3, 8, 9, 4, 9, 10, 18, 21, 24, 9, 15, 20, 16, 12, 5, 12, 6, 12, 5, 9, 6, 11, 8, 10, 2
    };
    std::vector<double> const exp_cov = {
        0.606, 0.957, 0.988, 0.982, 0.828, 0.995, 0.990, 0.967, 0.933, 0.983, 0.991, 1.000, 0.927,
        0.871, 0.977, 0.837, 0.933, 0.887, 0.993, 0.960, 0.906, 0.922, 0.997, 0.763, 0.929, 0.999,
        0.991, 0.989, 0.864, 0.997, 0.994, 0.818, 0.997, 0.998, 0.993, 0.896, 1.000, 0.958, 0.949,
        0.998, 1.000, 0.997, 0.963, 0.998, 0.964, 0.977, 0.997, 0.986, 0.985, 0.967, 0.141
    };
    std::vector<double> const exp_pi = {
        0.002828868, 0.003551842, 0.008620834, 0.002140255, 0.005933968, 0.016407349, 0.001243119,
        0.011708482, 0.012274668, 0.003780370, 0.005777989, 0.011731806, 0.009474095, 0.013246511,
        0.008956950, 0.005549308, 0.017259885, 0.009433137, 0.009115424, 0.004549953, 0.006191080,
        0.001405973, 0.004827704, 0.007490545, 0.005137985, 0.001001777, 0.000902059, 0.002551349,
        0.005095216, 0.001478473, 0.005578831, 0.007278836, 0.009792987, 0.009996877, 0.013483395,
        0.006870132, 0.007874965, 0.011942693, 0.009170245, 0.005951344, 0.002949180, 0.005685832,
        0.004222115, 0.005529795, 0.003219569, 0.004252115, 0.002881243, 0.006127748, 0.004616422,
        0.006539171, 0.011550215
    };
    std::vector<double> const exp_tw = {
        0.002571700, 0.003350224, 0.009166134, 0.001846507, 0.005931611, 0.015788888, 0.001466433,
        0.010343512, 0.012401424, 0.003954569, 0.005865480, 0.012601217, 0.010022743, 0.013247794,
        0.009092277, 0.004937992, 0.015816639, 0.009322914, 0.010141464, 0.005384235, 0.006419515,
        0.001124550, 0.004012873, 0.007664440, 0.005601778, 0.001339554, 0.001333440, 0.003303282,
        0.005902944, 0.001929159, 0.005334840, 0.006631916, 0.009178426, 0.009258467, 0.013838456,
        0.006940651, 0.007770275, 0.011238698, 0.008664908, 0.006488325, 0.002268357, 0.005166702,
        0.003946841, 0.005920627, 0.002920130, 0.004690126, 0.002902389, 0.006156112, 0.004224226,
        0.005681248, 0.011882802
    };
    std::vector<double> const exp_td = {
        0.284578106, 0.192582068, -0.194060545, 0.487758589, 0.001285058, 0.127948564, -0.449156140,
        0.430526340, -0.033356987, -0.141493584, -0.048408427, -0.225182587, -0.178297114, -0.0003157667,
        -0.048518386, 0.399560639, 0.298089456, 0.038457024, -0.330034295, -0.502740128, -0.114750045,
        0.672276818, 0.655962019, -0.073510112, -0.267143845, -0.737907021, -0.945516711, -0.730646287,
        -0.44273502, -0.71878332, 0.148039558, 0.316135303, 0.21832839, 0.26017737, -0.083788817,
        -0.032937101, 0.043877175, 0.204396945, 0.190020150, -0.268953530, 0.941026912, 0.325990577,
        0.223266340, -0.214392335, 0.324062971, -0.301804585, -0.023170726, -0.014957651, 0.299195764,
        0.489457710, -0.080335423
    };

    using VariantWindow = Window<genesis::population::Variant>;

    // Prepare the reader.
    LOG_DBG << "SimplePileupReader()";
    auto reader = SimplePileupReader();
    reader.quality_encoding( genesis::sequence::QualityEncoding::kIllumina13 );
    reader.min_base_quality( min_phred_score );

    // Make a Lambda Iterator over the data stream.
    LOG_DBG << "make_variant_input_iterator_from_pileup_file()";
    auto data_gen = make_variant_input_iterator_from_pileup_file( infile, reader );
    auto pileup_begin = data_gen.begin();
    auto pileup_end   = data_gen.end();

    // Create a window iterator based on the lambda iterator.
    LOG_DBG << "make_default_sliding_interval_window_iterator()";
    auto win_it = make_default_sliding_interval_window_iterator(
        pileup_begin, pileup_end, window_width, window_stride
    );

    LOG_DBG << "for()";
    size_t value_count = 0;
    size_t window_cnt = 0;
    size_t iteration_count = 0;
    for( auto it = win_it.begin(); it != win_it.end(); ++it ) {
        auto const& window = *it;

        // We skip the initial part of the windows that we are not interested in.
        if( iteration_count < 7800 ) {
            ++iteration_count;
            continue;
        }

        // Select the sample within the current window.
        auto range = make_transform_range(
            []( VariantWindow::Entry const& entry ) -> BaseCounts const& {
                // Cannot use gtest within a lambda...
                // ASSERT_EQ( 1, entry.data.samples.size() );
                if( entry.data.samples.size() != 1 ) {
                    throw std::runtime_error( "Internal error: file is wrong." );
                }
                return entry.data.samples[0];
            },
            window.begin(), window.end()
        );

        // Compute all statistics and compare them to the expected PoPoolation results.
        auto const stats = pool_diversity_measures( settings, range.begin(), range.end() );

        // LOG_DBG1 << iteration_count << "\t" << value_count << "\t"
        //          << window.first_position() << "\t" << window.last_position() << "\t"
        //          << window.entries().front().position << "\t" << window.entries().back().position << "\t"
        //          << stats.snp_count << "\t" << stats.coverage_count << "\t"
        //          << stats.theta_pi_relative << "\t"
        //          << stats.theta_watterson_relative << "\t"
        //          << stats.tajima_d
        // ;

        // Compare counts
        EXPECT_EQ( exp_snp_cnt[value_count], stats.snp_count );
        // EXPECT_FLOAT_EQ( exp_cov[value_count], stats.coverage_fraction );
        EXPECT_FLOAT_EQ(
            exp_cov[value_count],
            static_cast<double>( stats.coverage_count ) / window_width
        );

        // Compare statistic measures
        EXPECT_FLOAT_EQ( exp_pi[value_count], stats.theta_pi_relative );
        EXPECT_FLOAT_EQ( exp_tw[value_count], stats.theta_watterson_relative );
        EXPECT_FLOAT_EQ( exp_td[value_count], stats.tajima_d );
        ++value_count;

        ++window_cnt;
        ++iteration_count;
    }
}
