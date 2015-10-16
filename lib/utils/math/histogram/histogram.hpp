#ifndef GENESIS_UTILS_MATH_HISTOGRAM_HISTOGRAM_H_
#define GENESIS_UTILS_MATH_HISTOGRAM_HISTOGRAM_H_

/**
 * @brief Header of Histogram class.
 *
 * @file
 * @ingroup utils
 */

#include <string>
#include <utility>
#include <vector>

namespace genesis {

// =================================================================================================
//     Histogram
// =================================================================================================

/**
 * @brief Histogram class for accumulating and summarizing data.
 *
 * Many details of the implementation are taken from the GNU Scientific Library:
 * https://www.gnu.org/software/gsl/manual/html_node/Histograms.html
 * See there for some more information.
 *
 * The structure of the kept data is as follows:
 *
 *        [ bin[0] )[ bin[1] )[ bin[2] )[ bin[3] )[ bin[4] )
 *     ---|---------|---------|---------|---------|---------|---  x
 *      r[0]      r[1]      r[2]      r[3]      r[4]      r[5]
 *
 * The number of bins needs to be determined at construction. The number of ranges is always one
 * more than the number of bins.
 *
 * If one of the constructors with value vectors but without min/max values is used, the max value
 * will be determined using `std::nextafter(max, max + 1)` on the max element of the value list.
 * This is because we want this element to just fit into the range.
 */
class Histogram
{
    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    enum class OutOfRangeBehaviour {
        kIgnore,
        kSqueeze,
        kThrow
    };

    typedef std::vector<double>::const_iterator const_iterator;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

protected:

    std::vector<double> bins_;
    std::vector<double> ranges_;

public:

    OutOfRangeBehaviour out_of_range_behaviour;

    // -------------------------------------------------------------------------
    //     Constructor and Destructor
    // -------------------------------------------------------------------------

public:

    // TODO Maybe a builder pattern would be neat here!

    Histogram(
        const size_t num_bins
    );

    Histogram(
        const size_t num_bins,
        const double range_min,
        const double range_max
    );

    Histogram(
        const size_t num_bins,
        const std::vector<double>& values,
        const double weight = 1.0
    );

    Histogram(
        const size_t num_bins,
        const std::vector<std::pair<double,double>>& weighted_values
    );

    Histogram(
        const size_t num_bins,
        const double range_min,
        const double range_max,
        const std::vector<double>& values,
        const double weight = 1.0
    );

    Histogram(
        const size_t num_bins,
        const double range_min,
        const double range_max,
        const std::vector<std::pair<double,double>>& weighted_values
    );

    Histogram(
        const std::vector<double>& ranges
    );

    void set_ranges(const std::vector<double>& ranges);

    void set_uniform_ranges(const double min, const double max);

    void clear();

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    const_iterator begin() const;

    const_iterator end() const;

    double operator [] (size_t bin_num) const;

    double value(size_t bin_num) const;

    double min() const;

    double max() const;

    size_t bins() const;

    std::pair<double, double> bin_range(size_t bin_num) const;

    double bin_midpoint(size_t bin_num) const;

    double bin_width(size_t bin_num) const;

    int find_bin (double x) const;

    bool check_range(double x) const;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    int increment (double x);

    int accumulate (double x, double weight);

    void increment_bin (size_t bin);

    void accumulate_bin (size_t bin, double weight);

    // -------------------------------------------------------------------------
    //     Dump
    // -------------------------------------------------------------------------

    std::string dump() const;

};

} // namespace genesis

#endif // include guard
