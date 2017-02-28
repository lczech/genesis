#ifndef GENESIS_UTILS_MATH_HISTOGRAM_H_
#define GENESIS_UTILS_MATH_HISTOGRAM_H_

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
 * @brief Header of Histogram class.
 *
 * @file
 * @ingroup utils
 */

#include <stddef.h>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Histogram;
bool equal_ranges( Histogram const& lhs, Histogram const& rhs );
void swap( Histogram& lhs, Histogram& rhs ) noexcept;

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

    friend void swap( Histogram& lhs, Histogram& rhs ) noexcept;

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

    bool operator== ( Histogram const& rhs );

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

} // namespace utils
} // namespace genesis

#endif // include guard
