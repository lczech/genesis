#pragma once

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2023-2025 Giannis Reppas and Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

#include <pybind11/pybind11.h>
#include <genesis/util/math/compensated_sum.hpp>

template <typename T>
void genesis_util_math_compensated_sum_add_ons( pybind11::class_<T, std::shared_ptr<T>> &cl )
{
    cl.def(
        "__float__",
        [](T const &self) {
            return static_cast<double>(self);
        },
        "Convert the compensated sum to a Python float via operator float()"
    );
}
