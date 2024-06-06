#ifndef GENESIS_POPULATION_FUNCTION_DIVERSITY_POOL_CALCULATOR_H_
#define GENESIS_POPULATION_FUNCTION_DIVERSITY_POOL_CALCULATOR_H_

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

#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/function/diversity_pool_functions.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/utils/math/compensated_sum.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Diversity Pool Calculator
// =================================================================================================

/**
 * @brief Compute Theta Pi, Theta Watterson, and Tajia's D in their pool-sequencing corrected
 * versions according to Kofler et al.
 *
 * This is an efficient high level helper that is meant to compute these statistics on input
 * iterator ranges. See theta_pi_pool(), theta_watterson_pool(), and tajima_d_pool() for details
 * on the functions it computes.
 *
 * The provided DiversityPoolSettings take care of most options offered by PoPoolation.
 * In particular, we want to set the `min_count`, as well as the `min_read_depth` and
 * `max_read_depth`. These read depths are called "coverage" in PoPoolation, which seems wrong.
 *
 * We do expect here that the input samples that are provided to the process() function
 * are already filtered (with the appropriate filter status set for the Variant and the SampleCounts)
 * and transformed as needed. For example, typically, we want to use a SampleCountsFilter with
 * settings that match the DiversityPoolSettings:
 *
 *     filter.min_count = settings.min_count;
 *     filter.min_read_depth = settings.min_read_depth;
 *     filter.max_read_depth = settings.max_read_depth;
 *     filter.only_snps = true;
 *
 * That is, the settings for the pool statistics should match the settings used for filtering the
 * samples. The function filter_sample_counts() can be used to transform and filter the input coming
 * from a file, in order to filter out base counts and samples that do not match these filters.
 *
 * There are multiple ways that this filtering can be applied. Typically for example, we want
 * to process a VariantInputStream, which allows us to use input from a variety of input
 * file formats, all converted into Variant%s at each position in the genome. This internally
 * is a genesis::utils::GenericInputStream, which offers to add
 * @link genesis::utils::GenericInputStream::add_transform_filter() add_transform_filter()@endlink
 * functions for this purpose. The make_sample_counts_filter_numerical_tagging() is a convenience
 * function that creates such a filter/transform function given a SampleCountsFilter settings
 * instance.
 *
 * Alternaively, genesis::utils::make_filter_range() can be used to achieve the same effect,
 * but requiring a bit more manual "wiring" of the components first. This however has the advantage
 * that SampleCountsFilterStats can be provided, e.g., per window of the analysis, to capture the
 * number of sites that pass read depth filters etc. These numbers can then be used for
 * get_theta_pi_relative() and get_theta_watterson_relative(), respectively. Otherwise (when instead
 * filtering directly in the VariantInputStream), these numbers are lost, and instead the relative
 * values would need to be computed, e.g., using the full window sizes, instead of taking only
 * sufficiently covered positions into account for the normalization.
 *
 * With either way of filtering, for all SNP positions of interest, call process() to compute the
 * values for theta pi and theta watterson of this sample. The values are internally accumualted.
 *
 * Once all samples have been processed, the getter functions get_theta_pi_absolute(),
 * get_theta_pi_relative(), get_theta_watterson_absolute(), and get_theta_watterson_relative()
 * can be used to obtain Theta Pi and Theta Watterson directly. For Tajima's D, more computation is
 * needed, which is why the according function is called compute_tajima_d().
 *
 * See
 *
 * > R. Kofler, P. Orozco-terWengel, N. De Maio, R. V. Pandey, V. Nolte,
 * > A. Futschik, C. Kosiol, C. Schlötterer.<br>
 * > PoPoolation: A Toolbox for Population Genetic Analysis of
 * > Next Generation Sequencing Data from Pooled Individuals.<br>
 * > (2011) PLoS ONE, 6(1), e15925. https://doi.org/10.1371/journal.pone.0015925
 *
 * for details on the equations. The paper unfortunately does not explain their equations, but there
 * is a hidden document in their code repository that illuminates the situation a bit. See
 * https://sourceforge.net/projects/popoolation/files/correction_equations.pdf
 */
