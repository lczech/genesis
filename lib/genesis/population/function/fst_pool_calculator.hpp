#ifndef GENESIS_POPULATION_FUNCTION_FST_POOL_CALCULATOR_H_
#define GENESIS_POPULATION_FUNCTION_FST_POOL_CALCULATOR_H_

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
#include "genesis/population/sample_counts.hpp"
#include "genesis/population/variant.hpp"

#include <algorithm>
#include <cassert>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Fst Pool Calculator
// =================================================================================================

/**
 * @brief Base class to compute FST between two pooled samples, given two instances of SampleCounts.
 *
 * The class is to be derived from for the actual computation, such as in FstPoolCalculatorKarlsson,
 * FstPoolCalculatorKofler, or FstPoolCalculatorUnbiased, with three virtual methods to override.
 * Then, an instance of such a calculator can be provided to FstPoolProcessor, which processes
 * Variant%s along a genome, computing FST along the way.
 *
 * The idea of using classes here instead of simple iterators over Variant%s, such as
 * VariantInputStream, is that we want to be able to compute FST for many pairs of samples in
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
        reset_();
        filter_stats_.clear();
    }

    void process( SampleCounts const& p1, SampleCounts const& p2 )
    {
        // For now, we accept two populations as input here.
        // If we ever implement an FST equation that also works for multiple populations,
        // see https://stackoverflow.com/a/9377363/4184258 for a way to implement this.
        // Or simply make this function accept the Variant instead, and store the needed sample
        // indices here, instead of the list of sample pairs stored in the FstPoolProcessor.

        // We only want to process samples that are both passing.
        // Otherwise, if both fail, we want to record that, but it does not really matter which
        // of the two reasons for failing we record. However, as we generally have our filter status
        // tags numerically sorted in the order in which they are applied, we here use the first
        // tag (smaller number) of the two samples that failed, as that is usually a better and
        // more general idicator of what failed. Probably does not really matter much anyway,
        // but why not. Lastly, if only one of the samples failed, we just record that one.
        // This can be done as the max of the two values, as one of them will be zero.
        if( p1.status.passing() && p2.status.passing() ) {
            process_( p1, p2 );
            ++filter_stats_[SampleCountsFilterTag::kPassed];
        } else if( ! p1.status.passing() && ! p2.status.passing() ) {
            ++filter_stats_[ std::min( p1.status.get(), p2.status.get() )];
        } else {
            ++filter_stats_[ std::max( p1.status.get(), p2.status.get() )];
        }
    }

    double get_result() const
    {
        return get_result_();
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
    //     Abstract Members
    // -------------------------------------------------------------------------

protected:

    // Non-virtual interface.
    virtual void reset_() = 0;
    virtual void process_( SampleCounts const& p1, SampleCounts const& p2 ) = 0;
    virtual double get_result_() const = 0;

    // -------------------------------------------------------------------------
    //     Private Members
    // -------------------------------------------------------------------------

private:

    // Count how many sample pairs were processed here,
    // and how many of them passed or failed the filters.
    SampleCountsFilterStats filter_stats_;

};

} // namespace population
} // namespace genesis

#endif // include guard
