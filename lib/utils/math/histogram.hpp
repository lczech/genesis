#ifndef GENESIS_UTILS_MATH_HISTOGRAM_H_
#define GENESIS_UTILS_MATH_HISTOGRAM_H_

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
 * @brief
 *
 * from https://www.gnu.org/software/gsl/manual/html_node/Histograms.html
 *
 *        [ bin[0] )[ bin[1] )[ bin[2] )[ bin[3] )[ bin[4] )
 *     ---|---------|---------|---------|---------|---------|---  x
 *      r[0]      r[1]      r[2]      r[3]      r[4]      r[5]
 *
 * TODO explain why std::nextafter(max, max + 1); might be important
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

    Histogram(const size_t num_bins);

    Histogram(const size_t num_bins, const double range_min, const double range_max);

    Histogram(const std::vector<double>& ranges);

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

    double bin_width(size_t bin_num) const;

    int find_bin (double x);

    bool check_range(double x);

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    int increment (double x);

    int accumulate (double x, double weight);

    void increment_bin (size_t bin);

    void accumulate_bin (size_t bin, double weight);

    // -------------------------------------------------------------------------
    //     Statistics
    // -------------------------------------------------------------------------

    double min_value() const;

    double max_value() const;

    size_t min_bin() const;

    size_t max_bin() const;

    double median() const;

    /**
     * @brief Compute the bin-weighted arithmetic mean.
     *
     * The histogram is regarded as a probability distribution.
     * Negative bin values are ignored for the purposes of this calculation.
     * The accuracy of the result is limited by the bin width.
     */
    double mean() const;

    /**
     * @brief Compute the bin-weighted standard deviation.
     *
     * The histogram is regarded as a probability distribution.
     * Negative bin values are ignored for the purposes of this calculation.
     * The accuracy of the result is limited by the bin width.
     */
    double sigma() const;

    double sum() const;

    // -------------------------------------------------------------------------
    //     Dump
    // -------------------------------------------------------------------------

    std::string dump() const;

};

} // namespace genesis

#endif // include guard
