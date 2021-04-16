#ifndef GENESIS_POPULATION_VARIANT_H_
#define GENESIS_POPULATION_VARIANT_H_

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

#include "genesis/population/base_counts.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Variant
// =================================================================================================

/**
 * @brief A single variant at a position in a chromosome, along with BaseCounts for a set of samples.
 *
 * Typically, this class is used as a general representation of the variants/SNPs at a given
 * chromosome position, for example coming from a line in a VCF file or (m)pileup file.
 *
 * The class consists of the following data fields:
 *
 *   * A chromosome and position.
 *   * The reference base and alternative base at that position (which might not always be available
 *     in the input file format, in which case we use 'N'). Note that we currently only support
 *     one alternative base here, that is, biallelic variants only.
 *   * A set of BaseCounts%s, that is, information on the counts of nucleotide bases of thereads
 *     of a set of pool samples or individuals that cover this variants/SNPs at that position.
 *     See BaseCounts for details on the latter.
 */
struct Variant
{
    std::string chromosome;
    size_t      position = 0;
    char        reference_base = 'N';
    char        alternative_base = 'N';

    std::vector<BaseCounts> samples;
};

} // namespace population
} // namespace genesis

#endif // include guard
