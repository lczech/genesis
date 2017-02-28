/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
 * @brief Implementation of Histogram Accumulator class.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/histogram/accumulator.hpp"

#include <algorithm>
#include <cmath>

#include "genesis/utils/math/histogram.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

HistogramAccumulator::HistogramAccumulator (
    const std::vector<double>& values,
    double weight
) {
    for (const auto v : values) {
        accumulate(v, weight);
    }
}

HistogramAccumulator::HistogramAccumulator (
    const std::vector<std::pair<double,double>>& weighted_values
) {
    for (const auto pair : weighted_values) {
        accumulate(pair.first, pair.second);
    }
}

void HistogramAccumulator::swap (HistogramAccumulator& other) noexcept
{
    using std::swap;
    swap(values_, other.values_);
}

// =================================================================================================
//     Accessors
// =================================================================================================

HistogramAccumulator::const_iterator HistogramAccumulator::begin() const
{
    return values_.cbegin();
}

HistogramAccumulator::const_iterator HistogramAccumulator::end() const
{
    return values_.cend();
}

// =================================================================================================
//     Modifiers
// =================================================================================================

void HistogramAccumulator::clear()
{
    values_.clear();
    added_values_ = 0;
}

void HistogramAccumulator::increment (double x)
{
    accumulate(x, 1.0);
}

void HistogramAccumulator::accumulate (double x, double weight)
{
    values_[x] += weight;
    ++added_values_;
}

// =================================================================================================
//     Properties
// =================================================================================================

double HistogramAccumulator::min() const
{
    return values_.begin()->first;
}

double HistogramAccumulator::max() const
{
    return values_.rbegin()->first;
}

size_t HistogramAccumulator::size() const
{
    return values_.size();
}

bool HistogramAccumulator::empty() const
{
    return values_.empty();
}

/**
 * @brief Return the number of values that have been added to the Accumulator.
 *
 * This is simply the number of function calls to increment() and accumulate().
 * The count is reset when calling clear().
 */
size_t HistogramAccumulator::added_values() const
{
    return added_values_;
}

// =================================================================================================
//     Factory Methods
// =================================================================================================

Histogram HistogramAccumulator::build_uniform_ranges_histogram(
    size_t num_bins,
    bool   integer_ranges
) const
{
    if (empty()) {
        return Histogram(num_bins);
    }

    auto lower = min();
    auto upper = std::nextafter(max(), max() + 1);

    if (integer_ranges) {
        lower = std::floor(lower);
        upper = std::ceil(upper);
    }

    auto h = Histogram(num_bins, lower, upper);
    for (const auto& pair : values_) {
        h.accumulate(pair.first, pair.second);
    }
    return h;
}

Histogram HistogramAccumulator::build_uniform_ranges_histogram(
    size_t num_bins,
    double min,
    double max
) const
{
    auto h = Histogram(num_bins, min, max);
    for (const auto& pair : values_) {
        h.accumulate(pair.first, pair.second);
    }
    return h;
}

} // namespace utils
} // namespace genesis
