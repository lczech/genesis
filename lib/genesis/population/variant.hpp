#ifndef GENESIS_POPULATION_VARIANT_H_
#define GENESIS_POPULATION_VARIANT_H_

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
#include "genesis/population/filter/filter_status.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Variant
// =================================================================================================

/**
 * @brief A single variant at a position in a chromosome, along with SampleCounts for a set of samples.
 *
 * Typically, this class is used as a general representation of the variants/SNPs at a given
 * chromosome position, for example coming from a line in a VCF file or (m)pileup file.
 *
 * The class consists of the following data fields:
 *
 *   * A chromosome and position, 1-based.
 *   * The reference base and alternative base at that position (which might not always be available
 *     in the input file format, in which case we use 'N'). Note that we currently only support
 *     one alternative base here, that is, biallelic variants only.
 *   * A set of SampleCounts%s, that is, information on the counts of nucleotide bases of thereads
 *     of a set of pool samples or individuals that cover this variants/SNPs at that position.
 *     See SampleCounts for details on the latter.
 *   * A filter status to indicate whether this Variant is to be considered in analyses or not.
 *     See FilterStatus for details.
 */
struct Variant
{
    // Internal memory layout: The status is 4 bytes, which we put next to the two one-byte members,
    // so that we do not get extra alignment padding here, to keep the data structure small.
    // Unfortunately, in release buiild, we are still a bit bigger than a cache line of 64 byte,
    // but short of making position a 32 bit number, there is nothing we can do about that.
    // Not sure if that would improve performance in our case anyway. Could be tested.

    std::string chromosome;
    size_t      position = 0;

    FilterStatus status;
    static_assert( sizeof(FilterStatus) == 4, "sizeof(FilterStatus) != 4" );

    char        reference_base   = 'N';
    char        alternative_base = 'N';

    std::vector<SampleCounts> samples;
};

} // namespace population
} // namespace genesis

#endif // include guard
