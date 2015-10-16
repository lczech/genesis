/**
 * @brief Implementation of Histogram distance functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/math/histogram/distances.hpp"

#include <assert.h>
#include <cmath>
#include <vector>

namespace genesis {

// =================================================================================================
//     Histogram Distances
// =================================================================================================

double earth_movers_distance (const Histogram& h1, const Histogram& h2)
{
    // We are calcuating the EMD linarly:
    // EMD[0]   = 0
    // EMD[i+1] = ( h1[i] + EMD[i] ) - h2[i]
    // result   = SUM (| EMD[i] |)

    // TODO replace by exception
    assert(h1.bins() == h2.bins());

    std::vector<double> emd (h1.bins(), 0.0);
    double result = 0.0;
    double delta;

    for (size_t i = 0; i < h1.bins() - 1; ++i) {
        emd[i + 1]  = h1[i] + emd[i] - h2[i];
        delta       = h1.bin_midpoint(i + 1) - h1.bin_midpoint(i);
        result     += std::abs(emd[i + 1]) * delta;
    }

    return result;
}

} // namespace genesis
