#ifndef GENESIS_POPULATION_WINDOW_VARIANT_WINDOW_ITERATOR_H_
#define GENESIS_POPULATION_WINDOW_VARIANT_WINDOW_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/formats/variant_input_iterator.hpp"
#include "genesis/population/window/base_window_iterator.hpp"
#include "genesis/population/window/window_view_iterator.hpp"
#include "genesis/population/window/window_view.hpp"
#include "genesis/population/window/window.hpp"

namespace genesis {
namespace population {

// =================================================================================================
//     Generic Variant Window Iterator
// =================================================================================================

/**
 * @brief Typedef for our uniform window iterator type.
 *
 * This typedef is used for any window iterator over an VariantInputIterator. Because we want to
 * model different types of window iterators, some of which use Window, some of which use WindowView,
 * we here use the latter, and hence expect to use WindowViewIterator to wrap window iterators.
 */
using VariantWindowIterator = BaseWindowIterator<
    VariantInputIterator::Iterator,
    VariantInputIterator::value_type,
    WindowView<VariantInputIterator::value_type>
>;

} // namespace population
} // namespace genesis

#endif // include guard
