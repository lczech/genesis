#ifndef GENESIS_POPULATION_FORMAT_SYNC_COMMON_H_
#define GENESIS_POPULATION_FORMAT_SYNC_COMMON_H_

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/format/sync_reader.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     PoPoolation2 Synchronized File Helper Functions
// =================================================================================================

/**
 * @brief Output a SampleCounts instance to a stream in the PoPoolation2 sync format.
 *
 * This is one column from that file, outputting the counts separated by colons, in the order
 * `A:T:C:G:N:D`, with `D` being deletions (`*` in pileup).
 *
 * If @p use_status_and_missing is set to `true` (default), any sample for which the
 * SampleCounts::status is not passing (any status value other than 0) is considered to be filtered
 * out. Instead of writing the counts, we then instead use the "missing" or "masked" extension of
 * the sync file format to denote this, which is `.:.:.:.:.:.` instead of the actual counts.
 *
 */
std::ostream& to_sync( SampleCounts const& bs, std::ostream& os, bool use_status_and_missing = true );

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
 *
 * If @p use_status_and_missing is set to `true` (default), any variant for which the Variant::status
 * is not passing (any status value other than 0) is considered to be filtered out. Instead of
 * writing the counts, we then instead use the "missing" or "masked" extension of the sync file
 * format to denote this, which is `.:.:.:.:.:.` instead of the actual counts. This is first applied
 * to the status of the Variant, in which case all samples are effected. It then is also propagated
 * to the SampleCounts themselves, and their status is checked, with the same effect, but per sample.
 * This allows to granuarly decide whether the whole Variant is filtered, or only individual samples.
 */
std::ostream& to_sync( Variant const& var, std::ostream& os, bool use_status_and_missing = true );

} // namespace population
} // namespace genesis

#endif // include guard
