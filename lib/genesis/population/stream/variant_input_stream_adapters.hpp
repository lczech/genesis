#ifndef GENESIS_POPULATION_STREAM_VARIANT_INPUT_STREAM_ADAPTERS_H_
#define GENESIS_POPULATION_STREAM_VARIANT_INPUT_STREAM_ADAPTERS_H_

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

#include "genesis/population/genome_locus_set.hpp"
#include "genesis/population/sample_counts.hpp"
#include "genesis/population/stream/variant_gapless_input_stream.hpp"
#include "genesis/population/stream/variant_input_stream_sources.hpp"
#include "genesis/population/stream/variant_input_stream.hpp"
#include "genesis/population/stream/variant_parallel_input_stream.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/sequence/reference_genome.hpp"
#include "genesis/sequence/sequence_dict.hpp"
#include "genesis/utils/containers/generic_input_stream.hpp"

#include <cassert>
#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Variant Parallel Input Stream
// =================================================================================================

/**
 * @brief Create a VariantInputStream to iterate multiple input sources at once,
 * using a VariantParallelInputStream.
 *
 * This wraps multiple input sources into one stream that traverses all of them in parallel,
 * and is here then yet again turned into a Variant per position, using
 * VariantParallelInputStream::Iterator::joined_variant() to combine all input sources into one.
 * See there for the meaning of the two `bool` parameters of this function.
 *
 * As this is iterating multiple files, we leave the VariantInputStreamData::file_path and
 * VariantInputStreamData::source_name empty, and fill the VariantInputStreamData::sample_names
 * with the sample names of the underlying input sources of the parallel stream, checking
 * for duplicates to avoid downstream trouble.
 */
VariantInputStream make_variant_input_stream_from_variant_parallel_input_stream(
    VariantParallelInputStream const& parallel_input,
    VariantParallelInputStream::JoinedVariantParams const& joined_variant_params = VariantParallelInputStream::JoinedVariantParams{}
);

// =================================================================================================
//     Variant Gapless Input Stream
// =================================================================================================

/**
 * @brief Create a VariantInputStream that wraps a VariantGaplessInputStream.
 *
 * See also make_variant_gapless_input_stream()
 */
VariantInputStream make_variant_input_stream_from_variant_gapless_input_stream(
    VariantGaplessInputStream const& gapless_input
);

/**
 * @brief Create a VariantGaplessInputStream from a VariantInputStream @p input, and wrap it
 * agian in a VariantInputStream.
 *
 * See also make_variant_input_stream_from_variant_gapless_input_stream()
 */
inline VariantInputStream make_variant_gapless_input_stream(
    VariantInputStream const& input
) {
    auto gapless_input = VariantGaplessInputStream( input );
    return make_variant_input_stream_from_variant_gapless_input_stream( gapless_input );
}

/**
 * @copybrief make_variant_gapless_input_stream( VariantInputStream const& )
 *
 * This overload additionally sets the reference genome for the gapless iteration.
 *
 * See also make_variant_input_stream_from_variant_gapless_input_stream()
 */
inline VariantInputStream make_variant_gapless_input_stream(
    VariantInputStream const& input,
    std::shared_ptr<::genesis::sequence::ReferenceGenome> ref_genome
) {
    auto gapless_input = VariantGaplessInputStream( input );
    gapless_input.reference_genome( ref_genome );
    return make_variant_input_stream_from_variant_gapless_input_stream( gapless_input );
}

/**
 * @copybrief make_variant_gapless_input_stream( VariantInputStream const& )
 *
 * This overload additionally sets the sequence dictionary for the gapless iteration.
 *
 * See also make_variant_input_stream_from_variant_gapless_input_stream()
 */
inline VariantInputStream make_variant_gapless_input_stream(
    VariantInputStream const& input,
    std::shared_ptr<::genesis::sequence::SequenceDict> seq_dict
) {
    auto gapless_input = VariantGaplessInputStream( input );
    gapless_input.sequence_dict( seq_dict );
    return make_variant_input_stream_from_variant_gapless_input_stream( gapless_input );
}

/**
 * @copybrief make_variant_gapless_input_stream( VariantInputStream const& )
 *
 * This overload additionally sets a genome locus set to filter the positions.
 *
 * See also make_variant_input_stream_from_variant_gapless_input_stream()
 */
inline VariantInputStream make_variant_gapless_input_stream(
    VariantInputStream const& input,
    std::shared_ptr<GenomeLocusSet> genome_locus_set
) {
    auto gapless_input = VariantGaplessInputStream( input );
    gapless_input.genome_locus_set( genome_locus_set );
    return make_variant_input_stream_from_variant_gapless_input_stream( gapless_input );
}

/**
 * @copybrief make_variant_gapless_input_stream( VariantInputStream const& )
 *
 * This overload additionally sets the reference genome for the gapless iteration,
 * as well as a genome locus set to filter the positions.
 *
 * See also make_variant_input_stream_from_variant_gapless_input_stream()
 */
inline VariantInputStream make_variant_gapless_input_stream(
    VariantInputStream const& input,
    std::shared_ptr<::genesis::sequence::ReferenceGenome> ref_genome,
    std::shared_ptr<GenomeLocusSet> genome_locus_set
) {
    auto gapless_input = VariantGaplessInputStream( input );
    gapless_input.reference_genome( ref_genome );
    gapless_input.genome_locus_set( genome_locus_set );
    return make_variant_input_stream_from_variant_gapless_input_stream( gapless_input );
}

/**
 * @copybrief make_variant_gapless_input_stream( VariantInputStream const& )
 *
 * This overload additionally sets the sequence dictionary for the gapless iteration,
 * as well as a genome locus set to filter the positions.
 *
 * See also make_variant_input_stream_from_variant_gapless_input_stream()
 */
inline VariantInputStream make_variant_gapless_input_stream(
    VariantInputStream const& input,
    std::shared_ptr<::genesis::sequence::SequenceDict> seq_dict,
    std::shared_ptr<GenomeLocusSet> genome_locus_set
) {
    auto gapless_input = VariantGaplessInputStream( input );
    gapless_input.sequence_dict( seq_dict );
    gapless_input.genome_locus_set( genome_locus_set );
    return make_variant_input_stream_from_variant_gapless_input_stream( gapless_input );
}

// =================================================================================================
//     Merging Input Stream
// =================================================================================================

/**
 * @brief Create a VariantInputStream that merges samples from its underlying @p input.
 *
 * This provides an on-the-fly merging of input samples by simply summing out their SampleCounts.
 * It takes a mapping of sample names to group names, and creates a VariantInputStream with
 * the group names as new sample names, which then merge the input of their respective samples.
 *
 * If @p allow_ungrouped_samples is set to `true`, any sample that does not occur in the map
 * will be added as-is, with its original sample name, and as its own "group". By default,
 * we throw an exception in this case, in order to make sure that the behavior is intended.
 */
VariantInputStream make_variant_merging_input_stream(
    VariantInputStream const& input,
    std::unordered_map<std::string, std::string> const& sample_name_to_group,
    bool allow_ungrouped_samples = false,
    SampleCountsFilterPolicy filter_policy = SampleCountsFilterPolicy::kOnlyPassing
);

} // namespace population
} // namespace genesis

#endif // include guard
