#ifndef GENESIS_POPULATION_BASE_COUNTS_H_
#define GENESIS_POPULATION_BASE_COUNTS_H_

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

#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Base Counts
// =================================================================================================

/**
 * @brief One set of nucleotide base counts, for example for a given sample that represents a pool
 * of sequenced individuals.
 *
 * This class is a general representation of the read counts (frequencies) contained in the
 * variants/SNPs of (a pool of) individuals at a certain chromosome position.
 * The class only stores theses counts; for the additional container that also captures the
 * chromosome and position, see Variant.
 */
struct BaseCounts
{
    /**
     * @brief Count of all `A` nucleotides that are present in the sample.
     */
    size_t a_count = 0;

    /**
     * @brief Count of all `C` nucleotides that are present in the sample.
     */
    size_t c_count = 0;

    /**
     * @brief Count of all `G` nucleotides that are present in the sample.
     */
    size_t g_count = 0;

    /**
     * @brief Count of all `T` nucleotides that are present in the sample.
     */
    size_t t_count = 0;

    /**
     * @brief Count of all `N` (undetermined/any) nucleotides that are present in the sample.
     */
    size_t n_count = 0;

    /**
     * @brief Count of all deleted (`*`) nucleotides that are present in the sample.
     */
    size_t d_count = 0;
};

} // namespace population
} // namespace genesis

#endif // include guard
