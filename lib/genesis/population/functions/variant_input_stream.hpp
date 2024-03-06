#ifndef GENESIS_POPULATION_FUNCTIONS_VARIANT_INPUT_STREAM_H_
#define GENESIS_POPULATION_FUNCTIONS_VARIANT_INPUT_STREAM_H_

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

#include "genesis/population/streams/variant_input_stream.hpp"
#include "genesis/population/functions/genome_locus.hpp"
#include "genesis/population/genome_locus.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/sequence/functions/dict.hpp"
#include "genesis/sequence/sequence_dict.hpp"

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Sample Name Filter
// =================================================================================================

/**
 * @brief Create a filter for samples, indicating which to keep.
 *
 * The resulting bool vector has the same length as the input @p sample_names vector,
 * and is `true` for all samples that are meant to be kept, and `false` otherwise.
 * By default, with `inverse_filter == false`, sample names that are in the @p names_filter are
 * kept, and those that are not are not kept. With `inverse_filter == true`, this is reversed.
 *
 * The function also checks that @p sample_names and @p names_filter are unique (as otherwise
 * the filtering might be wrong), and that the names in the @p names_filter actually appear in the
 * @p sample_names.
 */
std::vector<bool> make_sample_name_filter(
    std::vector<std::string> const& sample_names,
    std::vector<std::string> const& names_filter,
    bool inverse_filter = false
);

/**
 * @brief Helper function to create a Variant transform to filter out samples.
 *
 * The function expects a bool vector indicating which samples within a Variant to keep.
 * The vector needs to have the same length as the Variant has samples. It can be created for instance
 * with make_sample_name_filter() based on sample names. However, as Variant does not store the names
 * itself, those might need to be accessed from the VariantInputStream data() function,
 * which yields a VariantInputStreamData object.
 *
 * Using this to filter samples by their name is likely somewhat slower than doing it direclty
 * in the parsers, which we also offer. However, this way offers a unified and simple way to achieve
 * the filtering, as it is applied down the line, and hence can be used on any VariantInputStream.
 */
std::function<void(Variant&)> make_variant_input_stream_sample_name_filter_transform(
    std::vector<bool> const& sample_filter
);

// =================================================================================================
//     Sample Subsetting / Subsampling
// =================================================================================================

/**
 * @brief Select which method to use for reducing the max coverage of a BaseCounts sample or a
 * Variant.
 *
 * See make_variant_input_stream_sample_subsampling_transform() for usage.
 */
enum class SubsamplingMethod
{
    /**
     * @brief Use transform_subscale()
     */
    kSubscale,

    /**
     * @brief Use transform_subsample_with_replacement()
     */
    kSubsampleWithReplacement,

    /**
     * @brief Use transform_subsample_without_replacement()
     */
    kSubsampleWithoutReplacement
};

/**
 * @brief Create a Variant transformation function that subscales or subsamples the base counts
 * to be below a given @p max_coverage.
 *
 * This is intended to be used as a transformation on a VariantInputStream, see
 * @link ::genesis::utils::GenericInputStream::add_transform() add_transform()@endlink for details.
 * The function creates a transformation function to be used on a stream, and subsamples or
 * subscales the BaseCounts of each Variant in the stream, so that @p max_coverage is not
 * exceeded. This is useful for instance when computing the pool sequencing diversity estimators,
 * which have computational terms depending on coverage; reducing high coverage can hence help
 * to improve computational time.
 *
 * By default, we use SubsamplingMethod::kSubscale, which is the closest to a lossless reduction
 * of the coverage that can be achieved with integer counts. The two other methods instead
 * resample from a distribution based on the given counts of the Variant, and can hence also be
 * used to create in-silico alternative populations based on the original sample.
 */
std::function<void(Variant&)> make_variant_input_stream_sample_subsampling_transform(
    size_t max_coverage,
    SubsamplingMethod method = SubsamplingMethod::kSubscale
);

// =================================================================================================
//     Observers
// =================================================================================================

/**
 * @brief Helper function to check that some Variant input is sorted properly.
 *
 * The function creates a `std::function` that can be used with a VariantInputStream to check
 * the order (and length) of the Variant%s being processed.
 *
 * By default, the different types of VariantInputStream%s that we create for different file types
 * with the `make_variant_input_stream_from_...()` simply iterate over their respective input
 * files as they are. However, we might want to check that their order is correct, or that their
 * lengths fit our expectation.
 *
 * This function by default checks this, using lexicographical order for the chromosomes, and
 * numerical order for the positions within chromosomes. This however might not always be the order
 * as present in the input source. In order to provide a custom order, the function optinally takes
 * a @link ::genesis::sequence::SequenceDict SequenceDict@endlink, which is used for the order
 * instead. See there for details on how to create such a dictionary from, e.g., `.fai`, `.dict`,
 * or reference genome `.fasta` files.
 *
 * Furthermore, when a @p sequence_dict is provided, using the @p check_sequence_lengths, we can
 * also check that the positions within each chromosome that we encounter in the input source fit
 * with the expectations of that dictionary. This serves as an additional sanity check of the
 * input files.
 *
 * If any of these checks fail, an exception is thrown.
 *
 * Exemplary usage:
 *
 *     // Get a sequence dict from a fai file
 *     auto sequence_dict = read_sequence_fai( from_file( fai_file ));
 *
 *     // Create a VariantInputStream, for example from a sync file
 *     auto variant_stream = make_variant_input_stream_from_sync_file( sync_file );
 *
 *     // Add the observer that checks order, using a
 *     variant_stream.add_observer(
 *         make_variant_input_stream_sequence_order_observer(
 *             sequence_dict, true
 *         )
 *     );
 *
 *     // Use the iterator as usual.
 *     for( auto const& variant : variant_stream ) {
 *         // ...
 *     }
 *
 * @see See @link ::genesis::utils::GenericInputStream::add_observer() GenericInputStream::add_observer()@endlink
 * for the function of the underlying iterator that accepts the returned function from here.
 *
 * @see make_variant_input_stream_from_vector(),
 * make_variant_input_stream_from_sam_file(),
 * make_variant_input_stream_from_pileup_file(),
 * make_variant_input_stream_from_sync_file(),
 * make_variant_input_stream_from_frequency_table_file(),
 * make_variant_input_stream_from_pool_vcf_file(),
 * make_variant_input_stream_from_individual_vcf_file(),
 * make_variant_input_stream_from_variant_parallel_input_stream(),
 * make_variant_input_stream_from_variant_gapless_input_stream(),
 * make_variant_merging_input_stream()
 */
std::function<void(Variant const&)> make_variant_input_stream_sequence_order_observer(
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict = {},
    bool check_sequence_lengths = true
);

/**
 * @brief Helper function to check that some Variant input has positions that agree with those
 * reported in a SequenceDict.
 *
 * Similar to make_variant_input_stream_sequence_order_observer(), but without the sequence order
 * check. Meant for situations where this order check is either not necessary, or already done in
 * some other way, for example in a VariantParallelInputStream.
 *
 * See make_variant_input_stream_sequence_order_observer() for details on usage.
 */
std::function<void(Variant const&)> make_variant_input_stream_sequence_length_observer(
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict
);

} // namespace population
} // namespace genesis

#endif // include guard
