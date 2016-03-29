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
 * @brief Implementation of Histogram distance functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/math/histogram/distances.hpp"

#include <assert.h>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "utils/math/histogram.hpp"
#include "utils/math/histogram/operations.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Histogram Distances
// =================================================================================================

double earth_movers_distance_simple (const Histogram& h1, const Histogram& h2)
{
    // We are calcuating the EMD linarly:
    //
    //     EMD[0]   = 0
    //     EMD[i+1] = ( h1[i] + EMD[i] ) - h2[i]
    //     result   = SUM (| EMD[i] |)
    //
    // It would probably suffice to keep only the last two values of the emd array,
    // but for now this approach is easier to test.

    // There are approaches like bin-mapping that also allow emd on histograms with different
    // numbers of bins (but same min and max value). There might even be something that can compare
    // all kinds of histograms with each other. So far, we do not need those, so we expect our
    // histograms to have equal ranges.
    if (!equal_ranges(h1, h2)) {
        throw std::range_error("__FUNCTION__: Histograms do not have equal ranges.");
    }

    std::vector<double> emd (h1.bins(), 0.0);
    double result = 0.0;
    double delta;

    for (size_t i = 0; i < h1.bins() - 1; ++i) {
        emd[i + 1]  = h1[i] + emd[i] - h2[i];
        delta       = h1.bin_midpoint(i + 1) - h1.bin_midpoint(i);

        // If delta is < 0, our Histograms have non-sorted ranges. Should not happen.
        assert(delta >= 0);

        result += std::abs(emd[i + 1]) * delta;
    }

    return result;
}

double earth_movers_distance (const Histogram& h1, const Histogram& h2, bool norm)
{
    if (norm) {
        auto hn1 = h1;
        auto hn2 = h2;

        normalize(hn1);
        normalize(hn2);

        return earth_movers_distance_simple (hn1, hn2);
    } else {
        return earth_movers_distance_simple (h1, h2);
    }
}

} // namespace utils
} // namespace genesis
