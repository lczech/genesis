#ifndef GENESIS_POPULATION_FUNCTION_DIVERSITY_POOL_PROCESSOR_H_
#define GENESIS_POPULATION_FUNCTION_DIVERSITY_POOL_PROCESSOR_H_

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

#include "genesis/population/filter/variant_filter.hpp"
#include "genesis/population/function/diversity_pool_calculator.hpp"
#include "genesis/population/function/window_average.hpp"
#include "genesis/population/genome_locus_set.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/population/window/base_window.hpp"
#include "genesis/utils/core/options.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/threading/thread_functions.hpp"
#include "genesis/utils/threading/thread_pool.hpp"

#include <algorithm>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Diversity Pool Processor
// =================================================================================================

/**
 * @brief Helper class to iterate over Variant%s and process the samples (SampleCounts),
 * using a set of DiversityPoolCalculator instances, one for each sample.
 *
 * This mainly serves as a convenience tools that takes care of iterating the samples, and
 * also offers parallelization for larger datasets.
 */
class DiversityPoolProcessor
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Default constructor.
     *
     * We always want to make sure that the user provides a WindowAveragePolicy, so using this
     * default constructor leads to an unusable instance. We provide it so that dummy processors
     * can be constructed, but they have to be replaced by non-default-constructed instances
     * befor usage.
     */
    DiversityPoolProcessor() = default;

    /**
     * @brief Construct a processor.
     *
     * This defaults to using the global thread pool of Options::get().global_thread_pool()
     * If this is not desired, either pass a different @p thread_pool here, or, if no thread pool
     * is to be used, deactivate by explicitly setting the thread_pool() function here to `nullptr`
     * after construction.
     */
    DiversityPoolProcessor(
        WindowAveragePolicy window_average_policy,
        std::shared_ptr<utils::ThreadPool> thread_pool = nullptr,
        size_t threading_threshold = 4096
    )
        : avg_policy_( window_average_policy )
        , thread_pool_( thread_pool )
        , threading_threshold_( threading_threshold )
        , is_default_constructed_( false )
    {
        thread_pool_ = thread_pool ? thread_pool : utils::Options::get().global_thread_pool();
    }

    ~DiversityPoolProcessor() = default;

    DiversityPoolProcessor( DiversityPoolProcessor const& ) = default;
    DiversityPoolProcessor( DiversityPoolProcessor&& )      = default;

    DiversityPoolProcessor& operator= ( DiversityPoolProcessor const& ) = default;
    DiversityPoolProcessor& operator= ( DiversityPoolProcessor&& )      = default;

    // -------------------------------------------------------------------------
    //     Setup
    // -------------------------------------------------------------------------

    /**
     * @brief Get the thread pool used for processing, if enough sample pairs are being processed.
     */
    std::shared_ptr<utils::ThreadPool> thread_pool() const
    {
        return thread_pool_;
    }

    /**
     * @brief Set the thread pool used for processing, if enough sample pairs are being processed.
     *
     * See threading_threshold() for details on when we use the thread pool.
     * Shall not be changed after calling process().
     */
    DiversityPoolProcessor& thread_pool( std::shared_ptr<utils::ThreadPool> value )
    {
        thread_pool_ = value;
        return *this;
    }

    size_t threading_threshold() const
    {
        return threading_threshold_;
    }

    /**
     * @brief Set the threshold of calculators after which the processing is done in threads.
     *
     * For small numbers of processors (small number of sample pairs), starting threads for each
     * call of process() is more expensive than just doing the computation directly in the main
     * thread. Hence, we only want to use the thread pool if the overhead is justified.
     *
     * With this setting the number of sample pairs can be set after which we use the thread pool.
     */
    DiversityPoolProcessor& threading_threshold( size_t value )
    {
        threading_threshold_ = value;
        return *this;
    }

    /**
     * @brief Create and add a set of calculators for a given list of samples.
     *
     * The function takes the settings, as well as a list of pool sizes, which are forwarded to
     * a DiversityPoolCalculator each. We expect that the number of elements in @p pool_sizes is
     * the number of samples expected when calling process() later.
     */
    void add_calculators(
        DiversityPoolSettings const& settings,
        std::vector<size_t> const& pool_sizes
    ) {
        if( ! calculators_.empty() ) {
            throw std::runtime_error(
                "Cannot call DiversityPoolProcessor::add_calculators() multiple times."
            );
        }
        for( auto pool_size : pool_sizes ) {
            calculators_.push_back( DiversityPoolCalculator( settings, pool_size ));
            results_.emplace_back();
        }
    }

    // -------------------------------------------------------------------------
    //     Calculator Functions
    // -------------------------------------------------------------------------

    size_t size() const
    {
        return calculators_.size();
    }

    void reset()
    {
        assert( results_.size() == calculators_.size() );
        for( size_t i = 0; i < results_.size(); ++i ) {
            calculators_[i].reset();
            results_[i] = DiversityPoolCalculator::Result();
        }
        filter_stats_.clear();
    }

    void process( Variant const& variant )
    {
        // Check correct usage
        if( is_default_constructed_ ) {
            throw std::domain_error( "Cannot use a default constructed FstPoolProcessor" );
        }

        // Boundary error check. We do this before any other processing of the Variant,
        // as this indicates a serious error or issue with the data somewhere,
        // which we want to catch in any case.
        if( variant.samples.size() != calculators_.size() ) {
            throw std::runtime_error(
                "Invalid number of samples when computing Diversity Pool: Variant contains " +
                std::to_string( variant.samples.size() ) + " samples, but " +
                std::to_string( calculators_.size() ) + " pool sizes have been provided."
            );
        }

        // Only process Variants that are passing, but keep track of the ones that did not.
        ++filter_stats_[variant.status.get()];
        if( ! variant.status.passing() ) {
            return;
        }

        // Helper function for the processing.
        auto process_ = [&]( size_t index ) {
            assert( index < calculators_.size() );
            calculators_[index].process( variant.samples[index] );
        };

        // Switch dynamically between threading and no threading for the processing.
        assert( variant.samples.size() == calculators_.size() );
        if( thread_pool_ && calculators_.size() >= threading_threshold_ ) {
            parallel_for(
                0, variant.samples.size(), process_, thread_pool_
            );
        } else {
            for( size_t i = 0; i < variant.samples.size(); ++i ) {
                process_(i);
            }
        }
    }

    /**
     * @brief Get a list of all resulting values for all samples.
     *
     * This _always_ takes the @p window and @p provided_loci as input, even if the
     * WindowAveragePolicy does not require it. This is meant to make sure that we at least keep
     * track of the right things when doing any computations, and cannot forget about this.
     * For cases where the result is needed without averaging over windows (that is, just the sum
     * of all per site values), there is an overload of this function.
     */
    template<class D>
    std::vector<DiversityPoolCalculator::Result> const& get_result(
        BaseWindow<D> const& window,
        std::shared_ptr<GenomeLocusSet> provided_loci
    ) const {
        assert( results_.size() == calculators_.size() );
        for( size_t i = 0; i < results_.size(); ++i ) {
            auto const window_avg_denom = window_average_denominator(
                avg_policy_, window, provided_loci, filter_stats_, calculators_[i].get_filter_stats()
            );
            results_[i] = calculators_[i].get_result( window_avg_denom );
        }
        return results_;
    }

    /**
     * @brief Get a list of all resulting values for all samples.
     *
     * This overload does not consider the window averaging, and simply returns the sum of all
     * per site values.
     */
    std::vector<DiversityPoolCalculator::Result> const& get_result() const
    {
        assert( results_.size() == calculators_.size() );
        for( size_t i = 0; i < results_.size(); ++i ) {
            results_[i] = calculators_[i].get_result( 1 );
        }
        return results_;
    }

    /**
     * @brief Get the sum of filter statistics of all Variant%s processed here.
     *
     * With each call to process(), the filter stats are increased according to the filter status
     * of the provided Variant. Here, we offer to read out the total of all filter tags that
     * occurred in these Variants.
     */
    VariantFilterStats get_filter_stats() const
    {
        return filter_stats_;
    }

    std::vector<DiversityPoolCalculator> const& calculators() const
    {
        return calculators_;
    }

    // -------------------------------------------------------------------------
    //     Calculator Iterator
    // -------------------------------------------------------------------------

    std::vector<DiversityPoolCalculator>::iterator begin()
    {
        return calculators_.begin();
    }

    std::vector<DiversityPoolCalculator>::iterator end()
    {
        return calculators_.end();
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    // We force the correct usage of the window averaging policy here,
    // so that we make misinterpretation of the values less likely.
    WindowAveragePolicy avg_policy_;

    // Orocessors to use for these computations, which keep all the data they need.
    std::vector<DiversityPoolCalculator> calculators_;

    // Count how many Variants were processed in this processor,
    // and how many of them passed or failed the filters.
    VariantFilterStats filter_stats_;

    // We keep a mutable cache for the results, to avoid reallocating memory each time.
    mutable std::vector<DiversityPoolCalculator::Result> results_;

    // Thread pool to run the buffering in the background, and the size (number of sample pairs)
    // at which we start using the thread pool.
    std::shared_ptr<utils::ThreadPool> thread_pool_;
    size_t threading_threshold_ = 0;

    // We want to make sure to disallow default constructed instances.
    // Bit ugly to do it this way, but works for now.
    bool is_default_constructed_ = true;
};

// =================================================================================================
//     Make Diversity Processor Helper Functions
// =================================================================================================

/**
 * @brief Create an DiversityPoolProcessor to compute diversity for all samples.
 *
 * The function expects the settings to use for all samples, as well as the list of pool sizes
 * of all samples. It then yields a processor that can be provided with all Variant%s of interest
 * along the genome, and computes diversity for each sample.
 *
 * Compared to the corresponding make_fst_pool_processor() functions, this function here does not
 * really do much, and is just provided for symmetry reasons with the fst functions...
 */
inline DiversityPoolProcessor make_diversity_pool_processor(
    WindowAveragePolicy window_average_policy,
    DiversityPoolSettings const& settings,
    std::vector<size_t> const& pool_sizes
) {
    DiversityPoolProcessor processor{ window_average_policy };
    processor.add_calculators( settings, pool_sizes );
    return processor;
}

} // namespace population
} // namespace genesis

#endif // include guard
