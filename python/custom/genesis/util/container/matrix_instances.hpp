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

// #include <genesis/util/container/matrix.hpp>
// #include <genesis/util/container/matrix/col.hpp>
// #include <genesis/util/container/matrix/operator.hpp>
// #include <genesis/util/container/matrix/reader.hpp>
// #include <genesis/util/container/matrix/row.hpp>
// #include <genesis/util/container/matrix/simple_reader.hpp>
// #include <genesis/util/container/matrix/writer.hpp>

#include <pybind11/pybind11.h>

namespace genesis {
namespace util {
namespace container {

// Standard types that we are going to need.
// template class Matrix<double>;

// Explicit instantiations for Python objects, so that any type works in Python.
// Uses the pybind11::object hash and equal functions provided in pybind_add_ons.hpp
// template class Matrix<pybind11::object>;

} // namespace container
} // namespace util
} // namespace genesis
