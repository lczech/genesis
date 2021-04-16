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
#include <utility>

namespace genesis {
namespace population {

// =================================================================================================
//     Helper Functions
// =================================================================================================

BaseCounts total_base_counts( Variant const& variant );

std::array<std::pair<char, size_t>, 4> sorted_variant_counts(
    Variant const& variant, bool reference_first
);

// =================================================================================================
//     Conversion Functions
// =================================================================================================

Variant convert_to_variant( SimplePileupReader::Record const& record );

#ifdef GENESIS_HTSLIB

Variant convert_to_variant( VcfRecord const& record );

#endif // htslib guard

} // namespace population
} // namespace genesis

#endif // include guard
