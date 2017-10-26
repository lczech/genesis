#ifndef GENESIS_PLACEMENT_FUNCTION_NHD_H_
#define GENESIS_PLACEMENT_FUNCTION_NHD_H_

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
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <cstddef>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace placement {

    class Sample;
    class SampleSet;

}

namespace utils {

    template<typename T>
    class Matrix;

}

namespace placement {

// =================================================================================================
//     Node Histogram Distance
// =================================================================================================


/**
* @brief Calculate the Node Histogram Distance of two Sample%s.
*/
double node_histogram_distance (
    Sample const& sample_a,
    Sample const& sample_b,
    size_t const  histogram_bins = 25,
    bool use_negative_axis = true
);

/**
* @brief Calculate the
* @link node_histogram_distance( Sample const&, Sample const&, size_t ) node_histogram_distance()@endlink
* for every pair of Sample%s in the SampleSet.
*/
utils::Matrix<double> node_histogram_distance (
    SampleSet const& sample_set,
    size_t const     histogram_bins = 25,
    bool use_negative_axis = true
);

} // namespace placement
} // namespace genesis

#endif // include guard
