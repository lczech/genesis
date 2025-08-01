#ifndef GENESIS_UTILS_MATH_REGRESSION_DATAFRAME_H_
#define GENESIS_UTILS_MATH_REGRESSION_DATAFRAME_H_

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
 * @ingroup utils
 */

#include <genesis/utils/container/dataframe.hpp>
#include <genesis/utils/container/matrix.hpp>

#include <string>
#include <vector>

namespace genesis {
namespace utils {
namespace math {

// =================================================================================================
//     Dataframe Helper Functions
// =================================================================================================

genesis::utils::containers::Dataframe glm_prepare_dataframe(
    genesis::utils::containers::Dataframe const& df,
    std::string& report
);

genesis::utils::containers::Dataframe glm_prepare_dataframe(
    genesis::utils::containers::Dataframe const& df
);

genesis::utils::containers::Matrix<double> glm_convert_dataframe(
    genesis::utils::containers::Dataframe const& df,
    std::vector<std::string> row_order
);

genesis::utils::containers::Matrix<double> glm_convert_dataframe(
    genesis::utils::containers::Dataframe const& df
);

} // namespace math
} // namespace utils
} // namespace genesis

#endif // include guard
