#ifndef GENESIS_UTILS_MATH_TWOBIT_VECTOR_FUNCTIONS_H_
#define GENESIS_UTILS_MATH_TWOBIT_VECTOR_FUNCTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/utils/math/twobit_vector.hpp"

#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Strings
// =================================================================================================

TwobitVector::ValueType translate_from_nucleic_acid( char site );
char                    translate_to_nucleic_acid( TwobitVector::ValueType value );

TwobitVector from_nucleic_acids( std::string const& sequence );
std::string to_nucleic_acids( TwobitVector const& vec );

std::string bitstring( TwobitVector const& vec );
std::string bitstring( TwobitVector::WordType const& vec );

} // namespace utils
} // namespace genesis

#endif // include guard
