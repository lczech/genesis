#ifndef GENESIS_POPULATION_GENOME_LOCUS_H_
#define GENESIS_POPULATION_GENOME_LOCUS_H_

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

namespace genesis {
namespace population {

// =================================================================================================
//     Genome Locus
// =================================================================================================

/**
 * @brief A single locus, that is, a position on a chromosome.
 *
 * For our purposes here, we define a locus to be one position on one chromosome exactly.
 *
 * When working with multiple loci, we here often expect their chromosome names to be sorted in
 * lexicographical order. Hence, for example, when comparing two loci on different chromosomes,
 * we also take the ordering of their chromosome names into account.
 */
struct GenomeLocus
{
    std::string chromosome;
    size_t      position = 0;

    GenomeLocus() = default;
    GenomeLocus( std::string const& chr, size_t pos )
        : chromosome( chr )
        , position( pos )
    {}

    ~GenomeLocus() = default;

    GenomeLocus( GenomeLocus const& ) = default;
    GenomeLocus( GenomeLocus&& )      = default;

    GenomeLocus& operator= ( GenomeLocus const& ) = default;
    GenomeLocus& operator= ( GenomeLocus&& )      = default;

    bool empty() const
    {
        return chromosome == "" && position == 0;
    }

    operator std::string() const
    {
        return to_string();
    }

    std::string to_string() const
    {
        return chromosome + ":" + std::to_string( position );
    }
};

} // namespace population
} // namespace genesis

#endif // include guard
