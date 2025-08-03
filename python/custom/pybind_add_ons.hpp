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

// We use this header in other headers of our custom bindings code,
// in places were generic pybind-related additional funcitonality is needed.
// This way, we only need to define them once here.

#include <pybind11/pybind11.h>

// Provide a hash and equality for pybind11::object so it can be used as, e.g, a hash map key.
namespace std
{
    template<> struct hash<pybind11::object>
    {
        size_t operator()(pybind11::object const &o) const noexcept
        {
            // defer to Python's __hash__
            return (size_t) pybind11::hash(o);
        }
    };
    template<> struct equal_to<pybind11::object>
    {
        bool operator()(pybind11::object const &a, pybind11::object const &b) const noexcept
        {
            // defer to Python's equality
            return a.equal(b);
        }
    };
}
