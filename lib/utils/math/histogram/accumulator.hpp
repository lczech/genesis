#ifndef GENESIS_UTILS_MATH_HISTOGRAM_ACCUMULATOR_H_
#define GENESIS_UTILS_MATH_HISTOGRAM_ACCUMULATOR_H_

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
 * @brief Header of Histogram Accumulator class.
 *
 * @file
 * @ingroup utils
 */

#include <map>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Histogram;

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
 * `std::nextafter(max, max + 1)` on the max element of the value list. The Histogram uses
 * half-closed ranges like [a, b), so using the next-bigger value ensures that the max element of
 * the Accumulator just fits into the range.
 */
class HistogramAccumulator
{
    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

public:

    typedef std::map<double, double>::const_iterator const_iterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    HistogramAccumulator () {}

    HistogramAccumulator (const std::vector<double>& values, double weight = 1.0);

    HistogramAccumulator (const std::vector<std::pair<double,double>>& weighted_values);

    ~HistogramAccumulator() = default;

    HistogramAccumulator(HistogramAccumulator const&) = default;
    HistogramAccumulator(HistogramAccumulator&&)      = default;

    HistogramAccumulator& operator= (HistogramAccumulator const&) = default;
    HistogramAccumulator& operator= (HistogramAccumulator&&)      = default;

    void swap (HistogramAccumulator& other) noexcept;

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

    Histogram build_uniform_ranges_histogram (
        size_t num_bins,
        bool   integer_ranges = false
    ) const;

    Histogram build_uniform_ranges_histogram (
        size_t num_bins,
        double min,
        double max
    ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::map<double, double> values_;
};

// =================================================================================================
//     Basic Operators
// =================================================================================================

inline void swap( HistogramAccumulator& lhs, HistogramAccumulator& rhs ) noexcept
{
    lhs.swap(rhs);
}

} // namespace utils
} // namespace genesis

#endif // include guard
