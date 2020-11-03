#ifndef GENESIS_POPULATION_FUNCTIONS_VARIANT_H_
#define GENESIS_POPULATION_FUNCTIONS_VARIANT_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
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

namespace genesis {
namespace population {

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
