#ifndef GENESIS_POPULATION_FUNCTIONS_VARIANT_H_
#define GENESIS_POPULATION_FUNCTIONS_VARIANT_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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
#include "genesis/population/formats/simple_pileup_reader.hpp"

#ifdef GENESIS_HTSLIB

#include "genesis/population/formats/vcf_record.hpp"

#endif // htslib guard

#include <array>
#include <iosfwd>
#include <utility>

namespace genesis {
namespace population {

// =================================================================================================
//     Helper Functions
// =================================================================================================

/**
 * @brief Get the summed up total base counts of a Variant.
 *
 * This is the same as calling merge() on the samples in the Variant.
 */
BaseCounts total_base_counts( Variant const& variant );

/**
 * @brief Get a list of bases sorted by their counts.
 *
 * If @p reference_first is set to `true`, the first entry in the resulting array is always
 * the reference base of the Variant, while the other three bases are sorted by counts.
 * If @p reference_first is set to `false`, all four bases are sorted by their counts.
 */
SortedBaseCounts sorted_base_counts(
    Variant const& variant, bool reference_first
);

/**
 * @brief Guess the reference base of a Variant.
 *
 * If the Variant already has a `reference_base` in `ACGT`, this base is returned (meaning that
 * this function is idempotent; it does not change the reference base if there already is one).
 * However, if the `reference_base` is `N` or any other value not in `ACGT`,
 * the base with the highest count is returned instead,
 * unless all counts are 0, in which case the returned reference base is `N`.
 */
char guess_reference_base( Variant const& variant );

/**
 * @brief Guess the alternative base of a Variant.
 *
 * If the Variant already has an `alternative_base` in `ACGT` and @p force is not `true`,
 * this original base is returned (meaning that this function is idempotent; it does not change
 * the alternative base if there already is one).
 * However, if the `alternative_base` is `N` or any other char not in `ACGT`, or if @p force is `true`,
 * the base with the highest count that is not the reference base is returned instead.
 * This also means that the reference base has to be set to a value in `ACGT`,
 * as otherwise the concept of an alternative base is meaningless anyway.
 * If the reference base is not one of `ACGT`, the returned alternative base is `N`.
 * Furthermore, if all three non-reference bases have count 0, the returned alternative base is `N`.
 */
char guess_alternative_base( Variant const& variant, bool force = true );

// =================================================================================================
//     Conversion Functions
// =================================================================================================

/**
 * @brief Output a Variant instance to a stream in the PoPoolation2 sync format.
 *
 * The format is a tab-delimited file with one variant per line:
 *
 *   - col1: reference contig
 *   - col2: position within the refernce contig
 *   - col3: reference character
 *   - col4: allele frequencies of population number 1
 *   - col5: allele frequencies of population number 2
 *   - coln: allele frequencies of population number n
 *
 * Each population column outputs counts separated by colons, in the order
 * `A:T:C:G:N:D`, with `D` being deletions (`*` in pileup).
 *
 * See https://sourceforge.net/p/popoolation2/wiki/Tutorial/ for details.
 */
std::ostream& to_sync( Variant const& var, std::ostream& os );

Variant convert_to_variant(
    SimplePileupReader::Record const& record,
    unsigned char min_phred_score = 0
);

#ifdef GENESIS_HTSLIB

/**
 * @brief Convert a VcfRecord to a Variant, treating each sample column as a pool of individuals.
 *
 * This assumes that the data that was used to create the VCF file was actually a pool of
 * individuals (e.g., from pool sequencing) for each sample (column) of the VCF file.
 * We do not actually recommend to use variant calling software on pool-seq data, as it induces
 * frequency shifts due to the statistical models employed by variant calles that were not built
 * for pool sequencing data. It however seems to be a commonly used approach, and hence we offer
 * this function here. For this type of data, the VCF allelic depth ("AD") information contains
 * the counts of the reference and alternative base, which in this context can be interpreted as
 * describing the allele frequencines of each pool of individuals.
 * This requires the VCF to have the "AD" FORMAT field.
 *
 * Only SNP data (no indels) are allowed in this function; use VcfRecord::is_snp() to test this.
 *
 * @see See make_variant_input_iterator_from_pool_vcf_file() for an example where this is used.
 * @see See convert_to_variant_as_individuals() for the function that instead
 * interprets the VCF as usual as a set of individuals.
 */
Variant convert_to_variant_as_pool(
    VcfRecord const& record
);

/**
 * @brief Convert a VcfRecord to a Variant, treating each sample as an individual,
 * and combining them all into one BaseCounts sample.
 *
 * In this function, we assume that the data that was used to create the VCF file was the typical
 * use case of VCF, where each sample (column) in the file corresponds to an individual.
 * When using this function, all samples (individuals) are combined into one, as our targeted
 * output type Variant is used to describe allele counts of several individual (e.g., in a pool).
 * As all columns are combined, the resulting Variant only contains a single BaseCounts object.
 * We only consider biallelic SNP positions here.
 *
 * We offer two ways of combining the samples (columns) of the input VCF record into the BaseCounts:
 *
 *   1. When @p use_allelic_depth is `false` (default), individuals simply contribute to the
 *      BaseCounts according to their polidy. That is, an individual with genotype `A/T` will
 *      contribute one count each for `A` and `T`.
 *   2. When @p use_allelic_depth is `true` instead, we use the "AD" FORMAT field instead, to
 *      obtain the actual counts for the reference and alterantive allele, and use these to
 *      sum up the BaseCounts data.
 *
 * @see See make_variant_input_iterator_from_individual_vcf_file() for an example where this is used.
 * @see See convert_to_variant_as_pool() for the alterantive function that instead
 * interprets each sample (column) as a pool of individuals, e.g., from pool sequencing.
 */
Variant convert_to_variant_as_individuals(
    VcfRecord const& record,
    bool use_allelic_depth = false
);

#endif // htslib guard

} // namespace population
} // namespace genesis

#endif // include guard
