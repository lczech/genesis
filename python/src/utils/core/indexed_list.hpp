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
