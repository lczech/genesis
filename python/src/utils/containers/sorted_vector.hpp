/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <class T, class Compare>
void PythonExportClass_::genesis::utils::SortedVector(std::string name)
{

    // -------------------------------------------------------------------
    //     Class SortedVector
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using SortedVectorType = SortedVector<class T, class Compare>;

    pybind11::class_< SortedVectorType, std::shared_ptr<SortedVectorType> > ( scope, name.c_str() )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< SortedVector const & >(),
            pybind11::arg("arg")
        )
        .def(
            pybind11::init< std::initializer_list< value_type > >(),
            pybind11::arg("il")
        )

        // Public Member Functions

        .def(
            "at",
            ( const_reference ( SortedVectorType::* )( size_type ) const )( &SortedVectorType::at ),
            pybind11::arg("index")
        )
        .def(
            "at",
            ( reference ( SortedVectorType::* )( size_type ))( &SortedVectorType::at ),
            pybind11::arg("index")
        )
        .def(
            "back",
            ( const_reference ( SortedVectorType::* )(  ) const )( &SortedVectorType::back )
        )
        .def(
            "back",
            ( reference ( SortedVectorType::* )(  ))( &SortedVectorType::back )
        )
        .def(
            "cbegin",
            ( const_iterator ( SortedVectorType::* )(  ))( &SortedVectorType::cbegin )
        )
        .def(
            "cend",
            ( const_iterator ( SortedVectorType::* )(  ))( &SortedVectorType::cend )
        )
        .def(
            "clear",
            ( void ( SortedVectorType::* )(  ))( &SortedVectorType::clear )
        )
        .def(
            "contains",
            ( bool ( SortedVectorType::* )( const_reference ) const )( &SortedVectorType::contains ),
            pybind11::arg("value"),
            get_docstring("bool ::genesis::utils::SortedVector::contains (const_reference value) const")
        )
        .def(
            "empty",
            ( bool ( SortedVectorType::* )(  ) const )( &SortedVectorType::empty )
        )
        .def(
            "front",
            ( const_reference ( SortedVectorType::* )(  ) const )( &SortedVectorType::front )
        )
        .def(
            "front",
            ( reference ( SortedVectorType::* )(  ))( &SortedVectorType::front )
        )
        .def(
            "index_of",
            ( size_type ( SortedVectorType::* )( const_reference ) const )( &SortedVectorType::index_of ),
            pybind11::arg("value"),
            get_docstring("size_type ::genesis::utils::SortedVector::index_of (const_reference value) const")
        )
        .def(
            "insert",
            ( void ( SortedVectorType::* )( InputIterator, InputIterator ))( &SortedVectorType::insert ),
            pybind11::arg("first"),
            pybind11::arg("last"),
            get_docstring("void ::genesis::utils::SortedVector::insert (InputIterator first, InputIterator last)")
        )
        .def(
            "insert",
            ( void ( SortedVectorType::* )( const_reference ))( &SortedVectorType::insert ),
            pybind11::arg("value"),
            get_docstring("void ::genesis::utils::SortedVector::insert (const_reference value)")
        )
        .def(
            "insert",
            ( void ( SortedVectorType::* )( value_type && ))( &SortedVectorType::insert ),
            pybind11::arg("value"),
            get_docstring("void ::genesis::utils::SortedVector::insert (value_type && value)")
        )
        .def(
            "remove",
            ( void ( SortedVectorType::* )( const_reference ))( &SortedVectorType::remove ),
            pybind11::arg("value"),
            get_docstring("void ::genesis::utils::SortedVector::remove (const_reference value)")
        )
        .def(
            "reserve",
            ( void ( SortedVectorType::* )( size_t ))( &SortedVectorType::reserve ),
            pybind11::arg("n"),
            get_docstring("void ::genesis::utils::SortedVector::reserve (size_t n)")
        )
        .def(
            "size",
            ( size_type ( SortedVectorType::* )(  ) const )( &SortedVectorType::size )
        )

        // Operators

        .def(
            "__getitem__",
            ( reference ( SortedVectorType::* )( size_type ))( &SortedVectorType::operator[] ),
            pybind11::arg("index")
        )
        .def(
            "__getitem__",
            ( const_reference ( SortedVectorType::* )( size_type ) const )( &SortedVectorType::operator[] ),
            pybind11::arg("index")
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::utils::SortedVector& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            py::keep_alive<0, 1>()
        )
    ;
}
