/**
 * @brief Implementation of Histogram class.
 *
 * @file
 * @ingroup utils
 */

#include "utils/math/histogram/histogram.hpp"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iterator>
#include <numeric>

#include "utils/logging.hpp"

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
    bins_                  (num_bins),
    ranges_                (num_bins + 1),
    out_of_range_behaviour (OutOfRangeBehaviour::kIgnore)
{
    // TODO should be an exception.
    assert(num_bins > 0);

    set_uniform_ranges(range_min, range_max);
}

Histogram::Histogram(
    const size_t num_bins,
    const std::vector<double>& values,
    const double weight
) :
    bins_                  (num_bins),
    ranges_                (num_bins + 1),
    out_of_range_behaviour (OutOfRangeBehaviour::kIgnore)
{
    // TODO should be an exception.
    assert(num_bins > 0);

    double min = *std::min_element(values.begin(), values.end());
    double max = *std::max_element(values.begin(), values.end());
    set_uniform_ranges(min, std::nextafter(max, max + 1));

    for (const auto v : values) {
        accumulate(v, weight);
    }
}

Histogram::Histogram(
    const size_t num_bins,
    const std::vector<std::pair<double,double>>& weighted_values
) :
    bins_                  (num_bins),
    ranges_                (num_bins + 1),
    out_of_range_behaviour (OutOfRangeBehaviour::kIgnore)
{
    // TODO should be an exception.
    assert(num_bins > 0);

    auto first_smaller = [] (std::pair<double,double> p1, std::pair<double,double> p2) {
        return p1.first < p2.first;
    };

    auto min_p = *std::min_element(weighted_values.begin(), weighted_values.end(), first_smaller);
    auto max_p = *std::max_element(weighted_values.begin(), weighted_values.end(), first_smaller);

    double min = min_p.first;
    double max = max_p.first;
    set_uniform_ranges(min, std::nextafter(max, max + 1));

    for (const auto pair : weighted_values) {
        accumulate(pair.first, pair.second);
    }
}

Histogram::Histogram(
    const size_t num_bins,
    const double range_min,
    const double range_max,
    const std::vector<double>& values,
    const double weight
) :
    Histogram(
        num_bins,
        range_min,
        range_max
    )
{
    for (const auto v : values) {
        accumulate(v, weight);
    }
}

Histogram::Histogram(
    const size_t num_bins,
    const double range_min,
    const double range_max,
    const std::vector<std::pair<double,double>>& weighted_values
) :
    Histogram(
        num_bins,
        range_min,
        range_max
    )
{
    for (const auto pair : weighted_values) {
        accumulate(pair.first, pair.second);
    }
}

Histogram::Histogram(
    const std::vector<double>& ranges
) :
    bins_                  (ranges.size() - 1, 0.0),
    ranges_                (ranges),
    out_of_range_behaviour (OutOfRangeBehaviour::kIgnore)
{
    // TODO replace by exception. maybe not even neccessary, as constructor might just throw anyway.
    assert(ranges.size() >= 2);

    assert(std::is_sorted(ranges.begin(), ranges.end()));
}

// =================================================================================================
//     Other main methods
// =================================================================================================

void Histogram::set_ranges(const std::vector<double>& ranges)
{
    // TODO replace by throw exception
    assert(ranges.size() == ranges_.size());
    assert(std::is_sorted(ranges.begin(), ranges.end()));
    ranges_ = ranges;
    clear();
}

/**
 * @brief
 */
void Histogram::set_uniform_ranges(const double min, const double max)
{
    if (min >= max) {
        // TODO throw exception
        LOG_WARN << "Histogram range (" << min << ", " << max << ") not valid.";
        return;
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

// =================================================================================================
//     Accessors
// =================================================================================================

Histogram::const_iterator Histogram::begin() const
{
    return bins_.begin();
}

Histogram::const_iterator Histogram::end() const
{
    return bins_.end();
}

double Histogram::operator [] (size_t bin_num) const
{
    return bins_[bin_num];
}

double Histogram::value(size_t bin_num) const
{
    return bins_[bin_num];
}

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
    if (bin < 0 || static_cast <size_t>(bin) >= bins()) {
        switch (out_of_range_behaviour) {
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
                // TODO replace by exception
                assert(false);
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
        // TODO in case genesis switches to using exceptions, this part can simply be ommited.
        // the vector will throw, and the exception will be passt on to the calling funtion anyway.
        LOG_WARN << "Histogram bin " << bin << " not valid.";
        return;
    }

    bins_[bin] += weight;
}

// =================================================================================================
//     Dump
// =================================================================================================

std::string Histogram::dump() const
{
    std::string res;
    for (size_t i = 0; i < bins(); ++i) {
        res += "[" + std::to_string(ranges_[i]) + ", " + std::to_string(ranges_[i + 1]) + "): ";
        res += std::to_string(bins_[i]) + "\n";
    }
    return res;
}

} // namespace genesis
