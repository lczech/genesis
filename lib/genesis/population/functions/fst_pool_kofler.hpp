#ifndef GENESIS_POPULATION_FUNCTIONS_FST_POOL_KOFLER_H_
#define GENESIS_POPULATION_FUNCTIONS_FST_POOL_KOFLER_H_

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
 * @ingroup population
 */

#include "genesis/population/functions/fst_pool_calculator.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/compensated_sum.hpp"

#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Fst Pool Calculator Kofler
// =================================================================================================

/**
 * @brief Compute the F_ST statistic for pool-sequenced data of Kofler et al
 * as used in PoPoolation2, for two ranges of BaseCounts%s.
 *
 * The approach is called the "classical" or "conventional" estimator in PoPoolation2 [1],
 * and follows Hartl and Clark [2].
 *
 * > [1] **PoPoolation2: identifying differentiation between populations
 * > using sequencing of pooled DNA samples (Pool-Seq).**<br />
 * > Kofler R, Pandey RV, Schlotterer C.<br />
 * > Bioinformatics, 2011, 27(24), 3435–3436. https://doi.org/10.1093/bioinformatics/btr589
 *
 * > [2] **Principles of Population Genetics.**<br />
 * > Hartl DL, Clark AG.<br />
 * > Sinauer, 2007.
 */
class FstPoolCalculatorKofler final : public BaseFstPoolCalculator
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    FstPoolCalculatorKofler( size_t p1_poolsize, size_t p2_poolsize )
        : p1_poolsize_( static_cast<double>( p1_poolsize ))
        , p2_poolsize_( static_cast<double>( p2_poolsize ))
    {}

    ~FstPoolCalculatorKofler() = default;

    FstPoolCalculatorKofler( FstPoolCalculatorKofler const& ) = default;
    FstPoolCalculatorKofler( FstPoolCalculatorKofler&& )      = default;

    FstPoolCalculatorKofler& operator= ( FstPoolCalculatorKofler const& ) = default;
    FstPoolCalculatorKofler& operator= ( FstPoolCalculatorKofler&& )      = default;

    // -------------------------------------------------------------------------
    //     Abstract Members
    // -------------------------------------------------------------------------

