#ifndef GENESIS_UTILS_MATH_REGRESSION_DATAFRAME_H_
#define GENESIS_UTILS_MATH_REGRESSION_DATAFRAME_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/containers/dataframe.hpp"
#include "genesis/utils/containers/matrix.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Dataframe Helper Functions
// =================================================================================================

Dataframe glm_prepare_dataframe( Dataframe const& df, std::string& report );

Dataframe glm_prepare_dataframe( Dataframe const& df );

Matrix<double> glm_convert_dataframe(
    Dataframe const& df,
    std::vector<std::string> row_order
);

Matrix<double> glm_convert_dataframe(
    Dataframe const& df
);

} // namespace utils
} // namespace genesis

#endif // include guard
