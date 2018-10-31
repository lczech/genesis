/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename T>
void PythonExportClass_::genesis::utils::IndexedList(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IndexedList
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using IndexedListType = IndexedList<typename T>;

    pybind11::class_< IndexedListType, std::shared_ptr<IndexedListType> > ( scope, name.c_str() )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::utils::IndexedList::IndexedList ()")
        )
        .def(
            pybind11::init< size_type >(),
            pybind11::arg("n"),
            get_docstring("::genesis::utils::IndexedList::IndexedList (size_type n)")
        )
        .def(
            pybind11::init< size_type, value_type const & >(),
            pybind11::arg("n"),
            pybind11::arg("val"),
            get_docstring("::genesis::utils::IndexedList::IndexedList (size_type n, value_type const & val)")
        )
        .def(
            pybind11::init< IndexedList const & >(),
            pybind11::arg("other"),
            get_docstring("::genesis::utils::IndexedList::IndexedList (IndexedList const & other)")
        )
        .def(
            pybind11::init< std::initializer_list< value_type > >(),
            pybind11::arg("il"),
            get_docstring("::genesis::utils::IndexedList::IndexedList (std::initializer_list< value_type > il)")
        )

        // Public Member Functions

        .def(
            "at",
            ( const_reference ( IndexedListType::* )( size_type ) const )( &IndexedListType::at ),
            pybind11::arg("n")
        )
        .def(
            "at",
            ( reference ( IndexedListType::* )( size_type ))( &IndexedListType::at ),
            pybind11::arg("n")
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
            ( void ( IndexedListType::* )( value_type && ))( &IndexedListType::push_back ),
            pybind11::arg("value")
        )
        .def(
            "push_back",
            ( void ( IndexedListType::* )( value_type const & ))( &IndexedListType::push_back ),
            pybind11::arg("value")
        )
        .def(
            "size",
            ( size_type ( IndexedListType::* )(  ) const )( &IndexedListType::size )
        )
        .def(
            "swap",
            ( void ( IndexedListType::* )( IndexedList & ))( &IndexedListType::swap ),
            pybind11::arg("other")
        )

        // Operators

        .def(
            "__getitem__",
            ( reference ( IndexedListType::* )( size_type ))( &IndexedListType::operator[] ),
            pybind11::arg("n")
        )
        .def(
            "__getitem__",
            ( const_reference ( IndexedListType::* )( size_type ) const )( &IndexedListType::operator[] ),
            pybind11::arg("n")
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::utils::IndexedList& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            py::keep_alive<0, 1>()
        )
    ;
}

template <typename T>
void PythonExportClass_::genesis::utils::IndexedListIterator(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IndexedListIterator
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using IndexedListIteratorType = IndexedListIterator<typename T>;

    pybind11::class_< IndexedListIteratorType, std::shared_ptr<IndexedListIteratorType> > ( scope, name.c_str() )
        .def(
            pybind11::init< IndexedList< value_type > & >(),
            pybind11::arg("il")
        )
        .def(
            pybind11::init< IndexedList< value_type > &, size_type >(),
            pybind11::arg("il"),
            pybind11::arg("pos")
        )
        .def(
            pybind11::init< IndexedListIterator const & >(),
            pybind11::arg("x")
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
    ;
}
