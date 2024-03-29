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
 * @ingroup utils
 */

#include "genesis/utils/math/bitvector/operators.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Bitvector Operators
// =================================================================================================

Bitvector bitwise_and( Bitvector const& lhs, Bitvector const& rhs, bool use_larger )
{
    if(( use_larger ) ^ ( lhs.size() < rhs.size() )) {
        auto result = Bitvector( lhs.size(), rhs );
        result &= lhs;
        return result;
    } else {
        auto result = Bitvector( rhs.size(), lhs );
        result &= rhs;
        return result;
    }
}

Bitvector bitwise_or( Bitvector const& lhs, Bitvector const& rhs, bool use_larger )
{
    if(( use_larger ) ^ ( lhs.size() < rhs.size() )) {
        auto result = Bitvector( lhs.size(), rhs );
        result |= lhs;
        return result;
    } else {
        auto result = Bitvector( rhs.size(), lhs );
        result |= rhs;
        return result;
    }
}

Bitvector bitwise_xor( Bitvector const& lhs, Bitvector const& rhs, bool use_larger )
{
    if(( use_larger ) ^ ( lhs.size() < rhs.size() )) {
        auto result = Bitvector( lhs.size(), rhs );
        result ^= lhs;
        return result;
    } else {
        auto result = Bitvector( rhs.size(), lhs );
        result ^= rhs;
        return result;
    }
}

Bitvector set_minus( Bitvector const& lhs, Bitvector const& rhs )
{
    return lhs & (~rhs);
}

Bitvector symmetric_difference( Bitvector const& lhs, Bitvector const& rhs )
{
    return (lhs | rhs) & ~(lhs & rhs);
}

bool is_strict_subset( Bitvector const& sub, Bitvector const& super )
{
    // Not really efficient. We could stop early in the comparison instead.
    // But as of now, we do not need the speed, so let's keep it simple instead.
    // Same for the other variants of this function below.
    return ((sub & super) == sub) && (sub.count() < super.count());
}

bool is_strict_superset( Bitvector const& super, Bitvector const& sub )
{
    return is_strict_subset( sub, super );
}

bool is_subset( Bitvector const& sub, Bitvector const& super )
{
    return (sub == super) || is_strict_subset(sub, super);
}

bool is_superset( Bitvector const& super, Bitvector const& sub )
{
    return (super == sub) || is_strict_superset(super, sub);
}

// bool lexicographically_compare_helper_( Bitvector const& lhs, Bitvector const& rhs, bool on_equal )
// {
//     // Deactivated at the moment, as this does not take care of the typical little endian-ness
//     // of modern computers, and hence yields wrong results...
//
//     // Local helper function to avoid code duplication.
//     if( lhs.size() != rhs.size() ) {
//         throw std::runtime_error(
//             "Cannot use lexicographical comparison functions on Bitvectors of different size."
//         );
//     }
//     for( size_t i = 0; i < lhs.data().size(); ++i ) {
//         if( lhs.data()[i] < rhs.data()[i] ) {
//             return true;
//         } else if( lhs.data()[i] > rhs.data()[i] ) {
//             return false;
//         }
//     }
//
//     // If we are here, all of the above comparisons shows that lhs == rhs.
//     assert( lhs == rhs );
//     return on_equal;
// }
//
// bool is_lexicographically_less( Bitvector const& lhs, Bitvector const& rhs )
// {
//     return lexicographically_compare_helper_( lhs, rhs, false );
// }
//
// bool is_lexicographically_less_or_equal( Bitvector const& lhs, Bitvector const& rhs )
// {
//     return lexicographically_compare_helper_( lhs, rhs, true );
// }
//
// bool is_lexicographically_greater( Bitvector const& lhs, Bitvector const& rhs )
// {
//     return lexicographically_compare_helper_( rhs, lhs, false );
// }
//
// bool is_lexicographically_greater_or_equal( Bitvector const& lhs, Bitvector const& rhs )
// {
//     return lexicographically_compare_helper_( rhs, lhs, true );
// }

std::ostream& operator << (std::ostream& s, Bitvector const& bv)
{
    for( size_t i = 0; i < bv.size(); ++i ) {
        s << (bv.get(i) ? "1" : "0");
    }
    return s;
}

std::istream& operator >> ( std::istream& in, Bitvector& bv )
{
    // We need two steps, as we have to construct the bitvector with a known size.
    // First, bring the bits into string form...
    std::string str;
    auto c = in.peek();
    while( c == '0' || c == '1' ) {
        str += c;
        (void) in.get();
        c = in.peek();
    }

    // ... then, create the bitvector.
    bv = Bitvector( str.size() );
    for( size_t i = 0; i < str.size(); ++i ) {
        if( str[i] == '1' ) {
            bv.set(i);
        }
    }
    return in;
}

} // namespace utils
} // namespace genesis
