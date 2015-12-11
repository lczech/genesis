#ifndef GENESIS_UTILS_MATH_HISTOGRAM_H_
#define GENESIS_UTILS_MATH_HISTOGRAM_H_

/**
 * @brief Header of Histogram class.
 *
 * @file
 * @ingroup utils
 */

#include <stddef.h>
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
 */
class Histogram
{
    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

public:

    enum class OutOfRangeBehaviour {
        kIgnore,
        kSqueeze,
        kThrow
    };

    typedef std::vector<double>::iterator       iterator;
    typedef std::vector<double>::const_iterator const_iterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    Histogram(
        size_t num_bins
    );

    Histogram(
        size_t num_bins,
        double range_min,
        double range_max
    );

    Histogram(
        const std::vector<double>& ranges
    );

    ~Histogram() = default;

    Histogram(Histogram const&) = default;
    Histogram(Histogram&&)      = default;

    Histogram& operator= (Histogram const&) = default;
    Histogram& operator= (Histogram&&)      = default;

    void swap (Histogram& other);

    // -------------------------------------------------------------------------
    //     General Methods
    // -------------------------------------------------------------------------

    void set_ranges( const std::vector<double>& ranges );

    void set_uniform_ranges( const double min, const double max );

    void clear();

    OutOfRangeBehaviour out_of_range_behaviour() const;

    void                out_of_range_behaviour( OutOfRangeBehaviour v );

    // -------------------------------------------------------------------------
    //     Comparison
    // -------------------------------------------------------------------------

    friend bool operator== (Histogram const& lhs, Histogram const& rhs);

    friend bool equal_ranges (Histogram const& lhs, Histogram const& rhs);

    // -------------------------------------------------------------------------
    //     Bin Access
    // -------------------------------------------------------------------------

    double& at( size_t bin_num );

    double at( size_t bin_num ) const;

    double& operator [] ( size_t bin_num );

    double operator [] ( size_t bin_num ) const;

    // -------------------------------------------------------------------------
    //     Bin Iterators
    // -------------------------------------------------------------------------

    iterator begin();

    iterator end();

    const_iterator begin() const;

    const_iterator end() const;

    const_iterator cbegin() const;

    const_iterator cend() const;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    size_t bins() const;

    std::pair<double, double> bin_range( size_t bin_num ) const;

    double bin_midpoint( size_t bin_num ) const;

    double bin_width( size_t bin_num ) const;

    int find_bin( double x ) const;

    double range_min() const;

    double range_max() const;

    bool check_range( double x ) const;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    int increment( double x );

    int accumulate( double x, double weight );

    void increment_bin( size_t bin );

    void accumulate_bin( size_t bin, double weight );

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<double> bins_;
    std::vector<double> ranges_;

    OutOfRangeBehaviour out_of_range_behaviour_;
};

} // namespace genesis

// =================================================================================================
//     Namespace std Extension
// =================================================================================================

namespace std {

template<>
inline void swap( genesis::Histogram& lhs, genesis::Histogram& rhs ) noexcept
{
    lhs.swap(rhs);
}

} // namespace std

#endif // include guard
