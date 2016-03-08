#ifndef GENESIS_UTILS_MATH_HISTOGRAM_DISTANCES_H_
#define GENESIS_UTILS_MATH_HISTOGRAM_DISTANCES_H_

/**
 * @brief Header of Histogram distance functions.
 *
 * @file
 * @ingroup utils
 */

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Histogram;

// =================================================================================================
//     Histogram Distances
// =================================================================================================

double earth_movers_distance (const Histogram& h1, const Histogram& h2, bool normalize = true);

} // namespace utils
} // namespace genesis

#endif // include guard
