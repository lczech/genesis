#ifndef GENESIS_POPULATION_FUNCTIONS_FST_POOL_H_
#define GENESIS_POPULATION_FUNCTIONS_FST_POOL_H_

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

#include "genesis/population/variant.hpp"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Fst Pool Calculator
// =================================================================================================

/**
 * @brief Base class to compute FST between two pooled samples, given two instances of BaseCounts.
 *
 * The class is to be derived from for the actual computation, such as in FstPoolCalculatorKarlsson,
 * FstPoolCalculatorKofler, or FstPoolCalculatorUnbiased, with three virtual methods to override.
 * Then, an instance of such a calculator can be provided to FstPoolProcessor, which processes
 * Variant%s along a genome, computing FST along the way.
 *
 * The idea of using classes here instead of simple iterators over Variant%s, such as
 * VariantInputIterator, is that we want to be able to compute FST for many pairs of samples in
 * some input. With input iterators that just read the input files once however, we cannot iterate
 * multiple times over the same input. Hence we would have to keep all Variant%s in memory to be
 * able to compute FST for multiple pairs - which is not desirable when, e.g., computing FST
 * for the whole genome at once... So instead, we use these classes, which accumulate all needed
 * data along the way, and yield the FST value at the very end when calling get_result().
 */
class BaseFstPoolCalculator
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    BaseFstPoolCalculator() = default;
    virtual ~BaseFstPoolCalculator() = default;

    BaseFstPoolCalculator( BaseFstPoolCalculator const& ) = default;
    BaseFstPoolCalculator( BaseFstPoolCalculator&& )      = default;

    BaseFstPoolCalculator& operator= ( BaseFstPoolCalculator const& ) = default;
    BaseFstPoolCalculator& operator= ( BaseFstPoolCalculator&& )      = default;

    // -------------------------------------------------------------------------
    //     Calculator Functions
    // -------------------------------------------------------------------------

    void reset()
    {
        return reset_();
    }

    void process( BaseCounts const& p1, BaseCounts const& p2 )
    {
        // For now, we accept two populations as input here.
        // If we ever implement an FST equation that also works for multiple populations,
        // see https://stackoverflow.com/a/9377363/4184258 for a way to implement this.
        return process_( p1, p2 );
    }

    double get_result()
    {
        return get_result_();
    }

    // -------------------------------------------------------------------------
    //     Abstract Members
    // -------------------------------------------------------------------------

protected:

    // Non-virtual interface.
    virtual void reset_() = 0;
    virtual void process_( BaseCounts const& p1, BaseCounts const& p2 ) = 0;
    virtual double get_result_() = 0;

};

// =================================================================================================
//     Fst Pool Processor
// =================================================================================================

/**
 * @brief Helper class to iterate over Variant%s and process pairs of FST between their samples
 * (BaseCounts), using a set of BaseFstPoolCalculator.
 */
class FstPoolProcessor
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    FstPoolProcessor() = default;
    ~FstPoolProcessor() = default;

    FstPoolProcessor( FstPoolProcessor const& ) = default;
    FstPoolProcessor( FstPoolProcessor&& )      = default;

    FstPoolProcessor& operator= ( FstPoolProcessor const& ) = default;
    FstPoolProcessor& operator= ( FstPoolProcessor&& )      = default;

    // -------------------------------------------------------------------------
    //     Setup
    // -------------------------------------------------------------------------

    void add_calculator(
        size_t index_p1, size_t index_p2,
        std::unique_ptr<BaseFstPoolCalculator> calculator
    ) {
        assert( sample_pairs_.size() == calculators_.size() );
        assert( sample_pairs_.size() == results_.size() );
        sample_pairs_.push_back({ index_p1, index_p2 });
        calculators_.push_back( std::move( calculator ));
        results_.push_back( 0.0 );
    }

    // -------------------------------------------------------------------------
    //     Calculator Functions
    // -------------------------------------------------------------------------

    void reset()
    {
        for( auto& calc : calculators_ ) {
            assert( calc );
            calc->reset();
        }
    }

    void process( Variant const& variant )
    {
        assert( sample_pairs_.size() == calculators_.size() );
        for( size_t i = 0; i < sample_pairs_.size(); ++i ) {
            auto const& indices = sample_pairs_[i];
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
            calculators_[i]->process(
                variant.samples[indices.first],
                variant.samples[indices.second]
            );
        }
    }

    std::vector<double> const& get_result()
    {
        assert( results_.size() == calculators_.size() );
        for( size_t i = 0; i < results_.size(); ++i ) {
            results_[i] = calculators_[i]->get_result();
        }
        return results_;
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    std::vector<std::pair<size_t, size_t>> sample_pairs_;
    std::vector<std::unique_ptr<BaseFstPoolCalculator>> calculators_;
    std::vector<double> results_;

};

// =================================================================================================
//     Helper Functions with Pool Sizes
// =================================================================================================

/**
 * @brief Compute all-to-all FST for calculators that use pool sizes.
 */
template<class Calculator>
inline FstPoolProcessor make_fst_pool_processor(
    std::vector<size_t> const& pool_sizes
) {
    FstPoolProcessor result;
    for( size_t i = 0; i < pool_sizes.size(); ++i ) {
        for( size_t j = i + 1; j < pool_sizes.size(); ++j ) {
            result.add_calculator(
                i, j,
                ::genesis::utils::make_unique<Calculator>(
                    pool_sizes[i],
                    pool_sizes[j]
                )
            );
        }
    }
    return result;
}

/**
 * @brief Compute FST for specific pairs of samples, for calculators that use pool sizes.
 */
template<class Calculator>
inline FstPoolProcessor make_fst_pool_processor(
    std::vector<size_t> const& pool_sizes,
    std::vector<std::pair<size_t, size_t>> const& sample_pairs
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
                pool_sizes[p.second]
            )
        );
    }
    return result;
}

/**
 * @brief Compute one-to-all FST for calculators that use pool sizes.
 */
template<class Calculator>
inline FstPoolProcessor make_fst_pool_processor(
    std::vector<size_t> const& pool_sizes,
    size_t index
) {
    FstPoolProcessor result;
    for( size_t i = 0; i < pool_sizes.size(); ++i ) {
        result.add_calculator(
            index, i,
            ::genesis::utils::make_unique<Calculator>(
                pool_sizes[index],
                pool_sizes[i]
            )
        );
    }
    return result;
}

/**
 * @brief Compute one-to-one FST for calculators that use pool sizes.
 */
template<class Calculator>
inline FstPoolProcessor make_fst_pool_processor(
    std::vector<size_t> const& pool_sizes,
    size_t index_1, size_t index_2
) {
    FstPoolProcessor result;
    result.add_calculator(
        index_1, index_2,
        ::genesis::utils::make_unique<Calculator>(
            pool_sizes[index_1],
            pool_sizes[index_2]
        )
    );
    return result;
}

} // namespace population
} // namespace genesis

#endif // include guard
