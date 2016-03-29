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
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename T>
void PythonExportClass_IndexedList(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IndexedList
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using IndexedListType = IndexedList<typename T>;

    boost::python::class_< IndexedListType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< size_type >(( boost::python::arg("n") )) )
        .def( boost::python::init< size_type, const value_type & >(( boost::python::arg("n"), boost::python::arg("val") )) )
        .def( boost::python::init< const IndexedList & >(( boost::python::arg("x") )) )

        // Public Member Functions

        .def(
            "at",
            ( const_reference ( IndexedListType::* )( size_type ) const )( &IndexedListType::at ),
            ( boost::python::arg("n") )
        )
        .def(
            "at",
            ( reference ( IndexedListType::* )( size_type ))( &IndexedListType::at ),
            ( boost::python::arg("n") )
        )
        .def(
            "back",
            ( const_reference ( IndexedListType::* )(  ) const )( &IndexedListType::back )
        )
        .def(
            "back",
            ( reference ( IndexedListType::* )(  ))( &IndexedListType::back )
        )
        .def(
            "cbegin",
            ( const_iterator ( IndexedListType::* )(  ) const )( &IndexedListType::cbegin )
        )
        .def(
            "cend",
            ( const_iterator ( IndexedListType::* )(  ) const )( &IndexedListType::cend )
        )
        .def(
            "empty",
            ( bool ( IndexedListType::* )(  ) const )( &IndexedListType::empty )
        )
        .def(
            "front",
            ( const_reference ( IndexedListType::* )(  ) const )( &IndexedListType::front )
        )
        .def(
            "front",
            ( reference ( IndexedListType::* )(  ))( &IndexedListType::front )
        )
        .def(
            "push_back",
            ( void ( IndexedListType::* )(  ))( &IndexedListType::push_back )
        )
        .def(
            "size",
            ( size_type ( IndexedListType::* )(  ) const )( &IndexedListType::size )
        )

        // Operators

        .def(
            "__getitem__",
            ( reference ( IndexedListType::* )( size_type ))( &IndexedListType::operator[] ),
            ( boost::python::arg("n") )
        )
        .def(
            "__getitem__",
            ( const_reference ( IndexedListType::* )( size_type ) const )( &IndexedListType::operator[] ),
            ( boost::python::arg("n") )
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &IndexedListType::begin, &IndexedListType::end )
        )
    ;
}

template <typename T>
void PythonExportClass_IndexedListIterator(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IndexedListIterator
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using IndexedListIteratorType = IndexedListIterator<typename T>;

    boost::python::class_< IndexedListIteratorType > ( name.c_str(), boost::python::init< IndexedList< value_type > & >(( boost::python::arg("il") )) )
        .def( boost::python::init< IndexedList< value_type > &, size_type >(( boost::python::arg("il"), boost::python::arg("pos") )) )

        // Operators

        .def( boost::python::self != boost::python::self )
        .def( boost::python::self == boost::python::self )
    ;
}
