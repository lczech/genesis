/**
 * @brief Implementation of Histogram operator functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/math/histogram/operators.hpp"

#include <ostream>

#include "utils/math/histogram.hpp"

namespace genesis {

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

} // namespace genesis
