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
#include <genesis/util/bit/twobit_vector_add_ons.hpp>

#include <string>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <genesis/util/bit/twobit_vector.hpp>
#include <genesis/util/bit/twobit_vector/function.hpp>
#include <genesis/util/bit/twobit_vector/iterator_deletions.hpp>
#include <genesis/util/bit/twobit_vector/iterator_insertions.hpp>
#include <genesis/util/bit/twobit_vector/iterator_substitutions.hpp>

namespace py  = pybind11;

void genesis_util_bit_twobit_vector_add_ons(
    pybind11::class_<
        genesis::util::bit::TwobitVector,
        std::shared_ptr<genesis::util::bit::TwobitVector>
    > &cl
) {
    // Add operators
    cl.def(
        "__len__",
        []( genesis::util::bit::TwobitVector const& self )
        {
            return self.size();
        }
    );
    cl.def(
        "__setitem__",
        [](
            genesis::util::bit::TwobitVector& self,
            size_t i,
            genesis::util::bit::TwobitVector::ValueType v
        ) {
            return self.set(i, v);
        },
        py::arg("index"),
        py::arg("value")
    );
}

void genesis_util_bit_twobit_vector_functions_add_ons(
    pybind11::module &m
) {
    using namespace genesis::util::bit;

    // ---------------------------------------------------------------------
    //     Low-level: return iterator objects
    // ---------------------------------------------------------------------

    // These are equivalent to the C++ helper functions, but might be more low-level
    // than what the typical Python user expects, as they yield an iterator to the object,
    // instead of the object itself during the loop.

    m.def(
        "iterate_substitutions",
        [](const TwobitVector &tv)
        {
            return IteratorSubstitutions(tv);
        },
        py::return_value_policy::move,
        "Yield IteratorSubstitutions instances for each substitution. This is a more efficient "
        "alternative to substitutions() if hash values of the TwobitVector in each iteration "
        "need to be computed. Use .vector() on the iteration object to get the current "
        "TwobitVector during the loop."
    );

    m.def(
        "iterate_deletions",
        [](const TwobitVector &tv)
        {
            return IteratorDeletions(tv);
        },
        py::keep_alive<0, 1>(),
        "Yield IteratorDeletions instances for each deletion. This is a more efficient "
        "alternative to deletions() if hash values of the TwobitVector in each iteration "
        "need to be computed. Use .vector() on the iteration object to get the current "
        "TwobitVector during the loop."
    );

    m.def(
        "iterate_insertions",
        [](const TwobitVector &tv)
        {
            return IteratorInsertions(tv);
        },
        py::keep_alive<0, 1>(),
        "Yield IteratorInsertions instances for each insertion. This is a more efficient "
        "alternative to insertions() if hash values of the TwobitVector in each iteration "
        "need to be computed. Use .vector() on the iteration object to get the current "
        "TwobitVector during the loop."
    );

    // ---------------------------------------------------------------------
    //     High-level: yield actual TwobitVector copies
    // ---------------------------------------------------------------------

    // These are higher level functions that do not have a C++ equivalent directly,
    // but basically boil down to an additional dereference operator on each iterator.

    m.def(
        "substitutions",
        [](const TwobitVector &tv)
        {
            return py::make_iterator(
                IteratorSubstitutions(tv),
                IteratorSubstitutions(),
                py::return_value_policy::copy
            );
        },
        py::keep_alive<0, 1>(),
        "Yield TwobitVector instances for each substitution. This is a simple iterator to "
        "be used in a loop, where each iteration is the TwobitVector resulting from a "
        "substitution. See also iterate_substitutions() for a more efficient alternative if hash "
        "values of each yielded TwobitVector need to be computed."
    );

    m.def(
        "deletions",
        [](const TwobitVector &tv)
        {
            return py::make_iterator(
                IteratorDeletions(tv),
                IteratorDeletions(),
                py::return_value_policy::copy
            );
        },
        py::keep_alive<0, 1>(),
        "Yield TwobitVector instances for each deletion. This is a simple iterator to be used "
        "in a loop, where each iteration is the TwobitVector resulting from a deletion. "
        "See also iterate_deletions() for a more efficient alternative if hash values of each "
        "yielded TwobitVector need to be computed."
    );

    m.def(
        "insertions",
        [](const TwobitVector &tv)
        {
            return py::make_iterator(
                IteratorInsertions(tv),
                IteratorInsertions(),
                py::return_value_policy::copy
            );
        },
        py::keep_alive<0, 1>(),
        "Yield TwobitVector instances for each insertion. This is a simple iterator to be used "
        "in a loop, where each iteration is the TwobitVector resulting from a insertion. "
        "See also iterate_insertions() for a more efficient alternative if hash values of each "
        "yielded TwobitVector need to be computed."
    );

}
