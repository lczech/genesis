#ifndef GENESIS_POPULATION_FORMATS_SYNC_COMMON_H_
#define GENESIS_POPULATION_FORMATS_SYNC_COMMON_H_

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

#include "genesis/population/formats/sync_reader.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     PoPoolation2 Synchronized File Helper Functions
// =================================================================================================

/**
 * @brief Output a BaseCounts instance to a stream in the PoPoolation2 sync format.
 *
 * This is one column from that file, outputting the counts separated by colons, in the order
 * `A:T:C:G:N:D`, with `D` being deletions (`*` in pileup).
 */
std::ostream& to_sync( BaseCounts const& bs, std::ostream& os );

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

} // namespace population
} // namespace genesis

#endif // include guard
