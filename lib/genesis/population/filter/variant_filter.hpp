#ifndef GENESIS_POPULATION_FILTER_VARIANT_FILTER_H_
#define GENESIS_POPULATION_FILTER_VARIANT_FILTER_H_

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

#include "genesis/population/sample_counts.hpp"
#include "genesis/population/filter/filter_stats.hpp"
#include "genesis/population/filter/filter_status.hpp"
#include "genesis/population/variant.hpp"

#include <iosfwd>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Variant Filter Tag
// =================================================================================================

/**
 * @brief List of filters that we apply to a Variant, to indicate whether the Variant passed or not.
 *
 * This can be used with VariantFilterStatus to indicate the type of filter that did not pass.
 * This includes reasons such as missing data (nothing in the input file for this position),
 * or filtering or masking out regions of the input, so that those can be considered downstream
 * as well. We currently do not use all of them internall - some are meant as generic filter types
 * that can be added on later if additional filters are needed.
 *
 * We typically only want a single filter to fail, and do not consider any more filters once a
 * Variant has been filtered out. That makes it easy to keep track of the reason, and speeds
 * up processing by skipping filter calculations for Variants that won't be considered anyway.
 *
 * The only value for now that we assign a fixed integer value is VariantFilterTag::kPassed,
 * which is `0`, indicating that all filters passed. Other values of this enum are not meant to
 * have a numercial stable value (for now), as we might extend this list later on. Hence, instead
 * compare to the enum directly. However, we do provide a special VariantFilterTag::kEnd enum
 * value, which can be used to iterate the enum values, and facilitate index-based access for
 * counting; see VariantFilterStats for an example.
 */
enum class VariantFilterTag : FilterStatus::IntType
{
    /**
     * @brief Variant has passed all filters.
     */
    kPassed = 0,

    /**
    * @brief Position is missing in the input data.
    */
    kMissing,

    /**
     * @brief Generic indicator that the Variant has not passed a filter.
     *
     * Not used at the moment internally, but included here as a simple catch-all value if no
     * further distrinction for the filter that failed is needed.
     */
    kNotPassed,

    /**
     * @brief Generic indicator that the Variant is invalid.
     *
     * Not used at the moment internally. Similar to kNotPassed, this is a generic value for
     * invalid Variants.
     */
    kInvalid,

    // -------------------------------------------
    //     Position
    // -------------------------------------------

    /**
     * @brief Position has been masked out from processing.
     *
     * This can be due to, e.g., via a RegionLocus set from a fasta file, see read_mask_fasta().
     * We distinguish this from kMaskedRegion purely for semantic reasons. Both filters are due to
     * some user-specified position-based filter, and created by similar functions. However, we
     * generally mean to indicate that a masked position is due to some fine-grained positional
     * filter, while masked regions are meant to indicate filters for larger regions such as
     * chromsosomes or genes.
     */
    kMaskedPosition,

    /**
     * @brief Position is part of a masked region.
     *
     * See kMaskedPosition for details on the distrinction between the two.
     */
    kMaskedRegion,

    // -------------------------------------------
    //     Sample
    // -------------------------------------------

    /**
     * @brief None of the SampleCounts of the Variant passed their filters.
     *
     * If all samples of the Variant did not pass their respecive filters, this value can be
     * set to skip the whole Variant.
     */
    kNoSamplePassed,

    /**
     * @brief Some of the SampleCounts of the Variant did not pass their filters.
     *
     * For algorithms that need every sample to be passig their filters, this can be used to
     * indicate that some samples did not pass, and that this Variant hence also needs to be skipped.
     * In many algorithms though this is not needed - if at least some samples are valid, we
     * continue with the Variant and process it as far as possible.
     */
    kNotAllSamplesPassed,

    // -------------------------------------------
    //     Numeric
    // -------------------------------------------

    /**
     * @brief All counts across all samples are zero.
     *
     * This corresponds to a zero coverage position that is however not missing in the data.
     */
    kEmpty,

    /**
     * @brief Sum of counts across all samples is below the min coverage threshold.
     *
     * @see VariantFilterNumericalParams::min_coverage
     */
    kBelowMinCoverage,

    /**
     * @brief Sum of counts across all samples is above the max coverage threshold.
     *
     * @see VariantFilterNumericalParams::max_coverage
     */
    kAboveMaxCoverage,

    /**
     * @brief Too many deletions at the position.
     *
     * @see VariantFilterNumericalParams::deletions_count_limit
     */
    kAboveDeletionsCountLimit,

    // -------------------------------------------
    //     SNP vs Invariant
    // -------------------------------------------

    /**
     * @brief Invariant position, not a SNP.
     *
     * This is a generic filter type if a position is invariant, for instance used when
     * no min or max counts are given. Whenever those are given though, we use one of the
     * more specific types below that indicate why exactly the position is not a SNP.
     *
     * @see VariantFilterNumericalParams::only_snps
     */
    kNotSnp,

    /**
     * @brief SNP position, but not biallelic, i.e., has more than one alternative.
     *
     * This counts how many Variants were SNPs but not biallelic. It hence indicates how many
     * Variants were filtered out because of the only_biallelic_snps filter setting.
     *
     * @see VariantFilterNumericalParams::only_biallelic_snps
     */
    kNotBiallelicSnp,

    /**
     * @brief Sum of nucleotides is below VariantFilterNumericalParams::snp_min_count.
     *
     * That is, the variant would have counted as a SNP if the snp_min_count setting wasn't used.
     * This is hence useful to see how many Variants were filtered out because of that setting.
     *
     * Note though that we do not make a distinction between biallelic and multialleleic SNPs here
     * any more for simplicity. This counts any position that was filtered out for not being a
     * SNP according to the only_snps and/or only_biallelic_snps after considering snp_min_count.
     *
     * @see VariantFilterNumericalParams::snp_min_count
     */
    kBelowSnpMinCount,

    /**
     * @brief Sum of nucleotides is above VariantFilterNumericalParams::snp_max_count.
     *
     * Same as kBelowSnpMinCount, but for the snp_max_count setting instead.
     *
     * @see VariantFilterNumericalParams::snp_max_count
     */
    kAboveSnpMaxCount,

    /**
     * @brief Did not reach minimum allele frequency.
     *
     * This flags positions that are not a SNP according to the  minimum allele frequency did not have an allele frequency `af`
     *
     * See VariantFilterNumericalParams::snp_min_allele_frequency
     */
    kBelowMinAlleleFreq,

    // -------------------------------------------
    //     Special
    // -------------------------------------------

    /**
     * @brief End of the enum values.
     *
     * This value is solely provided as a stable name for referencing when iterating the values
     * in the enum. See VariantFilterStats for an example of where this is used.
     *
     * Note: For that reason, this value has to be remain the last in the enum.
     */
    kEnd
};

// =================================================================================================
//     Variant Filter Stats
// =================================================================================================

/**
 * @brief Counts of how many Variant%s with each VariantFilterTag occured in some data.
 */
using VariantFilterStats = FilterStats<VariantFilterTag>;

// =================================================================================================
//     Filter Functions
// =================================================================================================

// --------------------------------------------------------------------------------------
//     Print stats
// --------------------------------------------------------------------------------------

/**
 * @brief Print a textual representation of the counts collected.
 */
std::ostream& print_variant_filter_stats(
    std::ostream& os,
    VariantFilterStats const& stats,
    bool verbose = false
);

/**
 * @brief Print a textual representation of the counts collected.
 */
std::string print_variant_filter_stats(
    VariantFilterStats const& stats,
    bool verbose = false
);

} // namespace population
} // namespace genesis

#endif // include guard
