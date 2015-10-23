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

// =================================================================================================
//     Histogram Operations
// =================================================================================================

void add (Histogram& h, double value)
{
    for (size_t i = 0; i < h.bins(); ++i) {
        h[i] += value;
    }
}

bool add (Histogram& h1, const Histogram& h2)
{
    if (h1.bins() != h2.bins()) {
        return false;
    }

    for (size_t i = 0; i < h1.bins(); ++i) {
        h1[i] += h2[i];
    }

    return true;
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

} // namespace genesis
