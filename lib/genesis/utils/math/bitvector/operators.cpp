/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
 * @ingroup utils
 */

#include "genesis/utils/math/bitvector/operators.hpp"

#include <iostream>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Bitvector Operators
// =================================================================================================

Bitvector operator & (Bitvector const& lhs, Bitvector const& rhs)
{
    // make a copy.
    Bitvector result = Bitvector(lhs);

    // check for self-and.
    if (&lhs == &rhs) {
        return result;
    }

    // if not, return and with right hand side.
    result &= rhs;
    return result;
}

Bitvector operator | (Bitvector const& lhs, Bitvector const& rhs)
{
    // make a copy.
    Bitvector result = Bitvector(lhs);

    // check for self-or.
    if (&lhs == &rhs) {
        return result;
    }

    // if not, return or with right hand side.
    result |= rhs;
    return result;
}

Bitvector operator ^ (Bitvector const& lhs, Bitvector const& rhs)
{
    // check for self-xor. if so, return zero vector of same size.
    if (&lhs == &rhs) {
        return Bitvector(lhs.size(), false);
    }

    // otherwise, make a copy and xor it.
    Bitvector result = Bitvector(lhs);
    result ^= rhs;
    return result;
}

Bitvector bitwise_and (Bitvector const& lhs, Bitvector const& rhs)
{
    if( lhs.size() < rhs.size() ) {
        auto result = Bitvector( rhs, lhs.size() );
        result &= lhs;
        return result;
    } else {
        auto result = Bitvector( lhs, rhs.size() );
        result &= rhs;
        return result;
    }
}

Bitvector bitwise_or (Bitvector const& lhs, Bitvector const& rhs)
{
    if( lhs.size() < rhs.size() ) {
        auto result = Bitvector( rhs, lhs.size() );
        result |= lhs;
        return result;
    } else {
        auto result = Bitvector( lhs, rhs.size() );
        result |= rhs;
        return result;
    }
}

Bitvector bitwise_xor (Bitvector const& lhs, Bitvector const& rhs)
{
    if( lhs.size() < rhs.size() ) {
        auto result = Bitvector( rhs, lhs.size() );
        result ^= lhs;
        return result;
    } else {
        auto result = Bitvector( lhs, rhs.size() );
        result ^= rhs;
        return result;
    }
}

Bitvector set_minus (Bitvector const& lhs, Bitvector const& rhs)
{
    return lhs & (~rhs);
}

Bitvector symmetric_difference (Bitvector const& lhs, Bitvector const& rhs)
{
    return (lhs | rhs) & ~(lhs & rhs);
}

bool is_strict_subset( Bitvector const& sub, Bitvector const& super )
{
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

std::ostream& operator << (std::ostream& s, Bitvector const& bv)
{
    for(size_t i = 0; i < bv.size() ; ++i) {
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
