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

#include "utils/logging.hpp"

namespace genesis {

// =================================================================================================
//     Constructor and Destructor
// =================================================================================================

/**
 * @brief Constructor.
 */
Histogram::Histogram(const size_t num_bins) :
    Histogram(num_bins, 0.0, 1.0)
{
    // Nothing to do here.
}

/**
 * @brief Constructor.
 */
Histogram::Histogram(const size_t num_bins, const double range_min, const double range_max) :
    bins_                  (num_bins),
    ranges_                (num_bins + 1),
    out_of_range_behaviour (OutOfRangeBehaviour::kSqueeze)
{
    // TODO should be an exception.
    assert(num_bins > 0);

    set_uniform_ranges(range_min, range_max);
}

/**
 * @brief
 */
Histogram::Histogram(const std::vector<double>& ranges) :
    bins_                  (ranges.size() - 1, 0.0),
    ranges_                (ranges),
    out_of_range_behaviour (OutOfRangeBehaviour::kSqueeze)
{
    // TODO replace by exception. maybe not even neccessary, as constructor might just throw anyway.
    assert(ranges.size() >= 2);

    assert(std::is_sorted(ranges.begin(), ranges.end()));
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
    for (size_t i = 0; i <= bins() + 1; ++i) {
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

double Histogram::bin_width(size_t bin_num) const
{
    return ranges_[bin_num + 1] - ranges_[bin_num];
}

int Histogram::find_bin (double x)
{
    if (x < min()) {
        return -1;
    }
    if (x >= max()) {
        return bins();
    }

    // Get the bin for the uniform ranges case.
    int bin = static_cast <int> (std::floor( (x - min()) / bin_width(0) ));

    // If this worked, simply return the bin number.
    if (ranges_[bin] <= x && x < ranges_[bin + 1]) {
        return bin;
    }

    // If not, do a binary search.
    auto it = std::upper_bound(ranges_.begin(), ranges_.end(), x);
    return std::distance(ranges_.begin(), it) - 1;
}

bool Histogram::check_range(double x)
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
//     Statistics
// =================================================================================================

double Histogram::min_value() const
{
    return *std::min_element(bins_.begin(), bins_.end());
}

double Histogram::max_value() const
{
    return *std::max_element(bins_.begin(), bins_.end());
}

size_t Histogram::min_bin() const
{
    return std::distance(bins_.begin(), std::min_element(bins_.begin(), bins_.end()));
}

size_t Histogram::max_bin() const
{
    return std::distance(bins_.begin(), std::max_element(bins_.begin(), bins_.end()));
}

double Histogram::median() const
{
    const double mid = sum() / 2;
    double part = 0.0;

    size_t i;
    for (i = 0; i < bins(); ++i) {
        if (part + bins_[i] < mid) {
            part += bins_[i];
        } else {
            break;
        }
    }
    assert(part < mid && i < bins());

    // Find the relative position of mid within the interval [part, part + bin[i]).
    // This determines where exactly our median is.
    const double pos = (mid - part) / bins_[i];

    // Now find this position within the range of the bin and return it.
    return ranges_[i] + pos * bin_width(i);
}

double Histogram::mean() const
{
    // We are using the recurrence relation
    //   M(n) = M(n-1) + (x[n] - M(n-1)) (w(n)/(W(n-1) + w(n)))
    //   W(n) = W(n-1) + w(n)
    // which is also used in the GNU Scientific Library.

    double wmean  = 0.0;
    double weight = 0.0;

    for (size_t i = 0; i < bins(); ++i) {
        const double xi = (ranges_[i + 1] + ranges_[i]) / 2;
        const double wi = bins_[i];

        if (wi > 0.0) {
            weight += wi;
            wmean += (xi - wmean) * (wi / weight);
        }
    }

    return wmean;
}

double Histogram::sigma() const
{
    // We use the same method as in the GNU Scientific Library.
    // It is a two-pass algorithm for stability.
    // Could also use a single pass formula, as given in
    // N. J. Higham: "Accuracy and Stability of Numerical Methods", p.12

    const double wmean = mean();
    double weight = 0.0;
    double wvar   = 0.0;

    for (size_t i = 0; i < bins(); ++i) {
        const double xi = (ranges_[i + 1] + ranges_[i]) / 2;
        const double wi = bins_[i];

        if (wi > 0.0) {
            const double delta = (xi - wmean);

            weight += wi;
            wvar   += (delta * delta - wvar) * (wi / weight);
        }
    }

    return std::sqrt(wvar);
}

double Histogram::sum() const
{
    return std::accumulate (bins_.begin(), bins_.end(), 0.0);
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
