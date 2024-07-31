#ifndef GENESIS_POPULATION_FUNCTION_FST_POOL_UNBIASED_H_
#define GENESIS_POPULATION_FUNCTION_FST_POOL_UNBIASED_H_

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
 * @ingroup population
 */

#include "genesis/population/filter/filter_stats.hpp"
#include "genesis/population/filter/filter_status.hpp"
#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/filter/variant_filter.hpp"
#include "genesis/population/function/fst_pool_calculator.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/population/function/window_average.hpp"
#include "genesis/population/genome_locus_set.hpp"
#include "genesis/population/window/base_window.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/compensated_sum.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <cmath>
#include <limits>
#include <memory>
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
 * @brief Compute our unbiased F_ST statistic for pool-sequenced data for two ranges of SampleCounts%s.
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
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    enum class Estimator
    {
        kNei,
        kHudson
    };

    struct PiValues
    {
        double pi_within;
        double pi_between;
        double pi_total;
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    FstPoolCalculatorUnbiased(
        size_t smp1_poolsize,
        size_t smp2_poolsize,
        WindowAveragePolicy window_average_policy,
        Estimator est = Estimator::kNei
    )
        : smp1_poolsize_( smp1_poolsize )
        , smp2_poolsize_( smp2_poolsize )
        , avg_policy_( window_average_policy )
        , estimator_( est )
    {}

    virtual ~FstPoolCalculatorUnbiased() override = default;

    FstPoolCalculatorUnbiased( FstPoolCalculatorUnbiased const& ) = default;
    FstPoolCalculatorUnbiased( FstPoolCalculatorUnbiased&& )      = default;

    FstPoolCalculatorUnbiased& operator= ( FstPoolCalculatorUnbiased const& ) = default;
    FstPoolCalculatorUnbiased& operator= ( FstPoolCalculatorUnbiased&& )      = default;

    // -------------------------------------------------------------------------
    //     Abstract Members
    // -------------------------------------------------------------------------

private:

    void reset_() override
    {
        // Reset the internal counters, but not the pool sizes,
        // so that the instance can be reused across windows.
        sample_filter_stats_smp1_.clear();
        sample_filter_stats_smp2_.clear();
        sample_filter_stats_b_.clear();
        pi_w_smp1_sum_ = 0.0;
        pi_w_smp2_sum_ = 0.0;
        pi_b_sum_    = 0.0;
    }

    void process_( SampleCounts const& smp1, SampleCounts const& smp2 ) override
    {
        // Helper function for the two components of pi within
        auto pi_within_partial_ = [](
            double poolsize, double freq_a, double freq_c, double freq_g, double freq_t, double nt_cnt
        ) {
            assert( poolsize > 1.0 );

            double result = 1.0;
            result -= utils::squared( freq_a );
            result -= utils::squared( freq_c );
            result -= utils::squared( freq_g );
            result -= utils::squared( freq_t );
            result *= nt_cnt / ( nt_cnt - 1.0 );
            result *= poolsize / ( poolsize - 1.0 );
            return result;
        };

        // Prepare frequencies in sample 1. We only compute them when used.
        double smp1_nt_cnt = 0.0;
        double smp1_freq_a = 0.0;
        double smp1_freq_c = 0.0;
        double smp1_freq_g = 0.0;
        double smp1_freq_t = 0.0;

        // Prepare frequencies in sample 2. We only compute them when used.
        double smp2_nt_cnt = 0.0;
        double smp2_freq_a = 0.0;
        double smp2_freq_c = 0.0;
        double smp2_freq_g = 0.0;
        double smp2_freq_t = 0.0;

        // Compute pi within for smp1
        ++sample_filter_stats_smp1_[ smp1.status.get() ];
        if( smp1.status.passing() ) {
            smp1_nt_cnt = static_cast<double>( nucleotide_sum( smp1 ));
            smp1_freq_a = static_cast<double>( smp1.a_count ) / smp1_nt_cnt;
            smp1_freq_c = static_cast<double>( smp1.c_count ) / smp1_nt_cnt;
            smp1_freq_g = static_cast<double>( smp1.g_count ) / smp1_nt_cnt;
            smp1_freq_t = static_cast<double>( smp1.t_count ) / smp1_nt_cnt;

            auto const pi_within_smp1 = pi_within_partial_(
                smp1_poolsize_, smp1_freq_a, smp1_freq_c, smp1_freq_g, smp1_freq_t, smp1_nt_cnt
            );
            if( std::isfinite( pi_within_smp1 )) {
                pi_w_smp1_sum_ += pi_within_smp1;
                assert( smp1.a_count + smp1.c_count + smp1.g_count + smp1.t_count > 0 );
            } else {
                assert( smp1.a_count + smp1.c_count + smp1.g_count + smp1.t_count <= 1 );
            }
        }

        // Compute pi within for smp2
        ++sample_filter_stats_smp2_[ smp2.status.get() ];
        if( smp2.status.passing() ) {
            smp2_nt_cnt = static_cast<double>( nucleotide_sum( smp2 ));
            smp2_freq_a = static_cast<double>( smp2.a_count ) / smp2_nt_cnt;
            smp2_freq_c = static_cast<double>( smp2.c_count ) / smp2_nt_cnt;
            smp2_freq_g = static_cast<double>( smp2.g_count ) / smp2_nt_cnt;
            smp2_freq_t = static_cast<double>( smp2.t_count ) / smp2_nt_cnt;

            auto const pi_within_smp2 = pi_within_partial_(
                smp2_poolsize_, smp2_freq_a, smp2_freq_c, smp2_freq_g, smp2_freq_t, smp2_nt_cnt
            );
            if( std::isfinite( pi_within_smp2 )) {
                pi_w_smp2_sum_ += pi_within_smp2;
                assert( smp2.a_count + smp2.c_count + smp2.g_count + smp2.t_count > 0 );
            } else {
                assert( smp2.a_count + smp2.c_count + smp2.g_count + smp2.t_count <= 1 );
            }
        }

        // Compute pi between
        if( smp1.status.passing() && smp2.status.passing() ) {
            double pi_between = 1.0;
            pi_between -= smp1_freq_a * smp2_freq_a;
            pi_between -= smp1_freq_c * smp2_freq_c;
            pi_between -= smp1_freq_g * smp2_freq_g;
            pi_between -= smp1_freq_t * smp2_freq_t;
            if( std::isfinite( pi_between )) {
                pi_b_sum_ += pi_between;
            }

            ++sample_filter_stats_b_[SampleCountsFilterTag::kPassed];
        } else if( ! smp1.status.passing() && ! smp2.status.passing() ) {
            ++sample_filter_stats_b_[ std::min( smp1.status.get(), smp2.status.get() )];
        } else {
            ++sample_filter_stats_b_[ std::max( smp1.status.get(), smp2.status.get() )];
        }
    }

    double get_result_() const override
    {
        // We have a bit of an unfortunate situation here from a software design point of view.
        // The other fst calculator classes (Kofler and Karlsson, at the moment) do not use the
        // window normalization, and it would be weird to use dummies there. So instead, in the
        // FstPoolProcessor, we cast accordingly (which is ugly as well), to avoid this.

        // Here, we offer an additional function though for the non-window-averaging case,
        // that just returns the sum. It uses the special overloads below.
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
    //     Additional Members
    // -------------------------------------------------------------------------

public:

    // -------------------------------------------------------------------------
    //     With Window Averaging
    // -------------------------------------------------------------------------

    template<class D>
    double get_result(
        BaseWindow<D> const& window,
        std::shared_ptr<GenomeLocusSet> provided_loci,
        VariantFilterStats const& variant_filter_stats
    ) const {
        switch( estimator_ ) {
            case Estimator::kNei: {
                return get_result_pair( window, provided_loci, variant_filter_stats ).first;
            }
            case Estimator::kHudson: {
                return get_result_pair( window, provided_loci, variant_filter_stats ).second;
            }
            default: {
                throw std::invalid_argument( "Invalid FstPoolCalculatorUnbiased::Estimator" );
            }
        }
        return 0.0;
    }

    /**
     * @brief Get both variants of FST, following Nei, and following Hudson, as a pair.
     */
    template<class D>
    std::pair<double, double> get_result_pair(
        BaseWindow<D> const& window,
        std::shared_ptr<GenomeLocusSet> provided_loci,
        VariantFilterStats const& variant_filter_stats
    ) const {
        // Get the components that we need, each normalized using their own filter stats.
        auto const pi_within  = get_pi_within( window, provided_loci, variant_filter_stats );
        auto const pi_between = get_pi_between( window, provided_loci, variant_filter_stats );
        auto const pi_total   = get_pi_total( pi_within, pi_between );

        // Final computation of our two FST estimators, using Nei and Hudson, respectively.
        double const fst_nei = 1.0 - ( pi_within / pi_total );
        double const fst_hud = 1.0 - ( pi_within / pi_between );
        return { fst_nei, fst_hud };
    }

    template<class D>
    double get_pi_within(
        BaseWindow<D> const& window,
        std::shared_ptr<GenomeLocusSet> provided_loci,
        VariantFilterStats const& variant_filter_stats
    ) const {
        auto const pi_w_smp1 = pi_w_smp1_sum_.get() / window_average_denominator(
            avg_policy_, window, provided_loci, variant_filter_stats, sample_filter_stats_smp1_
        );
        auto const pi_w_smp2 = pi_w_smp2_sum_.get() / window_average_denominator(
            avg_policy_, window, provided_loci, variant_filter_stats, sample_filter_stats_smp2_
        );
        return 0.5 * ( pi_w_smp1 + pi_w_smp2 );
    }

    template<class D>
    double get_pi_between(
        BaseWindow<D> const& window,
        std::shared_ptr<GenomeLocusSet> provided_loci,
        VariantFilterStats const& variant_filter_stats
    ) const {
        return pi_b_sum_.get() / window_average_denominator(
            avg_policy_, window, provided_loci, variant_filter_stats, sample_filter_stats_b_
        );
    }

    double get_pi_total( double pi_within, double pi_between ) const
    {
        return 0.5 * ( pi_within + pi_between );
    }

    template<class D>
    double get_pi_total(
        BaseWindow<D> const& window,
        std::shared_ptr<GenomeLocusSet> provided_loci,
        VariantFilterStats const& variant_filter_stats
    ) const {
        auto const pi_within  = get_pi_within( window, provided_loci, variant_filter_stats );
        auto const pi_between = get_pi_between( window, provided_loci, variant_filter_stats );
        return get_pi_total( pi_within, pi_between );
    }

    template<class D>
    PiValues get_pi_values(
        BaseWindow<D> const& window,
        std::shared_ptr<GenomeLocusSet> provided_loci,
        VariantFilterStats const& variant_filter_stats
    ) const {
        PiValues result;
        result.pi_within  = get_pi_within( window, provided_loci, variant_filter_stats );
        result.pi_between = get_pi_between( window, provided_loci, variant_filter_stats );
        result.pi_total   = get_pi_total( result.pi_within, result.pi_between );
        return result;
    }

    WindowAveragePolicy get_window_average_policy() const
    {
        return avg_policy_;
    }

    // -------------------------------------------------------------------------
    //     Without Window Averaging
    // -------------------------------------------------------------------------

    /**
     * @brief Get both variants of FST, following Nei, and following Hudson, as a pair.
     */
    std::pair<double, double> get_result_pair() const {
        // Get the components that we need, each normalized using their own filter stats.
        auto const pi_within  = get_pi_within();
        auto const pi_between = get_pi_between();
        auto const pi_total   = get_pi_total( pi_within, pi_between );

        // Final computation of our two FST estimators, using Nei and Hudson, respectively.
        double const fst_nei = 1.0 - ( pi_within / pi_total );
        double const fst_hud = 1.0 - ( pi_within / pi_between );
        return { fst_nei, fst_hud };
    }

    double get_pi_within() const
    {
        auto const pi_w_smp1 = pi_w_smp1_sum_.get();
        auto const pi_w_smp2 = pi_w_smp2_sum_.get();
        return 0.5 * ( pi_w_smp1 + pi_w_smp2 );
    }

    double get_pi_between() const
    {
        return pi_b_sum_.get();
    }

    double get_pi_total() const
    {
        auto const pi_within  = get_pi_within();
        auto const pi_between = get_pi_between();
        return get_pi_total( pi_within, pi_between );
    }

    PiValues get_pi_values() const
    {
        PiValues result;
        result.pi_within  = get_pi_within();
        result.pi_between = get_pi_between();
        result.pi_total   = get_pi_total( result.pi_within, result.pi_between );
        return result;
    }

    // -------------------------------------------------------------------------
    //     Private Member Variables
    // -------------------------------------------------------------------------

private:

    // Pool sizes
    size_t smp1_poolsize_ = 0;
    size_t smp2_poolsize_ = 0;

    // Parameters
    WindowAveragePolicy avg_policy_;
    Estimator estimator_ = Estimator::kNei;

    // Filter stats of everything that is processed here.
    SampleCountsFilterStats sample_filter_stats_smp1_;
    SampleCountsFilterStats sample_filter_stats_smp2_;
    SampleCountsFilterStats sample_filter_stats_b_;

    // Sums over the window of pi within for both pools, and pi between them.
    utils::NeumaierSum pi_w_smp1_sum_ = 0.0;
    utils::NeumaierSum pi_w_smp2_sum_ = 0.0;
    utils::NeumaierSum pi_b_sum_    = 0.0;

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
