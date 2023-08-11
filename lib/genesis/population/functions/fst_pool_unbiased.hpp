#ifndef GENESIS_POPULATION_FUNCTIONS_FST_POOL_UNBIASED_H_
#define GENESIS_POPULATION_FUNCTIONS_FST_POOL_UNBIASED_H_

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
#include "genesis/population/functions/functions.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/compensated_sum.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Fst Pool Calculator Karlsson
// =================================================================================================

/**
 * @brief Compute our unbiased F_ST statistic for pool-sequenced data for two ranges of BaseCounts%s.
 *
 * This is our novel approach for estimating F_ST, using pool-sequencing corrected estimates
 * of Pi within, Pi between, and Pi total, to compute F_ST following the definitions of
 * Nei [1] and Hudson [2], respectively. These are returned here as a pair in that order.
 * See https://github.com/lczech/pool-seq-pop-gen-stats for details.
 *
 * > [1] **Analysis of Gene Diversity in Subdivided Populations.**<br />
 * > Nei M.<br />
 * > Proceedings of the National Academy of Sciences, 1973, 70(12), 3321–3323.
 * > https://doi.org/10.1073/PNAS.70.12.3321
 *
 * > [2] **Estimation of levels of gene flow from DNA sequence data.**<br />
 * > Hudson RR, Slatkin M, Maddison WP.<br />
 * > Genetics, 1992, 132(2), 583–589. https://doi.org/10.1093/GENETICS/132.2.583
 */
class FstPoolCalculatorUnbiased final : public BaseFstPoolCalculator
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    enum class Estimator
    {
        kNei,
        kHudson
    };

    FstPoolCalculatorUnbiased( size_t p1_poolsize, size_t p2_poolsize, Estimator est = Estimator::kNei )
        : p1_poolsize_( p1_poolsize )
        , p2_poolsize_( p2_poolsize )
        , estimator_( est )
    {}

    ~FstPoolCalculatorUnbiased() = default;

    FstPoolCalculatorUnbiased( FstPoolCalculatorUnbiased const& ) = default;
    FstPoolCalculatorUnbiased( FstPoolCalculatorUnbiased&& )      = default;

    FstPoolCalculatorUnbiased& operator= ( FstPoolCalculatorUnbiased const& ) = default;
    FstPoolCalculatorUnbiased& operator= ( FstPoolCalculatorUnbiased&& )      = default;

    // -------------------------------------------------------------------------
    //     Additional Members
    // -------------------------------------------------------------------------

    /**
     * @brief Get both variants of FST, following Nei, and following Hudson, as a pair.
     */
    std::pair<double, double> get_result_pair() const
    {
        // Final computation of our two FST estimators, using Nei and Hudson, respectively.
        double const fst_nei = 1.0 - ( pi_w_sum_ / pi_t_sum_ );
        double const fst_hud = 1.0 - ( pi_w_sum_ / pi_b_sum_ );
        return { fst_nei, fst_hud };
    }

    double get_pi_within() const
    {
        return pi_w_sum_;
    }

    double get_pi_between() const
    {
        return pi_b_sum_;
    }

    double get_pi_total() const
    {
        return pi_t_sum_;
    }

    // -------------------------------------------------------------------------
    //     Abstract Members
    // -------------------------------------------------------------------------

