#ifndef GENESIS_POPULATION_FORMATS_SIMPLE_PILEUP_COMMON_H_
#define GENESIS_POPULATION_FORMATS_SIMPLE_PILEUP_COMMON_H_

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

#include "genesis/population/formats/simple_pileup_reader.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Simple (m)pileup Reader Helper Functions
// =================================================================================================

BaseCounts convert_to_base_counts(
    SimplePileupReader::Sample const& sample,
    unsigned char min_phred_score = 0
);

Variant convert_to_variant(
    SimplePileupReader::Record const& record,
    unsigned char min_phred_score = 0
);

} // namespace population
} // namespace genesis

#endif // include guard
