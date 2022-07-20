#ifndef GENESIS_POPULATION_FUNCTIONS_GENOME_REGION_H_
#define GENESIS_POPULATION_FUNCTIONS_GENOME_REGION_H_

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

#include "genesis/population/genome_region.hpp"
#include "genesis/population/genome_region_list.hpp"

#ifdef GENESIS_HTSLIB

#include "genesis/population/formats/vcf_record.hpp"

#endif // htslib guard

#include <iosfwd>
#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Comparion Operators
// =================================================================================================

/**
 * @brief Equality comparison (`!=`) for two GenomeRegion%s.
 */
bool operator ==( GenomeRegion const& a, GenomeRegion const& b );

/**
 * @brief Inequality comparison (`!=`) for two GenomeRegion%s.
 */
bool operator !=( GenomeRegion const& a, GenomeRegion const& b );

// =================================================================================================
//     Parsing & Printing
// =================================================================================================

std::string to_string( GenomeRegion const& region );
std::ostream& operator<<( std::ostream& os, GenomeRegion const& region );

/**
 * @brief Parse a genomic region.
 *
 * Accepted formats are "chromosome", "chromosome:position", "chromosome:start-end", and
 * "chromosome:start..end".
 *
 * By default, we expect positions (coordindates) to be 1-based amd  inclusive (closed interval),
 * but this can be changed with the additional parameters @p zero_based and @p end_exclusive.
 */
GenomeRegion parse_genome_region(
    std::string const& region,
    bool zero_based = false,
    bool end_exclusive = false
);

/**
 * @brief Parse a set/list of genomic regions.
 *
 * The individual regions need to be separated by commas (surrounding white space is okay),
 * and each region needs to follow the format as explained in parse_genome_region().
 * See there for details.
 */
GenomeRegionList parse_genome_regions(
    std::string const& regions,
    bool zero_based = false,
    bool end_exclusive = false
);

/**
 * @brief Parse a file with one position or region per line, in different formats.
 *
 * The function expects an input file, and tries to interpret each line as a position or region
 * in a chromosome, offering a variety of formats:
 *
 *   * "chr" for whole chromosomes,
 *   * "chr:position", "chr:start-end", "chr:start..end" for positions and regions,
 *   * tab- or space-delimited "chr position" or "chr start end" as well for positions and regions.
 *
 * This allows for maximum flexibility when reading in such files. Note that this is more flexible
 * than parse_genome_region(), which does not support the tab- and space-delimiation - when parsing
 * an individual string, that does not make much sense, hence we only offer this delimitation
 * for files as of now.
 */
void parse_genome_region_file(
    std::string const& filename,
    GenomeRegionList& target,
    bool overlap = false,
    bool zero_based = false,
    bool end_exclusive = false
);

// =================================================================================================
//     Region Coverage
// =================================================================================================

/**
 * @brief Test whether the chromosome/position is within a given genomic @p region.
 */
bool is_covered( GenomeRegion const& region, std::string const& chromosome, size_t position );

/**
 * @brief Test whether the chromosome/position is within a given list of genomic @p regions.
 */
bool is_covered( GenomeRegionList const& regions, std::string const& chromosome, size_t position );

/**
 * @brief Test whether the chromosome/position of a @p locus is within a given genomic @p region.
 *
 * This is a function template, so that it can accept any data structure that contains public
 * member variables `chromosome` (`std::string`) and `position` (`size_t`), such as Variant
 * or GenomeLocus.
 */
template<class T>
bool is_covered( GenomeRegion const& region, T const& locus )
{
    return is_covered( region, locus.chromosome, locus.position );
}

/**
 * @brief Test whether the chromosome/position of a @p locus is within a given list of
 * genomic @p regions.
 *
 * This is a function template, so that it can accept any data structure that contains public
 * member variables `chromosome` (`std::string`) and `position` (`size_t`), such as Variant
 * or GenomeLocus.
 */
template<class T>
bool is_covered( GenomeRegionList const& regions, T const& locus )
{
    return is_covered( regions, locus.chromosome, locus.position );
}

#ifdef GENESIS_HTSLIB

bool is_covered( GenomeRegion const& region, VcfRecord const& variant );
bool is_covered( GenomeRegionList const& regions, VcfRecord const& variant );

#endif // htslib guard

} // namespace population
} // namespace genesis

#endif // include guard