private:

    void reset_() override
    {
        // Reset the internal counters, but not the pool sizes, so that the object can be reused.
        pi_w_sum_ = 0.0;
        pi_b_sum_ = 0.0;
        pi_t_sum_ = 0.0;
    }

    void process_( BaseCounts const& p1, BaseCounts const& p2 ) override
    {
        // Compute pi values for the snp.
        // The tuple `pi_snp` returns pi within, between, and total, in that order.
        auto const pi_snp = f_st_pool_unbiased_pi_snp( p1_poolsize_, p2_poolsize_, p1, p2 );

        // Skip invalid entries than can happen when less than two of [ACGT] have
        // counts > 0 in one of the BaseCounts samples.
        if(
            std::isfinite( std::get<0>( pi_snp )) &&
            std::isfinite( std::get<1>( pi_snp )) &&
            std::isfinite( std::get<2>( pi_snp ))
        ) {
            // If we are here, both p1 and p2 have counts. Let's assert.
            assert( p1.a_count + p1.c_count + p1.g_count + p1.t_count > 0 );
            assert( p2.a_count + p2.c_count + p2.g_count + p2.t_count > 0 );

            // Now add them to the tally.
            pi_w_sum_ += std::get<0>( pi_snp );
            pi_b_sum_ += std::get<1>( pi_snp );
            pi_t_sum_ += std::get<2>( pi_snp );
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
        switch( estimator_ ) {
            case Estimator::kNei: {
                return get_result_pair().first;
            }
            case Estimator::kHudson: {
                return get_result_pair().second;
            }
            default: {
                throw std::invalid_argument( "Invalid FstPoolCalculatorUnbiased::Estimator" );
            }
        }
        return 0.0;
    }

    // -------------------------------------------------------------------------
    //     Helper Functions
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Compute the SNP-based Theta Pi values used in f_st_pool_unbiased().
     *
     * The function returns pi within, between, and total, in that order.
     * See f_st_pool_unbiased() for details.
     */
    static std::tuple<double, double, double> f_st_pool_unbiased_pi_snp(
        size_t p1_poolsize, size_t p2_poolsize,
        BaseCounts const& p1_counts, BaseCounts const& p2_counts
    ) {
        using namespace genesis::utils;

        // There is some code duplication from f_st_pool_kofler_pi_snp() here, but for speed reasons,
        // we keep it that way for now. Not worth calling more functions than needed.

        // Helper function for the two components of pi within
        auto pi_within_partial_ = [](
            double poolsize, double freq_a, double freq_c, double freq_g, double freq_t, double nt_cnt
        ) {
            assert( poolsize > 1.0 );

            double result = 1.0;
            result -= squared( freq_a );
            result -= squared( freq_c );
            result -= squared( freq_g );
            result -= squared( freq_t );
            result *= nt_cnt / ( nt_cnt - 1.0 );
            result *= poolsize / ( poolsize - 1.0 );
            return result;
        };

        // Get frequencies in sample 1
        double const p1_nt_cnt = static_cast<double>( nucleotide_sum( p1_counts ));
        double const p1_freq_a = static_cast<double>( p1_counts.a_count ) / p1_nt_cnt;
        double const p1_freq_c = static_cast<double>( p1_counts.c_count ) / p1_nt_cnt;
        double const p1_freq_g = static_cast<double>( p1_counts.g_count ) / p1_nt_cnt;
        double const p1_freq_t = static_cast<double>( p1_counts.t_count ) / p1_nt_cnt;

        // Get frequencies in sample 2
        double const p2_nt_cnt = static_cast<double>( nucleotide_sum( p2_counts ));
        double const p2_freq_a = static_cast<double>( p2_counts.a_count ) / p2_nt_cnt;
        double const p2_freq_c = static_cast<double>( p2_counts.c_count ) / p2_nt_cnt;
        double const p2_freq_g = static_cast<double>( p2_counts.g_count ) / p2_nt_cnt;
        double const p2_freq_t = static_cast<double>( p2_counts.t_count ) / p2_nt_cnt;

        // Compute pi within
        auto const pi_within = 0.5 * (
            pi_within_partial_( p1_poolsize, p1_freq_a, p1_freq_c, p1_freq_g, p1_freq_t, p1_nt_cnt ) +
            pi_within_partial_( p2_poolsize, p2_freq_a, p2_freq_c, p2_freq_g, p2_freq_t, p2_nt_cnt )
        );

        // Compute pi between
        double pi_between = 1.0;
        pi_between -= p1_freq_a * p2_freq_a;
        pi_between -= p1_freq_c * p2_freq_c;
        pi_between -= p1_freq_g * p2_freq_g;
        pi_between -= p1_freq_t * p2_freq_t;

        // Compute pi total
        double const pi_total = 0.5 * ( pi_within + pi_between );

        return std::tuple<double, double, double>{ pi_within, pi_between, pi_total };
    }

    // -------------------------------------------------------------------------
    //     Private Member Variables
    // -------------------------------------------------------------------------

private:

    // Pool sizes
    size_t p1_poolsize_ = 0;
    size_t p2_poolsize_ = 0;

    Estimator estimator_ = Estimator::kNei;

    // Sums over the window of pi within, between, total.
    utils::NeumaierSum pi_w_sum_ = 0.0;
    utils::NeumaierSum pi_b_sum_ = 0.0;
    utils::NeumaierSum pi_t_sum_ = 0.0;

};

// =================================================================================================
//     Estimator Helper Functions
// =================================================================================================

inline std::string fst_pool_unbiased_estimator_to_string(
    FstPoolCalculatorUnbiased::Estimator estimator
) {
    switch( estimator ) {
        case FstPoolCalculatorUnbiased::Estimator::kNei: {
            return "Nei";
        }
        case FstPoolCalculatorUnbiased::Estimator::kHudson: {
            return "Hudson";
        }
        default: {
            throw std::invalid_argument( "Invalid FstPoolCalculatorUnbiased::Estimator" );
        }
    }
}

inline FstPoolCalculatorUnbiased::Estimator fst_pool_unbiased_estimator_from_string(
    std::string const& str
) {
    auto const low = genesis::utils::to_lower( str );
    if( low == "nei" ) {
        return FstPoolCalculatorUnbiased::Estimator::kNei;
    }
    if( low == "hudson" ) {
        return FstPoolCalculatorUnbiased::Estimator::kHudson;
    }
    throw std::invalid_argument( "Invalid FstPoolCalculatorUnbiased::Estimator: " + str );
}

} // namespace population
} // namespace genesis

#endif // include guard
