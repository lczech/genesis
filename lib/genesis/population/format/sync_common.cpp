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

#include "genesis/population/format/sync_common.hpp"

#include <iostream>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     PoPoolation2 Synchronized File Helper Functions
// =================================================================================================

std::ostream& to_sync( SampleCounts const& bs, std::ostream& os, bool use_status_and_missing )
{
    if( use_status_and_missing && ! bs.status.passing() ) {
        os << ".:.:.:.:.:.";
    } else {
        os << bs.a_count << ":" << bs.t_count << ":" << bs.c_count << ":" << bs.g_count;
        os << ":" << bs.n_count << ":" << bs.d_count;
    }
    return os;
}

std::ostream& to_sync( Variant const& var, std::ostream& os, bool use_status_and_missing )
{
    os << var.chromosome << "\t" << var.position << "\t" << var.reference_base;
    if( use_status_and_missing && ! var.status.passing() ) {
        for( auto const& bs : var.samples ) {
            (void) bs;
            os << "\t";
            os << ".:.:.:.:.:.";
        }
    } else {
        for( auto const& bs : var.samples ) {
            os << "\t";
            to_sync( bs, os, use_status_and_missing );
        }
    }
    os << "\n";
    return os;
}

} // namespace population
} // namespace genesis
