/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "utils/math/twobit_vector/functions.hpp"

#include <algorithm>
#include <assert.h>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Strings
// =================================================================================================

TwobitVector::CharType translate_from_nucleic_acid( char site )
{
    switch( site ) {
        case 'a':
        case 'A':
            return 0u;

        case 'c':
        case 'C':
            return 1u;

        case 'g':
        case 'G':
            return 2u;

        case 't':
        case 'T':
            return 3u;

        default:
            throw std::runtime_error( "Invalid nucleic acid." );
    }
}

char translate_to_nucleic_acid( TwobitVector::CharType value )
{
    switch( value ) {
        case 0u: return 'A';
        case 1u: return 'C';
        case 2u: return 'G';
        case 3u: return 'T';
        default:
            throw std::runtime_error( "Invalid twobit value." );
    }
}

TwobitVector from_nucleic_acids( std::string const& sequence )
{
    auto result = TwobitVector( sequence.size() );
    for( size_t i = 0; i < sequence.size(); ++i ) {
        result.set( i, translate_from_nucleic_acid( sequence[i] ));
    }
    return result;
}

std::string to_nucleic_acids( TwobitVector const& vec )
{
    std::string result;
    result.reserve( vec.size() );

    for( size_t i = 0; i < vec.size(); ++i ) {
        result += translate_to_nucleic_acid( vec[i] );
    }
    return result;
}

std::string twobit_to_bitstring( TwobitVector const& vec )
{
    std::string res = "";
    for( size_t i = 0; i < vec.data_size(); ++i ) {
        res += twobit_to_bitstring( vec.data_at( i )) + "\n";
    }
    return res;
}

std::string twobit_to_bitstring( TwobitVector::WordType const& vec )
{
    auto cpy = vec;
    std::string res = "";

    for (size_t i = 0; i < TwobitVector::CharsPerWord; ++i) {
        auto tmp = cpy & 3;
        switch( tmp ) {
            case 0:
                res += "00";
                break;

            case 1:
                res += "10";
                break;

            case 2:
                res += "01";
                break;

            case 3:
                res += "11";
                break;

            default:
                assert( false );
        }
        if( i < TwobitVector::CharsPerWord - 1 ) {
            res += " ";
        }
        cpy >>= 2;
    }

    std::reverse( res.begin(), res.end() );
    return res;
}

} // namespace utils
} // namespace genesis
