/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
 * @ingroup sequence
 */

#include "genesis/sequence/functions/dict.hpp"

#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <stdexcept>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Sequence Dict
// =================================================================================================

SequenceDict sequence_set_to_dict( SequenceSet const& set )
{
    SequenceDict result;
    for( auto const& seq : set ) {
        result.add( seq.label(), seq.length() );
    }
    return result;
}

bool verify( SequenceDict const& dict, SequenceSet const& set, bool lenient )
{
    if( dict.size() != set.size() ) {
        return false;
    }
    for( size_t i = 0; i < dict.size(); ++i ) {
        if( dict[i].name.empty() || set[i].label().empty() ) {
            return false;
        }
        if( lenient ) {
            auto const s_dct = utils::split( dict[i].name, '\t' );
            auto const s_set = utils::split( set[i].label(), '\t' );
            assert( ! s_dct.empty() );
            assert( ! s_set.empty() );
            if( s_dct[0] != s_set[0] ) {
                return false;
            }
        } else if( dict[i].name != set[i].label() || dict[i].length != set[i].length() ) {
            return false;
        }
    }
    return true;
}

} // namespace sequence
} // namespace genesis
