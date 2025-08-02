#ifndef GENESIS_UTIL_MATH_REGRESSION_DATAFRAME_H_
#define GENESIS_UTIL_MATH_REGRESSION_DATAFRAME_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
 * @ingroup util
 */

#include <genesis/util/container/dataframe.hpp>
#include <genesis/util/container/matrix.hpp>

#include <string>
#include <vector>

namespace genesis {
namespace util {
namespace math {

// =================================================================================================
//     Dataframe Helper Functions
// =================================================================================================

genesis::util::container::Dataframe glm_prepare_dataframe(
    genesis::util::container::Dataframe const& df,
    std::string& report
);

genesis::util::container::Dataframe glm_prepare_dataframe(
    genesis::util::container::Dataframe const& df
);

genesis::util::container::Matrix<double> glm_convert_dataframe(
    genesis::util::container::Dataframe const& df,
    std::vector<std::string> row_order
);

genesis::util::container::Matrix<double> glm_convert_dataframe(
    genesis::util::container::Dataframe const& df
);

} // namespace math
} // namespace util
} // namespace genesis

#endif // include guard
