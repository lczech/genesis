#ifndef GENESIS_UTILS_MATH_EUCLIDEAN_KMEANS_H_
#define GENESIS_UTILS_MATH_EUCLIDEAN_KMEANS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/math/kmeans.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace utils {

// =================================================================================================
//     Euclidean K-Means Specialization
// =================================================================================================

class EuclideanKmeans
    : public Kmeans< std::vector<double> >
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Constants
    // -------------------------------------------------------------------------

    using Point = std::vector<double>;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    EuclideanKmeans( size_t dimensions );
    virtual ~EuclideanKmeans() override = default;

    EuclideanKmeans( EuclideanKmeans const& ) = default;
    EuclideanKmeans( EuclideanKmeans&& )      = default;

    EuclideanKmeans& operator= ( EuclideanKmeans const& ) = default;
    EuclideanKmeans& operator= ( EuclideanKmeans&& )      = default;

    // -------------------------------------------------------------------------
    //     Default K-Means Functions
    // -------------------------------------------------------------------------

private:

    virtual bool data_validation( std::vector<Point> const& data ) const override;

    virtual void update_centroids(
        std::vector<Point>  const& data,
        std::vector<size_t> const& assignments,
        std::vector<Point>&        centroids
    ) override;

    virtual double distance( Point const& lhs, Point const& rhs ) const override;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    size_t dimensions_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
