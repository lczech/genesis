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
 * @brief Implementation of Histogram operations functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/math/histogram/operations.hpp"

#include <assert.h>
#include <cmath>
#include <vector>

#include "utils/math/histogram.hpp"
#include "utils/math/histogram/stats.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Histogram Operations
// =================================================================================================

void offset (Histogram& h, double value)
{
    for (size_t i = 0; i < h.bins(); ++i) {
        h[i] += value;
    }
}

void scale (Histogram& h, double factor)
{
    for (size_t i = 0; i < h.bins(); ++i) {
        h[i] *= factor;
    }
}

void normalize (Histogram& h, double total)
{
    scale(h, total / sum(h));
}

} // namespace utils
} // namespace genesis
