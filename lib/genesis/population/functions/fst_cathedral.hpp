#ifndef GENESIS_POPULATION_FUNCTIONS_FST_CATHEDRAL_H_
#define GENESIS_POPULATION_FUNCTIONS_FST_CATHEDRAL_H_

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

#include "genesis/population/formats/variant_input_iterator.hpp"
#include "genesis/population/functions/fst_pool_processor.hpp"
#include "genesis/population/functions/fst_pool_unbiased.hpp"
#include "genesis/population/plotting/cathedral_plot.hpp"
#include "genesis/sequence/sequence_dict.hpp"
#include "genesis/utils/containers/matrix.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Fst Cathedral Data
// =================================================================================================

/**
 * @brief Data for making one FST cathedral plot, that is, one pair of samples and one chromosome.
 *
 * We only offer this plot for our two unbiased estimators, but not for the Kofler and Karlsson
 * estimators. Hence, we keep the per position pi values here, so that they can then be accumulated
 * into the per-pixel values for the plot later.
 */
struct FstCathedralData
{
    struct Entry
    {
        Entry( size_t pos_, double pi_w_, double pi_b_, double pi_t_ )
            : pos( pos_ )
            , pi_within( pi_w_ )
            , pi_between( pi_b_ )
            , pi_total( pi_t_ )
        {}

        size_t pos   = 0;
        double pi_within  = 0.0;
        double pi_between = 0.0;
        double pi_total   = 0.0;
    };

    // General data for bookkeeping
    std::string sample_name_1;
    std::string sample_name_2;
    std::string chromosome;
    size_t chromosome_length = 0;

    // The actual components of FST values per position
    std::vector<Entry> entries;
};

/**
 * @brief Accumulate the partial pi values for a given window to produce a cathedral plot.
 */
struct FstCathedralAccumulator
{
    FstCathedralAccumulator(
        FstPoolCalculatorUnbiased::Estimator fst_estimator_ =
        FstPoolCalculatorUnbiased::Estimator::kNei
    )
        : fst_estimator( fst_estimator_ )
    {}

    void accumulate( FstCathedralData::Entry const& entry )
    {
        if(
            ! std::isfinite( entry.pi_within )  ||
            ! std::isfinite( entry.pi_between ) ||
            ! std::isfinite( entry.pi_total )
        ) {
            return;
        }

        pi_within_sum  += entry.pi_within;
        pi_between_sum += entry.pi_between;
        pi_total_sum   += entry.pi_total;
    }

    double get_result() const
    {
        double result;
        switch( fst_estimator ) {
            case FstPoolCalculatorUnbiased::Estimator::kNei: {
                result = 1.0 - ( pi_within_sum / pi_total_sum );
                break;
            }
            case FstPoolCalculatorUnbiased::Estimator::kHudson: {
                result = 1.0 - ( pi_within_sum / pi_between_sum );
                break;
            }
            default: {
                throw std::invalid_argument(
                    "FstCathedralAccumulator: Invalid FstPoolCalculatorUnbiased::Estimator"
                );
            }
        }
        return result;
    }

    void reset()
    {
        pi_within_sum  = 0.0;
        pi_between_sum = 0.0;
        pi_total_sum   = 0.0;
    }

    FstPoolCalculatorUnbiased::Estimator fst_estimator;
    double pi_within_sum  = 0.0;
    double pi_between_sum = 0.0;
    double pi_total_sum   = 0.0;
};

// =================================================================================================
//     Fst Cathedral Functions
// =================================================================================================

/**
 * @brief Compute the components of per-position FST data for all pairs of samples in the given
 * @p processor, for the chromsomes in the given input @p iterator.
 *
 * The result contains entries for all pairs of samples and all chromosomes, in one vector.
 *
 * This expects the processor to only contain FstPoolCalculatorUnbiased calculators, as those
 * are the only ones for which we can compute cathedral plots with our current implementation.
 *
 * If given @p sample_names, we use those to set the sample names in the resulting FstCathedralData
 * objects, so that downstream we can keep track of them.
 *
 * If given a @p sequence_dict, we use the information in there to set the chromosome length;
 * otherwise, we use the last position found in the data for that.
 */
std::vector<FstCathedralData> compute_fst_cathedral_data(
    VariantInputIterator& iterator,
    FstPoolProcessor& processor,
    std::vector<std::string> const& sample_names = std::vector<std::string>{},
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict = nullptr
);

genesis::utils::Matrix<double> compute_fst_cathedral_matrix(
    FstCathedralData const& data,
    size_t width, size_t height,
    CathedralWindowWidthMethod window_width_method = CathedralWindowWidthMethod::kExponential,
    FstPoolCalculatorUnbiased::Estimator fst_estimator = FstPoolCalculatorUnbiased::Estimator::kNei
) {
    auto accu = FstCathedralAccumulator( fst_estimator );
    return compute_cathedral_matrix<FstCathedralData, FstCathedralAccumulator>(
        data, accu, width, height, window_width_method
    );
}

} // namespace population
} // namespace genesis

#endif // include guard
