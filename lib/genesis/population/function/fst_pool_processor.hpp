#ifndef GENESIS_POPULATION_FUNCTION_FST_POOL_PROCESSOR_H_
#define GENESIS_POPULATION_FUNCTION_FST_POOL_PROCESSOR_H_

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
#include "genesis/population/function/fst_pool_calculator.hpp"
#include "genesis/population/function/fst_pool_unbiased.hpp"
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
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <tuple>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Fst Pool Processor
// =================================================================================================

/**
 * @brief Helper class to iterate over Variant%s and process pairs of FST between their samples
 * (SampleCounts), using a set of BaseFstPoolCalculator.
 */
class FstPoolProcessor
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using PiVectorTuple = std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Construct a processor.
     *
     * This defaults to using the global thread pool of Options::get().global_thread_pool()
     * If this is not desired, either pass a different @p thread_pool here, or, if no thread pool
     * is to be used, deactivate by explicitly setting the thread_pool() function here to `nullptr`.
     */
    FstPoolProcessor(
        std::shared_ptr<utils::ThreadPool> thread_pool = nullptr,
        size_t threading_threshold = 4096
    )
        : thread_pool_( thread_pool )
        , threading_threshold_( threading_threshold )
    {
        thread_pool_ = thread_pool ? thread_pool : utils::Options::get().global_thread_pool();
    }

    ~FstPoolProcessor() = default;

    FstPoolProcessor( FstPoolProcessor const& ) = default;
    FstPoolProcessor( FstPoolProcessor&& )      = default;

    FstPoolProcessor& operator= ( FstPoolProcessor const& ) = default;
    FstPoolProcessor& operator= ( FstPoolProcessor&& )      = default;

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
    FstPoolProcessor& thread_pool( std::shared_ptr<utils::ThreadPool> value )
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
    FstPoolProcessor& threading_threshold( size_t value )
    {
        threading_threshold_ = value;
        return *this;
    }

    /**
     * @brief Add a calculator, that is, an instance to compute FST for a pair of samples.
     */
    void add_calculator(
        size_t index_p1, size_t index_p2,
        std::unique_ptr<BaseFstPoolCalculator> calculator
    ) {
        assert( sample_pairs_.size() == calculators_.size() );
        assert( sample_pairs_.size() == results_.size() );
        sample_pairs_.push_back({ index_p1, index_p2 });
        calculators_.push_back( std::move( calculator ));
        results_.push_back( std::numeric_limits<double>::quiet_NaN() );
    }

    // -------------------------------------------------------------------------
    //     Calculator Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Get the total number of calculates, i.e., the number of pairs of samples
     * for which we comute FST here.
     */
    size_t size() const
    {
        assert( calculators_.size() == sample_pairs_.size() );
        assert( calculators_.size() == results_.size() );
        return calculators_.size();
    }

    void reset()
    {
        assert( calculators_.size() == results_.size() );
        for( auto& calc : calculators_ ) {
            assert( calc );
            calc->reset();
        }
        filter_stats_.clear();
        std::fill(
            results_.begin(), results_.end(),
            std::numeric_limits<double>::quiet_NaN()
        );

        // Also reset the pi vectors to nan.
        // If they are not allocated, nothing happens.
        auto const res_sz = results_.size();
        (void) res_sz;
        assert( std::get<0>( results_pi_ ).size() == 0 || std::get<0>( results_pi_ ).size() == res_sz );
        assert( std::get<1>( results_pi_ ).size() == 0 || std::get<1>( results_pi_ ).size() == res_sz );
        assert( std::get<2>( results_pi_ ).size() == 0 || std::get<2>( results_pi_ ).size() == res_sz );
        std::fill(
            std::get<0>( results_pi_ ).begin(), std::get<0>( results_pi_ ).end(),
            std::numeric_limits<double>::quiet_NaN()
        );
        std::fill(
            std::get<1>( results_pi_ ).begin(), std::get<1>( results_pi_ ).end(),
            std::numeric_limits<double>::quiet_NaN()
        );
        std::fill(
            std::get<2>( results_pi_ ).begin(), std::get<2>( results_pi_ ).end(),
            std::numeric_limits<double>::quiet_NaN()
        );
    }

    void process( Variant const& variant )
    {
        // Check correct usage
        assert( sample_pairs_.size() == calculators_.size() );

        // Only process Variants that are passing, but keep track of the ones that did not.
        ++filter_stats_[variant.status.get()];
        if( ! variant.status.passing() ) {
            return;
        }

        // Helper function for the processing.
        auto process_ = [&]( size_t index ) {
            auto const& indices = sample_pairs_[index];
            if(
                indices.first >= variant.samples.size() ||
                indices.second >= variant.samples.size()
            ) {
                throw std::runtime_error(
                    "Invalid sample indices for computing FST Pool: Variant contains " +
                    std::to_string( variant.samples.size() ) + " samples, but indices " +
                    std::to_string( indices.first ) + " and " + std::to_string( indices.second ) +
                    " have been requested."
                );
            }
            calculators_[index]->process(
                variant.samples[indices.first],
                variant.samples[indices.second]
            );
        };

        // Switch dynamically between threading and no threading for the processing.
        if( thread_pool_ && calculators_.size() >= threading_threshold_ ) {
            parallel_for(
                0, sample_pairs_.size(), process_, thread_pool_
            );
        } else {
            for( size_t i = 0; i < sample_pairs_.size(); ++i ) {
                process_(i);
            }
        }
    }

    /**
     * @brief Get a list of all resulting FST values for all pairs of samples.
     *
     * This _always_ takes the @p window and @p provided_loci as input, which are components needed
     * for some of the window averaging policies, even if the WindowAveragePolicy does
     * not require it. This is meant to make sure that we at least keep track of the right things
     * when doing any computations, and cannot forget about this.
     * There is an overload of this function which does not need this, and always returns the sum.
     */
    template<class D>
    std::vector<double> const& get_result(
        BaseWindow<D> const& window,
        std::shared_ptr<GenomeLocusSet> provided_loci
    ) const {
        assert( results_.size() == calculators_.size() );
        for( size_t i = 0; i < results_.size(); ++i ) {
            // We do an ugly dispatch here to treat the special case of the FstPoolCalculatorUnbiased
            // class, which needs additional information on the window in order to normalize
            // the pi values correctly. The Kofler and Karlsson do not need that, and we want to
            // avoid using dummies in these places. So instead, we just do a dispatch here.
            // If in the future more calculators are added that need special behaviour,
            // we might want to redesign this...
            auto const* raw_calc = calculators_[i].get();
            auto const* unbiased_calc = dynamic_cast<FstPoolCalculatorUnbiased const*>( raw_calc );
            if( unbiased_calc ) {
                results_[i] = unbiased_calc->get_result( window, provided_loci, filter_stats_ );
            } else {
                results_[i] = raw_calc->get_result();
            }
        }
        return results_;
    }

    /**
     * @brief Get a list of all resulting FST values for all pairs of samples.
     *
     * This overload does not use window averaging, and always returns the sum.
     */
    std::vector<double> const& get_result() const
    {
        assert( results_.size() == calculators_.size() );
        for( size_t i = 0; i < results_.size(); ++i ) {
            // No dispatch here as in the above overload. Instead, we just use the result function
            // that does not use window averaging directly.
            results_[i] = calculators_[i]->get_result();
        }
        return results_;
    }

    /**
     * @brief Get lists of all the three intermediate pi values (within, between, total) that
     * are part of our unbiased estimators.
     *
     * This computes the window-average-corrected values for pi within, pi between, and pi total
     * (in that order in the tuple), for each sample pair (order in the three vectors).
     * This uses the same window averaging policy as the get_result() function.
     *
     * This only works when all calculators are of type FstPoolCalculatorUnbiased, and throws an
     * exception otherwise. It is merely meant as a convenience function for that particular case.
     */
    template<class D>
    PiVectorTuple const& get_pi_vectors(
        BaseWindow<D> const& window,
        std::shared_ptr<GenomeLocusSet> provided_loci
    ) const {
        allocate_pi_result_vectors_();

        // Get the pi values from all calculators, assuming that they are of the correct type.
        for( size_t i = 0; i < calculators_.size(); ++i ) {
            auto const& raw_calc = calculators_[i];
            auto const* unbiased_calc = dynamic_cast<FstPoolCalculatorUnbiased const*>( raw_calc.get() );
            if( ! unbiased_calc ) {
                throw std::domain_error(
                    "Can only call FstPoolProcessor::get_pi_vectors() "
                    "for calculators of type FstPoolCalculatorUnbiased."
                );
            }

            // We compute the window-averaged values here.
            // Unfortunately, we need to copy this value-by-value, as we want to return
            // three independent vectors for user convenienec on the caller's end.
            auto const pis = unbiased_calc->get_pi_values( window, provided_loci, filter_stats_ );
            std::get<0>(results_pi_)[i] = pis.pi_within;
            std::get<1>(results_pi_)[i] = pis.pi_between;
            std::get<2>(results_pi_)[i] = pis.pi_total;
        }

        return results_pi_;
    }

    /**
     * @brief Get lists of all the three intermediate pi values (within, between, total) that
     * are part of our unbiased estimators.
     *
     * This overload ignores the window average policy, and just returns the sum.
     */
    PiVectorTuple const& get_pi_vectors() const
    {
        // Same as above, but using a different get_pi_vectors() overload.
        allocate_pi_result_vectors_();
        for( size_t i = 0; i < calculators_.size(); ++i ) {
            auto const& raw_calc = calculators_[i];
            auto const* unbiased_calc = dynamic_cast<FstPoolCalculatorUnbiased const*>( raw_calc.get() );
            if( ! unbiased_calc ) {
                throw std::domain_error(
                    "Can only call FstPoolProcessor::get_pi_vectors() "
                    "for calculators of type FstPoolCalculatorUnbiased."
                );
            }
            auto const pis = unbiased_calc->get_pi_values();
            std::get<0>(results_pi_)[i] = pis.pi_within;
            std::get<1>(results_pi_)[i] = pis.pi_between;
            std::get<2>(results_pi_)[i] = pis.pi_total;
        }

        return results_pi_;
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

    std::vector<std::pair<size_t, size_t>> const& sample_pairs() const
    {
        return sample_pairs_;
    }

    std::vector<std::unique_ptr<BaseFstPoolCalculator>> const& calculators() const
    {
        return calculators_;
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    void allocate_pi_result_vectors_() const
    {
        // Only allocate when someone first calls this.
        // Does not do anything afterwards.
        auto const res_sz = calculators_.size();
        assert( std::get<0>( results_pi_ ).size() == 0 || std::get<0>( results_pi_ ).size() == res_sz );
        assert( std::get<1>( results_pi_ ).size() == 0 || std::get<1>( results_pi_ ).size() == res_sz );
        assert( std::get<2>( results_pi_ ).size() == 0 || std::get<2>( results_pi_ ).size() == res_sz );
        std::get<0>( results_pi_ ).resize( res_sz );
        std::get<1>( results_pi_ ).resize( res_sz );
        std::get<2>( results_pi_ ).resize( res_sz );
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    // The pairs of sample indices of the variant between which we want to compute FST,
    // and the processors to use for these computations,
    std::vector<std::pair<size_t, size_t>> sample_pairs_;
    std::vector<std::unique_ptr<BaseFstPoolCalculator>> calculators_;

    // Count how many Variants were processed in this processor,
    // and how many of them passed or failed the filters.
    VariantFilterStats filter_stats_;

    // We keep a mutable cache for the results, to avoid reallocating memory each time.
    // We do the same of the pi values, but this is only allocated when first called.
    mutable std::vector<double> results_;
    mutable PiVectorTuple       results_pi_;

    // Thread pool to run the buffering in the background, and the size
    // (number of sample pairs) at which we start using the thread pool.
    std::shared_ptr<utils::ThreadPool> thread_pool_;
    size_t threading_threshold_ = 0;
};

// =================================================================================================
//     Make Fst Processor Helper Functions
// =================================================================================================

/**
 * @brief Create an FstPoolProcessor for all-to-all computation of FST between all pairs of samples.
 *
 * The function expects the pool sizes of all samples, as well as any additional @p args to be
 * provided to each processor after the pair of pool sizes. It then yields a processor that can be
 * provided with all Variant%s of interest along the genome, and computes FST between all pairs
 * of their samples.
 */
template<class Calculator, typename... Args>
inline FstPoolProcessor make_fst_pool_processor(
    std::vector<size_t> const& pool_sizes,
    Args... args
) {
    FstPoolProcessor result;
    for( size_t i = 0; i < pool_sizes.size(); ++i ) {
        for( size_t j = i + 1; j < pool_sizes.size(); ++j ) {
            result.add_calculator(
                i, j,
                ::genesis::utils::make_unique<Calculator>(
                    pool_sizes[i],
                    pool_sizes[j],
                    args...
                )
            );
        }
    }
    return result;
}

/**
 * @brief Create an FstPoolProcessor for computation of FST between specific pairs of samples.
 *
 * The function expects the the pool sizes of all samples, as well as the pairs of indices of the
 * Variant::samples SampleCounts between which FST shall be calculated, and any additional @p args to
 * be provided to each processor after the pair of pool sizes.
 * It then yields a processor that can be provided with all Variant%s of interest along the genome,
 * and computes FST between all provided pairs of their samples.
 */
template<class Calculator, typename... Args>
inline FstPoolProcessor make_fst_pool_processor(
    std::vector<std::pair<size_t, size_t>> const& sample_pairs,
    std::vector<size_t> const& pool_sizes,
    Args... args
) {
    FstPoolProcessor result;
    for( auto const& p : sample_pairs ) {
        if( p.first >= pool_sizes.size() || p.second >= pool_sizes.size() ) {
            throw std::invalid_argument(
                "Invalid sample indices for computing FST Pool Kofler: " +
                std::to_string( pool_sizes.size() ) +
                " pool sizes provided, but asked to use indices " +
                std::to_string( p.first ) + " and " + std::to_string( p.second )
            );
        }
        result.add_calculator(
            p.first, p.second,
            ::genesis::utils::make_unique<Calculator>(
                pool_sizes[p.first],
                pool_sizes[p.second],
                args...
            )
        );
    }
    return result;
}

/**
 * @brief Create an FstPoolProcessor for one-to-all FST computation between one sample and all others.
 *
 * The function expects the pool sizes of all samples, as well as the index of the
 * Variant::samples SampleCounts object between which FST to all other samples shall be calculated,
 * and any additional @p args to be provided to each processor after the pair of pool sizes.
 * It then yields a processor that can be provided with all Variant%s of interest along the genome,
 * and computes FST between the given index and all other samples.
 */
template<class Calculator, typename... Args>
inline FstPoolProcessor make_fst_pool_processor(
    size_t index,
    std::vector<size_t> const& pool_sizes,
    Args... args
) {
    FstPoolProcessor result;
    for( size_t i = 0; i < pool_sizes.size(); ++i ) {
        result.add_calculator(
            index, i,
            ::genesis::utils::make_unique<Calculator>(
                pool_sizes[index],
                pool_sizes[i],
                args...
            )
        );
    }
    return result;
}

/**
 * @brief Create an FstPoolProcessor for one-to-one FST computation between two samples.
 *
 * The function expects the pool sizes of all samples, as well as two indices of the
 * Variant::samples SampleCounts objects between which FST shall be calculated,
 * and any additional @p args to be provided to each processor after the pair of pool sizes.
 * It then yields a processor that can be provided with all Variant%s of interest along the genome,
 * and computes FST between the given pair of samples.
 */
template<class Calculator, typename... Args>
inline FstPoolProcessor make_fst_pool_processor(
    size_t index_1, size_t index_2,
    std::vector<size_t> const& pool_sizes,
    Args... args
) {
    FstPoolProcessor result;
    result.add_calculator(
        index_1, index_2,
        ::genesis::utils::make_unique<Calculator>(
            pool_sizes[index_1],
            pool_sizes[index_2],
            args...
        )
    );
    return result;
}

// =================================================================================================
//     Sample Names Helper Function
// =================================================================================================

/**
 * @brief Return a list of sample name pairs for each calculator in an FstPoolProcessor.
 *
 * The function takes a @p processor, and the original list of @p sample_names of the samples in the
 * calculators in the @p processor, and uses their indices (as stored in the @p processor) to get
 * pairs of sample names.
 */
inline std::vector<std::pair<std::string, std::string>> fst_pool_processor_sample_names(
    FstPoolProcessor const& processor,
    std::vector<std::string> const& sample_names
) {
    // Without sample names given, we just return an empty pair.
    if( sample_names.empty() ) {
        return {};
    }

    // Prepare the actual result vector.
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve( processor.size() );

    // Make a list of sample name pairs, one for each calculator in the processor.
    assert( processor.sample_pairs().size() == processor.size() );
    for( auto const& sample_pair : processor.sample_pairs() ) {
        if( sample_pair.first >= sample_names.size() || sample_pair.second >= sample_names.size() ) {
            throw std::invalid_argument(
                "In fst_pool_processor_sample_names(): sample names at indices " +
                std::to_string( sample_pair.first ) + " and " + std::to_string( sample_pair.second ) +
                " requested, but sample names with " + std::to_string( sample_names.size() ) +
                " entries given."
            );
        }
        result.push_back( std::make_pair(
            sample_names[ sample_pair.first ], sample_names[ sample_pair.second ]
        ));
    }
    return result;
}

} // namespace population
} // namespace genesis

#endif // include guard
