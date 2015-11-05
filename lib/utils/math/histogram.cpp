/**
 * @brief Implementation of Histogram class.
 *
 * @file
 * @ingroup utils
 */

#include "utils/math/histogram.hpp"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iterator>
#include <numeric>
#include <stdexcept>

namespace genesis {

// =================================================================================================
//     Constructors
// =================================================================================================

Histogram::Histogram(
    const size_t num_bins
) :
    Histogram(num_bins, 0.0, 1.0)
{
    // Nothing to do here.
}

Histogram::Histogram(
    const size_t num_bins,
    const double range_min,
    const double range_max
) :
    bins_                   (num_bins),
    ranges_                 (num_bins + 1),
    out_of_range_behaviour_ (OutOfRangeBehaviour::kIgnore)
{
    if (num_bins == 0) {
        throw std::domain_error("__FUNCTION__: domain_error");
    }

    set_uniform_ranges(range_min, range_max);
}

Histogram::Histogram(
    const std::vector<double>& ranges
) :
    bins_                   (ranges.size() - 1, 0.0),
    ranges_                 (ranges),
    out_of_range_behaviour_ (OutOfRangeBehaviour::kIgnore)
{
    if (ranges.size() < 2) {
        throw std::domain_error("__FUNCTION__: domain_error");
    }
    if (!std::is_sorted(ranges.begin(), ranges.end())) {
        throw std::invalid_argument("__FUNCTION__: invalid_argument");
    }
}

// =================================================================================================
//     General Methods
// =================================================================================================

void Histogram::set_ranges(const std::vector<double>& ranges)
{
    if (ranges.size() != ranges_.size() || !std::is_sorted(ranges.begin(), ranges.end())) {
        throw std::invalid_argument("__FUNCTION__: invalid_argument");
    }

    ranges_ = ranges;
    clear();
}

/**
 * @brief
 */
void Histogram::set_uniform_ranges(const double min, const double max)
{
    if (min >= max) {
        throw std::invalid_argument("__FUNCTION__: invalid_argument");
    }

    // const size_t num_bins  = bins();
    // const double bin_width = (max - min) / num_bins;
    //
    // for (size_t i = 0; i < num_bins + 1; ++i) {
    //     ranges_[i] = min + static_cast<double>(i) * bin_width;
    // }

    // More stable version from the GNU Scientific Library.
    const double n = static_cast<double>(bins());
    for (size_t i = 0; i < bins() + 1; ++i) {
        const double p = static_cast<double>(i);

        const double f1 = (n - p) / n;
        const double f2 = p / n;

        ranges_[i] = f1 * min +  f2 * max;
    }

    clear();
}

/**
 * @brief
 */
void Histogram::clear()
{
    for (auto& b : bins_) {
        b = 0.0;
    }
}

Histogram::OutOfRangeBehaviour Histogram::out_of_range_behaviour() const
{
    return out_of_range_behaviour_;
}

void Histogram::out_of_range_behaviour(OutOfRangeBehaviour v)
{
    out_of_range_behaviour_ = v;
}

// =================================================================================================
//     Bin Access
// =================================================================================================

double& Histogram::at (size_t bin_num)
{
    if (bin_num >= bins_.size()) {
        throw std::out_of_range("__FUNCTION__: out_of_range");
    }
    return bins_.at(bin_num);
}

double Histogram::at (size_t bin_num) const
{
    if (bin_num >= bins_.size()) {
        throw std::out_of_range("__FUNCTION__: out_of_range");
    }
    return bins_.at(bin_num);
}

double& Histogram::operator [] (size_t bin_num)
{
    return bins_[bin_num];
}

double Histogram::operator [] (size_t bin_num) const
{
    return bins_[bin_num];
}

// =================================================================================================
//     Bin Iterators
// =================================================================================================

Histogram::iterator Histogram::begin()
{
    return bins_.begin();
}

Histogram::iterator Histogram::end()
{
    return bins_.end();
}

Histogram::const_iterator Histogram::begin() const
{
    return bins_.cbegin();
}

Histogram::const_iterator Histogram::end() const
{
    return bins_.cend();
}

Histogram::const_iterator Histogram::cbegin() const
{
    return bins_.cbegin();
}

Histogram::const_iterator Histogram::cend() const
{
    return bins_.cend();
}

// =================================================================================================
//     Properties
// =================================================================================================

double Histogram::min() const
{
    return ranges_.front();
}

double Histogram::max() const
{
    return ranges_.back();
}

size_t Histogram::bins() const
{
    return bins_.size();
}

std::pair<double, double> Histogram::bin_range(size_t bin_num) const
{
    return std::make_pair(ranges_[bin_num], ranges_[bin_num + 1]);
}

double Histogram::bin_midpoint(size_t bin_num) const
{
    return (ranges_[bin_num] + ranges_[bin_num + 1]) / 2;
}

double Histogram::bin_width(size_t bin_num) const
{
    return ranges_[bin_num + 1] - ranges_[bin_num];
}

int Histogram::find_bin (double x) const
{
    if (x < min()) {
        return -1;
    }
    if (x >= max()) {
        return bins();
    }

    // Get the bin for the uniform ranges case.
    double bin_width = (max() - min()) / static_cast<double>(bins());
    int bin = static_cast <int> (std::floor( (x - min()) / bin_width ));

    // If this worked, simply return the bin number.
    if (ranges_[bin] <= x && x < ranges_[bin + 1]) {
        return bin;
    }

    // If not, do a binary search.
    auto it = std::upper_bound(ranges_.begin(), ranges_.end(), x);
    return std::distance(ranges_.begin(), it) - 1;
}

bool Histogram::check_range(double x) const
{
    return x >= min() && x < max();
}

// =================================================================================================
//     Modifiers
// =================================================================================================

int Histogram::increment (double x)
{
    return accumulate(x, 1.0);
}

int Histogram::accumulate (double x, double weight)
{
    int bin = find_bin(x);

    // Do boundary check on the bin.
    // The cast is only done if we already know that bin >= 0, so it is always valid.
    if (bin < 0 || static_cast <size_t>(bin) >= bins()) {
        switch (out_of_range_behaviour()) {
            case OutOfRangeBehaviour::kIgnore:
                return bin;

            case OutOfRangeBehaviour::kSqueeze:
                if (bin < 0) {
                    bin = 0;
                } else {
                    bin = bins() - 1;
                }
                break;

            case OutOfRangeBehaviour::kThrow:
                throw std::out_of_range("__FUNCTION__: out_of_range");
        }
    }

    // Set the new bin value and return its index.
    bins_[bin] += weight;
    return bin;
}

void Histogram::increment_bin (size_t bin)
{
    accumulate_bin(bin, 1.0);
}

void Histogram::accumulate_bin (size_t bin, double weight)
{
    if (bin >= bins()) {
        throw std::out_of_range("__FUNCTION__: out_of_range");
    }

    bins_[bin] += weight;
}

} // namespace genesis
