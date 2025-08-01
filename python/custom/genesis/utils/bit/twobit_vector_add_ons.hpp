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
#include <genesis/utils/bit/twobit_vector.hpp>

// Add ons for the TwobitVector class
void genesis_utils_bit_twobit_vector_add_ons(
    pybind11::class_<
        genesis::utils::bit::TwobitVector,
        std::shared_ptr<genesis::utils::bit::TwobitVector>
    > &cl
);

// Add ons for the namespace
void genesis_utils_bit_twobit_vector_functions_add_ons(
    pybind11::module &m
);

// Add ons for the iterator classes. They are all the same, so we use a function template
// that covers all cases, to remove code duplication
template<class Iterator>
void genesis_utils_bit_iterator_add_ons(
    pybind11::class_<
        Iterator,
        std::shared_ptr<Iterator>
    > &cl
) {
    cl.def(
        "__iter__",
        [](Iterator &it) -> Iterator &
        {
            return it;
        }
    );
    cl.def(
        "__next__",
        [](Iterator &it)
        {
            Iterator end;
            if (it == end) {
                throw pybind11::stop_iteration();
            }
            Iterator current = it;
            ++it;
            return current;
        }
    );
}
