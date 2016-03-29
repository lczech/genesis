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
 * @brief Implementation of Histogram operator functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/math/histogram/operators.hpp"

#include <ostream>

#include "utils/math/histogram.hpp"
#include "utils/math/histogram/accumulator.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Histogram Operators
// =================================================================================================

std::ostream& operator<< (std::ostream& os, const Histogram& h)
{
    for (size_t i = 0; i < h.bins(); ++i) {
        auto range = h.bin_range(i);
        os << "[" << range.first << ", " << range.second << "): ";
        os << h[i] << "\n";
    }
    return os;
}

// =================================================================================================
//     Histogram Accumulator Operators
// =================================================================================================

std::ostream& operator<< (std::ostream& os, const HistogramAccumulator& h)
{
    for (auto& v : h) {
        os << v.first << " <- " << v.second << "\n";
    }
    return os;
}

} // namespace utils
} // namespace genesis
