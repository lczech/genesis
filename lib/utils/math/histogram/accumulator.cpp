/**
 * @brief Implementation of Histogram Accumulator class.
 *
 * @file
 * @ingroup utils
 */

#include "utils/math/histogram/accumulator.hpp"

#include <algorithm>
// #include <assert.h>
// #include <cmath>
// #include <iterator>
// #include <numeric>

namespace genesis {

// =================================================================================================
//     Constructors
// =================================================================================================

HistogramAccumulator::HistogramAccumulator (
    const std::vector<double>& values,
    const double weight
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

// =================================================================================================
//     Modifiers
// =================================================================================================

void HistogramAccumulator::clear()
{
    values_.clear();
}

void HistogramAccumulator::increment (double x)
{
    accumulate(x, 1.0);
}

void HistogramAccumulator::accumulate (double x, double weight)
{
    values_[x] += weight;
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

// =================================================================================================
//     Factory Methods
// =================================================================================================

Histogram HistogramAccumulator::build_uniform_ranges_histogram(const size_t num_bins)
{
    if (empty()) {
        return Histogram(num_bins);
    }

    auto upper = std::nextafter(max(), max() + 1);
    auto h = Histogram(num_bins, min(), upper);

    for (const auto& pair : values_) {
        h.accumulate(pair.first, pair.second);
    }

    return h;
}

Histogram HistogramAccumulator::build_rounded_uniform_ranges_histogram(const size_t num_bins)
{
    if (empty()) {
        return Histogram(num_bins);
    }

    auto upper = std::nextafter(max(), max() + 1);
    auto h = Histogram(num_bins, std::floor(min()), std::ceil(upper));

    for (const auto& pair : values_) {
        h.accumulate(pair.first, pair.second);
    }

    return h;
}

// =================================================================================================
//     Dump
// =================================================================================================

std::string HistogramAccumulator::dump() const
{
    std::string res;
    for (auto& v : values_) {
        res += std::to_string(v.first) + " <- " + std::to_string(v.second) + "\n";
    }
    return res;
}

} // namespace genesis
