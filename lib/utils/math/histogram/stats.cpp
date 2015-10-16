/**
 * @brief Implementation of Histogram statistics functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/math/histogram/stats.hpp"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iterator>
#include <numeric>

namespace genesis {

// =================================================================================================
//     Histogram Statistics
// =================================================================================================

double min_value(const Histogram& h)
{
    return *std::min_element(h.begin(), h.end());
}

double max_value(const Histogram& h)
{
    return *std::max_element(h.begin(), h.end());
}

size_t min_bin(const Histogram& h)
{
    return std::distance(h.begin(), std::min_element(h.begin(), h.end()));
}

size_t max_bin(const Histogram& h)
{
    return std::distance(h.begin(), std::max_element(h.begin(), h.end()));
}

double median(const Histogram& h)
{
    const double mid = sum(h) / 2;
    double part = 0.0;

    size_t i;
    for (i = 0; i < h.bins(); ++i) {
        if (part + h[i] < mid) {
            part += h[i];
        } else {
            break;
        }
    }
    assert(part < mid && i < h.bins());

    // Find the relative position of mid within the interval [part, part + bin[i]).
    // This determines where exactly our median is.
    const double pos = (mid - part) / h[i];

    // Now find this position within the range of the bin and return it.
    return h.bin_range(i).first + pos * h.bin_width(i);
}

double mean(const Histogram& h)
{
    // We are using the recurrence relation
    //   M(n) = M(n-1) + (x[n] - M(n-1)) (w(n)/(W(n-1) + w(n)))
    //   W(n) = W(n-1) + w(n)
    // which is also used in the GNU Scientific Library.

    double wmean  = 0.0;
    double weight = 0.0;

    for (size_t i = 0; i < h.bins(); ++i) {
        const double xi = h.bin_midpoint(i);
        const double wi = h[i];

        if (wi > 0.0) {
            weight += wi;
            wmean += (xi - wmean) * (wi / weight);
        }
    }

    return wmean;
}

double sigma(const Histogram& h)
{
    // We use the same method as in the GNU Scientific Library.
    // It is a two-pass algorithm for stability.
    // Could also use a single pass formula, as given in
    // N. J. Higham: "Accuracy and Stability of Numerical Methods", p.12

    const double wmean = mean(h);
    double weight = 0.0;
    double wvar   = 0.0;

    for (size_t i = 0; i < h.bins(); ++i) {
        const double xi = h.bin_midpoint(i);
        const double wi = h[i];

        if (wi > 0.0) {
            const double delta = (xi - wmean);

            weight += wi;
            wvar   += (delta * delta - wvar) * (wi / weight);
        }
    }

    return std::sqrt(wvar);
}

double sum(const Histogram& h)
{
    return std::accumulate (h.begin(), h.end(), 0.0);
}

} // namespace genesis
