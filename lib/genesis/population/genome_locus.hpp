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

#include <iostream>
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
 * We also define all comparison operators for two @link GenomeLocus Loci@endlink.
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

bool operator ==( GenomeLocus const& a, GenomeLocus const& b )
{
    return a.chromosome == b.chromosome && a.position == b.position;
}

bool operator !=( GenomeLocus const& a, GenomeLocus const& b )
{
    return !(a == b);
}

bool operator <( GenomeLocus const& a, GenomeLocus const& b )
{
    return a.chromosome < b.chromosome || ( a.chromosome == b.chromosome && a.position < b.position );
}

bool operator >( GenomeLocus const& a, GenomeLocus const& b )
{
    return b < a;
}

bool operator <=( GenomeLocus const& a, GenomeLocus const& b )
{
    // We could do the simple default way of implementing this as `a == b || a < b`,
    // but this seems wasteful; in this case, we can do with fewer comparisons!
    return a.chromosome < b.chromosome || ( a.chromosome == b.chromosome && a.position <= b.position );
}

bool operator >=( GenomeLocus const& a, GenomeLocus const& b )
{
    return b <= a;
}

std::ostream& operator<< ( std::ostream& os, GenomeLocus const& l )
{
    os << l.to_string();
    return os;
}

} // namespace population
} // namespace genesis

#endif // include guard
