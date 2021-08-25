#ifndef GENESIS_POPULATION_FUNCTIONS_VARIANT_H_
#define GENESIS_POPULATION_FUNCTIONS_VARIANT_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
std::array<std::pair<char, size_t>, 4> sorted_base_counts(
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

Variant convert_to_variant( VcfRecord const& record );

#endif // htslib guard

} // namespace population
} // namespace genesis

#endif // include guard