class DiversityPoolCalculator
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief Data struct to collect all diversity statistics computed here.
     *
     * This is meant as a simple way to obtain all diversity measures at once. See get_result().
     *
     * The struct stores all results of the diversity metrics, as well as the overall
     * number of positions that were used to compute those: the `filter_stats` contains the counts
     * of all SampleCounts::status filters of the samples passed to process().
     */
    struct Result
    {
        // Results of the diversity statistics
        double theta_pi        = std::numeric_limits<double>::quiet_NaN();
        double theta_watterson = std::numeric_limits<double>::quiet_NaN();
        double tajima_d        = std::numeric_limits<double>::quiet_NaN();
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    DiversityPoolCalculator( DiversityPoolSettings const& settings, size_t pool_size )
        : settings_( settings )
        , pool_size_( pool_size )
    {
        if( settings.min_count == 0 ) {
            throw std::invalid_argument(
                "In DiversityPoolCalculator, settings.min_count == 0 is not allowed."
            );
        }
        if( pool_size == 0 ) {
            throw std::invalid_argument(
                "In DiversityPoolCalculator, pool_size == 0 is not allowed."
            );
        }
    }
    ~DiversityPoolCalculator() = default;

    DiversityPoolCalculator( DiversityPoolCalculator const& ) = default;
    DiversityPoolCalculator( DiversityPoolCalculator&& )      = default;

    DiversityPoolCalculator& operator= ( DiversityPoolCalculator const& ) = default;
    DiversityPoolCalculator& operator= ( DiversityPoolCalculator&& )      = default;

    using self_type = DiversityPoolCalculator;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    self_type& only_passing_samples( bool value )
    {
        only_passing_samples_ = value;
        return *this;
    }

    bool only_passing_samples() const
    {
        return only_passing_samples_;
    }

    self_type& enable_theta_pi( bool value )
    {
        enable_theta_pi_ = value;
        return *this;
    }

    bool enable_theta_pi() const
    {
        return enable_theta_pi_;
    }

    self_type& enable_theta_watterson( bool value )
    {
        enable_theta_watterson_ = value;
        return *this;
    }

    bool enable_theta_watterson() const
    {
        return enable_theta_watterson_;
    }

    self_type& enable_tajima_d( bool value )
    {
        enable_tajima_d_ = value;
        return *this;
    }

    bool enable_tajima_d() const
    {
        return enable_tajima_d_;
    }

    // -------------------------------------------------------------------------
    //     Calculator Functions
    // -------------------------------------------------------------------------

    void reset()
    {
        theta_pi_sum_.reset();
        theta_watterson_sum_.reset();
        filter_stats_.clear();
        empirical_min_read_depth_ = std::numeric_limits<size_t>::max();
    }

    /**
     * @brief Process a @p sample, by computing its Theta Pi and Theta Watterson values,
     * respectively.
     *
     * The values are internally accumulated, so that they are usable for the getter functions.
     * This function here also returns both of them (Pi first, Watterson second) for the given
     * sample, as a convenience.
     */
    void process( SampleCounts const& sample )
    {
        // We first check if we need to process this sample at all.
        // That is either the case if we process _all_ samples (!only_passing_samples_)
        // or if we only consider the passing ones and the sample is passing.
        // We assume that the Variant::status has already been checked before calling this,
        // for instance by the DiversityPoolProcessor.
        ++filter_stats_[sample.status.get()];
        if( ! sample.status.passing() ) {
            return;
        }

        // Now run the calculations that we need.
        double tp = std::numeric_limits<double>::quiet_NaN();
        double tw = std::numeric_limits<double>::quiet_NaN();
        if( enable_theta_pi_ || enable_tajima_d_ ) {
            tp = theta_pi_pool( settings_, pool_size_, sample );
            if( std::isfinite( tp )) {
                theta_pi_sum_ += tp;
            }
            // assert( std::isfinite( tp ) );
        }
        if( enable_theta_watterson_ || enable_tajima_d_ ) {
            tw = theta_watterson_pool( settings_, pool_size_, sample );
            if( std::isfinite( tw )) {
                theta_watterson_sum_ += tw;
            }
            // assert( std::isfinite( tw ) );
        }

        // We want to keep track of the minimum read depth of the data that we are processing.
        // This is only needed when using TajimaDenominatorPolicy::kEmpiricalMinReadDepth,
        // but cheap enough to just always keep track of here.
        auto const cov = nucleotide_sum( sample );
        if( cov > 0 && cov < empirical_min_read_depth_ ) {
            empirical_min_read_depth_ = cov;
        }
    }

    /**
     * @brief Convenience function to obtain all results at once.
     *
     * The function fills the Result with both diversity statistics, depending on which of them
     * have been computed, according to enable_theta_pi(), enable_theta_watterson().
     * It computes the relative variants of those statistics using the provided window averaging,
     * and computes Tajima's D if enable_tajima_d() is set.
     */
    Result get_result( double window_avg_denom ) const
    {
        Result result;
        if( enable_theta_pi_ ) {
            result.theta_pi = theta_pi_sum_.get() / window_avg_denom;
        }
        if( enable_theta_watterson_ ) {
            result.theta_watterson = theta_watterson_sum_.get() / window_avg_denom;
        }
        if( enable_tajima_d_ ) {
            // Yet another problem in PoPoolation: For the |W| window size in the denominator
            // of Tajima's D, they use the number of SNPs in that window, which might or might not
            // be correct - we will have to figure this out. There is a chance that this is correct,
            // but it could also be that we want to use the the number of _all_ valid positions
            // (the ones that passed all filters, including any invariant positions) here again.
            // For now, we follow their approach, but might leave this to fix later.
            double const tajimas_window_avg_denom = filter_stats_[SampleCountsFilterTag::kPassed];

            // Potential fix:
            // auto tajimas_window_avg_denom = window_avg_denom;
            // if( settings_.tajima_denominator_policy == TajimaDenominatorPolicy::kWithPopoolationBugs ) {
            //     tajimas_window_avg_denom = filter_stats_[SampleCountsFilterTag::kPassed];
            // }

            result.tajima_d = tajima_d_pool(
                settings_,
                theta_pi_sum_.get(), theta_watterson_sum_.get(),
                pool_size_, tajimas_window_avg_denom, empirical_min_read_depth_
            );
        }
        return result;
    }

    /**
     * @brief Get the sum of filter statistics of all sample pairs processed here.
     *
     * With each call to process(), the filter stats are increased according to the filter status
     * of both provided samples. Hence, the counts returned here always have an even sum.
     */
    SampleCountsFilterStats get_filter_stats() const
    {
        return filter_stats_;
    }

    // -------------------------------------------------------------------------
    //     Private Members
    // -------------------------------------------------------------------------

private:

    // Settings
    DiversityPoolSettings settings_;
    size_t pool_size_ = 0;

    bool only_passing_samples_   = true;
    bool enable_theta_pi_        = true;
    bool enable_theta_watterson_ = true;
    bool enable_tajima_d_        = true;

    // Data Accumulation
    utils::NeumaierSum theta_pi_sum_;
    utils::NeumaierSum theta_watterson_sum_;
    SampleCountsFilterStats filter_stats_;

    // Find the minimum empirical read depth that we see in the processed data.
    size_t empirical_min_read_depth_ = std::numeric_limits<size_t>::max();
};

} // namespace population
} // namespace genesis

#endif // include guard
