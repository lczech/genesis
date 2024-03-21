#ifndef GENESIS_POPULATION_FUNCTION_GENOME_REGION_H_
#define GENESIS_POPULATION_FUNCTION_GENOME_REGION_H_

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

#include "genesis/population/genome_locus_set.hpp"
#include "genesis/population/genome_region_list.hpp"
#include "genesis/population/genome_region.hpp"

#ifdef GENESIS_HTSLIB

#include "genesis/population/format/vcf_record.hpp"

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

// =================================================================================================
//     Region Coverage
// =================================================================================================

// -------------------------------------------------------------------------
//     Explicit chr and pos
// -------------------------------------------------------------------------

/**
 * @brief Test whether the chromosome/position is within a given genomic @p region.
 */
bool is_covered( GenomeRegion const& region, std::string const& chromosome, size_t position );

/**
 * @brief Test whether the chromosome/position is within a given list of genomic @p regions.
 */
inline bool is_covered( GenomeRegionList const& regions, std::string const& chromosome, size_t position )
{
    return regions.is_covered( chromosome, position );
}

/**
 * @brief Test whether the chromosome/position is within a given @p GenomeLocusSet.
 */
inline bool is_covered( GenomeLocusSet const& loci, std::string const& chromosome, size_t position )
{
    return loci.is_covered( chromosome, position );
}

// -------------------------------------------------------------------------
//     Templates for structs
// -------------------------------------------------------------------------

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

/**
 * @brief Test whether the chromosome/position of a @p locus is within a given @p GenomeLocusSet.
 *
 * This is a function template, so that it can accept any data structure that contains public
 * member variables `chromosome` (`std::string`) and `position` (`size_t`), such as Variant
 * or GenomeLocus.
 */
template<class T>
bool is_covered( GenomeLocusSet const& loci, T const& locus )
{
    return is_covered( loci, locus.chromosome, locus.position );
}

// -------------------------------------------------------------------------
//     VCF versions
// -------------------------------------------------------------------------

#ifdef GENESIS_HTSLIB

inline bool is_covered( GenomeRegion const& region, VcfRecord const& variant )
{
    return is_covered( region, variant.get_chromosome(), variant.get_position() );
}

inline bool is_covered( GenomeRegionList const& regions, VcfRecord const& variant )
{
    return is_covered( regions, variant.get_chromosome(), variant.get_position() );
}

inline bool is_covered( GenomeLocusSet const& loci, VcfRecord const& variant )
{
    return is_covered( loci, variant.get_chromosome(), variant.get_position() );
}

#endif // htslib guard

} // namespace population
} // namespace genesis

#endif // include guard