private:

    void reset_() override
    {
        // Reset the internal counters, but not the pool sizes, so that the object can be reused.
        p1_pi_sum_ = 0.0;
        p2_pi_sum_ = 0.0;
        pp_pi_sum_ = 0.0;
    }

    void process_( BaseCounts const& p1, BaseCounts const& p2 ) override
    {
        // Edge and error cases. We will return nan anyway when finializing,
        // so we can skip all the computation here.
        if( p1_poolsize_ <= 1.0 || p2_poolsize_ <= 1.0 ) {
            return;
        }

        // Compute frequency based pi snps. The tuple returns p1, p2, pp, in that order.
        auto const pi_snps = f_st_pool_kofler_pi_snp( p1, p2 );

        // Skip invalid entries than can happen when less than two of [ACGT] have counts > 0
        // in one of the BaseCounts samples.
        if(
            std::isfinite( std::get<0>( pi_snps )) &&
            std::isfinite( std::get<1>( pi_snps )) &&
            std::isfinite( std::get<2>( pi_snps ))
        ) {
            // If we are here, both p1 and p2 have counts. Let's assert.
            assert( p1.a_count + p1.c_count + p1.g_count + p1.t_count > 0 );
            assert( p2.a_count + p2.c_count + p2.g_count + p2.t_count > 0 );

            // Now add them to the tally.
            p1_pi_sum_ += std::get<0>( pi_snps );
            p2_pi_sum_ += std::get<1>( pi_snps );
            pp_pi_sum_ += std::get<2>( pi_snps );
        } else {
            // If we are here, at least one of the two inputs has one or fewer counts in [ACGT],
            // otherwise, the results would have been finite. Let's assert this.
            assert(
                ( p1.a_count + p1.c_count + p1.g_count + p1.t_count <= 1 ) ||
                ( p2.a_count + p2.c_count + p2.g_count + p2.t_count <= 1 )
            );
        }
    }

    double get_result_() override
    {
        // Edge and error cases.
        if( p1_poolsize_ <= 1.0 || p2_poolsize_ <= 1.0 ) {
            return std::numeric_limits<double>::quiet_NaN();
            // throw std::invalid_argument( "Cannot run f_st_pool_kofler() with poolsizes <= 1" );
        }

        // Normalize by poolsize
        assert( p1_poolsize_ > 1.0 && p2_poolsize_ > 1.0 );
        double const pp_poolsizem = std::min( p1_poolsize_, p2_poolsize_ );
        auto const p1 = static_cast<double>( p1_pi_sum_ ) * p1_poolsize_ / ( p1_poolsize_ - 1.0 );
        auto const p2 = static_cast<double>( p2_pi_sum_ ) * p2_poolsize_ / ( p2_poolsize_ - 1.0 );
        auto const pp = static_cast<double>( pp_pi_sum_ ) * pp_poolsizem / ( pp_poolsizem - 1.0 );

        // _calculateFstValues
        double const pp_avg = ( p1 + p2 ) / 2.0;
        return ( pp - pp_avg ) / pp;
    }

    // -------------------------------------------------------------------------
    //     Helper Functions
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Compute the SNP-based Theta Pi values used above.
     *
     * See there for details. The tuple returns Theta Pi for an individual position, which is simply
     * the heterozygosity() at this position, for both samples @p p1 and @p p2, as well as their
     * combined (average frequency) heterozygosity, in that order.
     */
    static std::tuple<double, double, double> f_st_pool_kofler_pi_snp(
        BaseCounts const& p1, BaseCounts const& p2
    ) {
        using namespace genesis::utils;

        // _pi / _uncorrectedPiPerSNPFromFreqs
        auto pi_snp_ = [](
            double freq_a, double freq_c, double freq_g, double freq_t, double nt_cnt
        ) {
            double result = 1.0;
            result -= squared( freq_a );
            result -= squared( freq_c );
            result -= squared( freq_g );
            result -= squared( freq_t );
            result *= nt_cnt / ( nt_cnt - 1.0 );
            return result;
        };

        // _calculateSNPFrequencies
        // We cannot/do not want to simply call our heterozygosity() function here, as we need to
        // re-use the frequencies anyway to compute their average, so we do everything at once here.

        // Get frequencies in sample 1
        double const p1_nt_cnt = static_cast<double>( nucleotide_sum( p1 )); // eucov
        double const p1_freq_a = static_cast<double>( p1.a_count ) / p1_nt_cnt;
        double const p1_freq_c = static_cast<double>( p1.c_count ) / p1_nt_cnt;
        double const p1_freq_g = static_cast<double>( p1.g_count ) / p1_nt_cnt;
        double const p1_freq_t = static_cast<double>( p1.t_count ) / p1_nt_cnt;

        // Get frequencies in sample 2
        double const p2_nt_cnt = static_cast<double>( nucleotide_sum( p2 )); // eucov
        double const p2_freq_a = static_cast<double>( p2.a_count ) / p2_nt_cnt;
        double const p2_freq_c = static_cast<double>( p2.c_count ) / p2_nt_cnt;
        double const p2_freq_g = static_cast<double>( p2.g_count ) / p2_nt_cnt;
        double const p2_freq_t = static_cast<double>( p2.t_count ) / p2_nt_cnt;

        // Compute their average
        double const min_cnt = std::min( p1_nt_cnt, p2_nt_cnt );
        double const avg_a = ( p1_freq_a + p2_freq_a ) / 2.0;
        double const avg_c = ( p1_freq_c + p2_freq_c ) / 2.0;
        double const avg_g = ( p1_freq_g + p2_freq_g ) / 2.0;
        double const avg_t = ( p1_freq_t + p2_freq_t ) / 2.0;

        // _calculatePivalues / _pi / _uncorrectedPiPerSNPFromFreqs
        auto const p1_pi = pi_snp_( p1_freq_a, p1_freq_c, p1_freq_g, p1_freq_t, p1_nt_cnt );
        auto const p2_pi = pi_snp_( p2_freq_a, p2_freq_c, p2_freq_g, p2_freq_t, p2_nt_cnt );
        auto const pp_pi = pi_snp_( avg_a, avg_c, avg_g, avg_t, min_cnt );

        return std::tuple<double, double, double>{ p1_pi, p2_pi, pp_pi };
    }

    // -------------------------------------------------------------------------
    //     Private Member Variables
    // -------------------------------------------------------------------------

private:

    // Pool sizes
    double p1_poolsize_ = 0;
    double p2_poolsize_ = 0;

    // Theta Pi values for the two populations and their combination
    utils::NeumaierSum p1_pi_sum_ = 0.0;
    utils::NeumaierSum p2_pi_sum_ = 0.0;
    utils::NeumaierSum pp_pi_sum_ = 0.0;

};

} // namespace population
} // namespace genesis

#endif // include guard
