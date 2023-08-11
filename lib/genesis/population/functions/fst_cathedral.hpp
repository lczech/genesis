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
#include "genesis/utils/math/compensated_sum.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

// =================================================================================================
//     Forward declacations
// =================================================================================================

namespace genesis {
namespace utils {

    class JsonDocument;

} // namespace utils
} // namespace genesis

namespace genesis {
namespace population {

// =================================================================================================
//     Fst Cathedral Plot Record
// =================================================================================================

/**
 * @brief Data for making one FST cathedral plot, that is, one pair of samples and one chromosome.
 *
 * We only offer this plot for our two unbiased estimators, but not for the Kofler and Karlsson
 * estimators. Hence, we keep the per position pi values here, so that they can then be accumulated
 * into the per-pixel values for the plot later.
 */
struct FstCathedralPlotRecord final : public CathedralPlotRecord
{
    struct Entry
    {
        Entry( size_t pos_, double pi_w_, double pi_b_, double pi_t_ )
            : position( pos_ )
            , pi_within( pi_w_ )
            , pi_between( pi_b_ )
            , pi_total( pi_t_ )
        {}

        size_t position   = 0;
        double pi_within  = 0.0;
        double pi_between = 0.0;
        double pi_total   = 0.0;
    };

    // The actual components of FST values per position.
    std::vector<Entry> entries;

    // Data-derived properties.
    std::string sample_name_1;
    std::string sample_name_2;

    // User-provided properties.
    // Type of accumulator. We store all three pi values here independently though,
    // to keep it simple, but use this to know what estimator was used for the data.
    FstPoolCalculatorUnbiased::Estimator fst_estimator;
};

// =================================================================================================
//     Fst Cathedral Accumulator
// =================================================================================================

/**
 * @brief Accumulate the partial pi values for a given window to produce a cathedral plot.
 */
class FstCathedralAccumulator
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    FstCathedralAccumulator(
        FstPoolCalculatorUnbiased::Estimator fst_estimator
    )
        : fst_estimator_( fst_estimator )
    {}

    ~FstCathedralAccumulator() = default;

    FstCathedralAccumulator( FstCathedralAccumulator const& ) = default;
    FstCathedralAccumulator( FstCathedralAccumulator&& )      = default;

    FstCathedralAccumulator& operator= ( FstCathedralAccumulator const& ) = default;
    FstCathedralAccumulator& operator= ( FstCathedralAccumulator&& )      = default;

    // -------------------------------------------------------------------------
    //     Accumulator Functions
    // -------------------------------------------------------------------------

    void accumulate( FstCathedralPlotRecord::Entry const& entry );
    void dissipate( FstCathedralPlotRecord::Entry const& entry );
    double aggregate() const;
    void reset();

    // -------------------------------------------------------------------------
    //     Private Member Variables
    // -------------------------------------------------------------------------

private:

    // Type of accumulator.
    FstPoolCalculatorUnbiased::Estimator fst_estimator_;

    // Store our accumualted values. We are using a Neumaier summation here,
    // as we might be adding and subtracting values of different orders of magnitude,
    // which would lead to large errors with the standard Kahan sum.
    utils::NeumaierSum pi_within_sum_  = 0.0;
    utils::NeumaierSum pi_between_sum_ = 0.0;
    utils::NeumaierSum pi_total_sum_   = 0.0;
    size_t value_count_ = 0;
};

// =================================================================================================
//     Fst Cathedral Functions
// =================================================================================================

/**
 * @brief Compute the components of per-position FST data for all pairs of samples in the given
 * @p processor, for the current chromsome in the given input @p iterator.
 *
 * The result contains entries for all pairs of samples. The computation starts at the current
 * position in @p iterator, uses that chromosome, and iterates until its end or until the next
 * chromosome is found, and stops there. See compute_fst_cathedral_records() for a helper function
 * that does this for all chromsomes in the input.
 *
 * This expects the processor to only contain FstPoolCalculatorUnbiased calculators, as those
 * are the only ones for which we can compute cathedral plots with our current implementation.
 *
 * If given @p sample_names, we use those to set the sample names in the resulting
 * FstCathedralPlotRecord objects, so that downstream we can keep track of them.
 *
 * If given a @p sequence_dict, we use the information in there to set the chromosome length;
 * otherwise, we use the last position found in the data for that.
 */
std::vector<FstCathedralPlotRecord> compute_fst_cathedral_records_for_chromosome(
    VariantInputIterator::Iterator& iterator,
    FstPoolProcessor& processor,
    FstPoolCalculatorUnbiased::Estimator fst_estimator,
    std::vector<std::string> const& sample_names = std::vector<std::string>{},
    std::shared_ptr<genesis::sequence::SequenceDict> const& sequence_dict = nullptr
);

/**
 * @brief Compute the components of per-position FST data for all pairs of samples in the given
 * @p processor, for the chromsomes in the given input @p iterator.
 *
 * The result contains entries for all pairs of samples and all chromosomes, in one vector.
 * This is a convenience function that calls compute_fst_cathedral_records_for_chromosome() for each
 * chromosome.
 * We however do not recommend this for larger datasets, as the resulting data can be quite
 * memory-intense. It might hence be better to use this per-chromosome function instead,
 * and process the returned data before starting with the next chromosome.
 */
std::vector<FstCathedralPlotRecord> compute_fst_cathedral_records(
    VariantInputIterator& iterator,
    FstPoolProcessor& processor,
    FstPoolCalculatorUnbiased::Estimator fst_estimator,
    std::vector<std::string> const& sample_names = std::vector<std::string>{},
    std::shared_ptr<genesis::sequence::SequenceDict> const& sequence_dict = nullptr
);

/**
 * @brief Compute the matrix of values that represents the cathedral plot for FST.
 *
 * This is merely a shortcut to call compute_cathedral_matrix() with the arguments for a cathedral
 * plot of FST, using the result of compute_fst_cathedral_records().
 * The returned matrix can then be plotted as a heatmap.
 */
void compute_fst_cathedral_matrix(
    CathedralPlotParameters const& parameters,
    FstCathedralPlotRecord& record
) {
    auto accu = FstCathedralAccumulator( record.fst_estimator );
    return compute_cathedral_matrix<FstCathedralPlotRecord, FstCathedralAccumulator>(
        parameters, record, accu
    );
}

// =================================================================================================
//     Storage Functions
// =================================================================================================

/**
 * @brief Get a user-readable description of the data of a FstCathedralPlotRecord as a
 * @link genesis::utils::JsonDocument JsonDocument@endlink.
 *
 * @see cathedral_plot_record_to_json_document(), cathedral_plot_parameters_to_json_document(),
 * save_cathedral_plot_record_to_files(), load_cathedral_plot_record_from_files()
 */
genesis::utils::JsonDocument fst_cathedral_plot_record_to_json_document(
    FstCathedralPlotRecord const& record
);

} // namespace population
} // namespace genesis

#endif // include guard
