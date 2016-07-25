#ifndef GENESIS_PLACEMENT_FUNCTION_SAMPLE_SET_H_
#define GENESIS_PLACEMENT_FUNCTION_SAMPLE_SET_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "placement/sample.hpp"
#include "placement/sample_set.hpp"
#include "tree/tree_set.hpp"

#include <iosfwd>
#include <string>

namespace genesis {
namespace placement {

// =================================================================================================
//     Sample Set Functions
// =================================================================================================

Sample      * find_sample( SampleSet      & sset, std::string const& name );
Sample const* find_sample( SampleSet const& sset, std::string const& name );

bool all_identical_trees( SampleSet const& sset );

tree::TreeSet tree_set( SampleSet const& sset );

Sample merge_all( SampleSet const& sset );

// =================================================================================================
//     Output
// =================================================================================================

std::ostream& operator << ( std::ostream& out, SampleSet const& sset );

} // namespace placement
} // namespace genesis

#endif // include guard
