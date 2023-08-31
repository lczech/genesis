#ifndef GENESIS_POPULATION_FUNCTIONS_DIVERSITY_POOL_CALCULATOR_H_
#define GENESIS_POPULATION_FUNCTIONS_DIVERSITY_POOL_CALCULATOR_H_

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

#include "genesis/population/functions/diversity_pool_functions.hpp"
#include "genesis/population/functions/functions.hpp"
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
 * In particular, we want to set the `min_count`, as well as the `min_coverage` and `max_coverage`.
 *
 * We do expect here that the input samples that are provided to the process() function
 * are already filtered and transformed as needed.
 * For example, typically, we want to use a BaseCountsFilter with these settings:
 *
 *     filter.min_count = settings.min_count;
 *     filter.min_coverage = settings.min_coverage;
 *     filter.max_coverage = settings.max_coverage;
 *     filter.only_snps = true;
 *
 * That is, the settings for the pool statistics should match the settings used for filtering the
 * samples. The function filter_base_counts() can be used to transform and filter the input
 * coming from a file, in order to remove base counts and samples that do not match these filters.
 *
 * There are multiple ways that this filtering can be applied. Typically for example, we want
 * to process a VariantInputIterator, which allows us to use input from a variety of input
 * file formats, all converted into Variant%s at each position in the genome. This internally
 * is a genesis::utils::LambdaIterator, which offers to add
 * @link genesis::utils::LambdaIterator::add_transform_filter() add_transform_filter()@endlink
 * functions for this purpose. The make_filter_base_counts() is a convenience function that creates
 * such a filter/transform function given a BaseCountsFilter settings instance.
 *
 * Alternaively, genesis::utils::make_filter_range() can be used to achieve the same effect,
 * but requiring a bit more manual "wiring" of the components first. This however has the advantage
 * that BaseCountsFilterStats can be provided, e.g., per window of the analysis, to capture the
 * number of sites that pass coverage filters etc. These numbers can then be used for
 * get_theta_pi_relative() and get_theta_watterson_relative(), respectively. Otherwise (when instead
 * filtering directly in the VariantInputIterator), these numbers are lost, and instead the relative
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
     * The value of `processed_count` is the number of times that process() as been called.
     */
    struct Result
    {
        double theta_pi_absolute = 0.0;
        double theta_pi_relative = 0.0;
        double theta_watterson_absolute = 0.0;
        double theta_watterson_relative = 0.0;
        double tajima_d = 0.0;
        size_t processed_count = 0;
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    DiversityPoolCalculator( DiversityPoolSettings const& settings, size_t poolsize )
        : settings_( settings )
        , poolsize_( poolsize )
    {
        if( settings.min_count == 0 ) {
            throw std::invalid_argument(
                "In DiversityPoolCalculator, settings.min_count == 0 is not allowed."
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
        empirical_min_coverage_ = std::numeric_limits<size_t>::max();
        processed_count_ = 0;
    }

    /**
     * @brief Process a @p sample, by computing its Theta Pi and Theta Watterson values,
     * respectively.
     *
     * The values are internally accumulated, so that they are usable for the getter functions.
     * This function here also returns both of them (Pi first, Watterson second) for the given
     * sample, as a convenience.
     */
    std::pair<double, double> process( BaseCounts const& sample )
    {
        double tp = 0.0;
        double tw = 0.0;
        if( enable_theta_pi_ || enable_tajima_d_ ) {
            tp = theta_pi_pool( settings_, poolsize_, sample );
            if( std::isfinite( tp )) {
                theta_pi_sum_ += tp;
            }
        }
        if( enable_theta_watterson_ || enable_tajima_d_ ) {
            tw = theta_watterson_pool( settings_, poolsize_, sample );
            if( std::isfinite( tw )) {
                theta_watterson_sum_ += tw;
            }
        }
        if(
            enable_tajima_d_ &&
            settings_.tajima_denominator_policy == TajimaDenominatorPolicy::kEmpiricalMinCoverage
        ) {
            // Only needed when we use the empirical coverage for the tajima d correction.
            // We want to find the minimum coverage of the data that we are processing.
            auto const cov = nucleotide_sum( sample );
            if( cov > 0 && cov < empirical_min_coverage_ ) {
                empirical_min_coverage_ = cov;
            }
        }
        ++processed_count_;
        return std::make_pair( tp, tw );
    }

    /**
     * @brief Get the absolute value of Theta Pi.
     *
     * This is the sum of all values for all BaseCounts samples that have been given to process().
     */
    double get_theta_pi_absolute() const
    {
        return theta_pi_sum_.get();
    }

    /**
     * @brief Compute the relative Theta Pi.
     *
     * According to PoPoolation, this is computed using only the number of SNPs with sufficient
     * coverage in the given window. This can for example be computed from BaseCountsFilterStats,
     * by using `coverage_count = BaseCountsFilterStats::passed + BaseCountsFilterStats::not_snp`.
     *
     * Alternaively, using the whole window size might also we a way to compute a relative value.
     * However, this might underestimate diversity in regions with low coverage, as then, we might
     * have positions with no coverage, so that we do not compute a value there, but they are still
     * used in the denominator here for computing the relative value.
     */
    double get_theta_pi_relative( size_t coverage_count ) const
    {
        return theta_pi_sum_.get() / static_cast<double>( coverage_count );
    }

    /**
     * @brief Get the absolute value of Theta Watterson.
     *
     * This is the sum of all values for all BaseCounts samples that have been given to process().
     */
    double get_theta_watterson_absolute() const
    {
        return theta_watterson_sum_.get();
    }

    /**
     * @brief Compute the relative Theta Watterson.
     *
     * @copydetails get_theta_pi_relative()
     */
    double get_theta_watterson_relative( size_t coverage_count ) const
    {
        return theta_watterson_sum_.get() / static_cast<double>( coverage_count );
    }

    /**
     * @brief Compute the value for Tajima's D, using the computed values for Theta Pi and Theta
     * Watterson.
     *
     * This uses the sums of all values for all BaseCounts samples that have been given to process().
     * By default, we use @p snp_count equal to the processed_count of positions that have been
     * given to process(); providing a different number here can be useful in situations were
     * some SNP positions were filtered externally for some reason, and can then for example
     * be obtained from BaseCountsFilterStats::passed. Typically though, we would expect
     * both numbers to be equal, that is, the get_processed_count() number here, and the
     * BaseCountsFilterStats::passed number obtained from filtering for SNPs.
     */
    double compute_tajima_d( size_t snp_count = 0 ) const
    {
        if( snp_count == 0 ) {
            snp_count = processed_count_;
        }
        return tajima_d_pool(
            settings_,
            theta_pi_sum_.get(), theta_watterson_sum_.get(),
            poolsize_, snp_count, empirical_min_coverage_
        );
    }

    /**
     * @brief Convenience function to obtain all other results at once.
     *
     * The function fills the Result with both diversity statistics, depending on which of them
     * have been computed, according to enable_theta_pi(), enable_theta_watterson().
     * It further computes the relative variants of those statistics if `coverage_count > 0` is
     * provided, and computes Tajima's D if enable_tajima_d() is set.
     */
    Result get_result( size_t coverage_count = 0, size_t snp_count = 0 ) const
    {
        Result res;
        if( enable_theta_pi_ ) {
            res.theta_pi_absolute = get_theta_pi_absolute();
            if( coverage_count > 0 ) {
                res.theta_pi_relative = get_theta_pi_relative( coverage_count );
            }
        }
        if( enable_theta_watterson_ ) {
            res.theta_watterson_absolute = get_theta_watterson_absolute();
            if( coverage_count > 0 ) {
                res.theta_watterson_relative = get_theta_watterson_relative( coverage_count );
            }
        }
        if( enable_tajima_d_ ) {
            res.tajima_d = compute_tajima_d( snp_count );
        }

        res.processed_count = processed_count_;
        return res;
    }

    size_t get_processed_count() const
    {
        return processed_count_;
    }

    // -------------------------------------------------------------------------
    //     Private Members
    // -------------------------------------------------------------------------

private:

    // Settings
    DiversityPoolSettings settings_;
    size_t poolsize_ = 0;

    bool enable_theta_pi_        = true;
    bool enable_theta_watterson_ = true;
    bool enable_tajima_d_        = true;

    // Data Accumulation
    utils::NeumaierSum theta_pi_sum_;
    utils::NeumaierSum theta_watterson_sum_;
    size_t processed_count_ = 0;

    // Find the minimum empirical coverage that we see in the processed data.
    size_t empirical_min_coverage_ = std::numeric_limits<size_t>::max();
};

} // namespace population
} // namespace genesis

#endif // include guard
