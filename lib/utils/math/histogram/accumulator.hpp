#ifndef GENESIS_UTILS_MATH_HISTOGRAM_ACCUMULATOR_H_
#define GENESIS_UTILS_MATH_HISTOGRAM_ACCUMULATOR_H_

/**
 * @brief Header of Histogram Accumulator class.
 *
 * @file
 * @ingroup utils
 */

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "utils/math/histogram.hpp"

namespace genesis {

// =================================================================================================
//     Histogram Accumulator
// =================================================================================================

/**
 * @brief Histogram helper class that collects data and builds a fitting Histogram.
 *
 * This class can be used to collect data and automatically build a Histogram that fits to the data.
 * It provides factory methods for differnt ways of initializing the Histogram ranges.
 *
 * The max value used for the build Histogram will be determined using
 * `std::nextafter(max, max + 1)` on the max element of the value list. This is because we want this
 * element to just fit into the range.
 */
class HistogramAccumulator
{
    // -------------------------------------------------------------------------
    //     Constructors and Typedefs
    // -------------------------------------------------------------------------

public:

    HistogramAccumulator () {}

    HistogramAccumulator (const std::vector<double>& values, const double weight = 1.0);

    HistogramAccumulator (const std::vector<std::pair<double,double>>& weighted_values);

    typedef std::map<double, double>::const_iterator const_iterator;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    const_iterator begin() const;

    const_iterator end() const;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    void clear();

    void increment (double x);

    void accumulate (double x, double weight);

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    double min() const;

    double max() const;

    size_t size() const;

    bool empty() const;

    // -------------------------------------------------------------------------
    //     Factory Methods
    // -------------------------------------------------------------------------

    Histogram build_uniform_ranges_histogram (const size_t num_bins);

    Histogram build_rounded_uniform_ranges_histogram (const size_t num_bins);

    // -------------------------------------------------------------------------
    //     Dump
    // -------------------------------------------------------------------------

    std::string dump() const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::map<double, double> values_;
};

} // namespace genesis

#endif // include guard
