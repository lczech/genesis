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

// Custom file in the python/custom dir
#include <genesis/utils/bit/bitvector_add_ons.hpp>

#include <string>

#include <pybind11/pybind11.h>

#include <genesis/utils/bit/bitvector.hpp>
#include <genesis/utils/bit/bitvector/function.hpp>
#include <genesis/utils/bit/bitvector/operator.hpp>

namespace py  = pybind11;

void genesis_utils_bit_bitvector_add_ons(
    pybind11::class_<
        genesis::utils::bit::Bitvector,
        std::shared_ptr<genesis::utils::bit::Bitvector>
    > &cl
) {
    // Custom constructor from vector<bool>
    cl.def(
        pybind11::init(
            [](std::vector<bool> const& input)
            {
                return ::genesis::utils::bit::Bitvector(input.begin(), input.end());
            }
        ),
        pybind11::arg("values")
    );

    // Add operators
    cl.def(
        "__len__",
        []( ::genesis::utils::bit::Bitvector const& self )
        {
            return self.size();
        }
    );
    cl.def(
        "__setitem__",
        []( ::genesis::utils::bit::Bitvector& self, size_t i, bool v )
        {
            return self.set(i, v);
        },
        py::arg("index"),
        py::arg("value")
    );

    // Add custom free function operators
    cl.def(
        "__and__",
        []( ::genesis::utils::bit::Bitvector &self, ::genesis::utils::bit::Bitvector const& rhs )
        {
            return self & rhs;
        },
        pybind11::arg("rhs")
    );
    cl.def(
        "__or__",
        []( ::genesis::utils::bit::Bitvector &self, ::genesis::utils::bit::Bitvector const& rhs )
        {
            return self | rhs;
        },
        pybind11::arg("rhs")
    );
    cl.def(
        "__xor__",
        []( ::genesis::utils::bit::Bitvector &self, ::genesis::utils::bit::Bitvector const& rhs )
        {
            return self ^ rhs;
        },
        pybind11::arg("rhs")
    );
}
