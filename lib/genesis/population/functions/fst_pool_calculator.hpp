#ifndef GENESIS_POPULATION_FUNCTIONS_FST_POOL_CALCULATOR_H_
#define GENESIS_POPULATION_FUNCTIONS_FST_POOL_CALCULATOR_H_

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

#include "genesis/population/base_counts.hpp"
#include "genesis/population/variant.hpp"

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

} // namespace population
} // namespace genesis

#endif // include guard
