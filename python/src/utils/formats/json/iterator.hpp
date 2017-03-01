/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"
/*
template <typename U>
void PythonExportClass_::genesis::utils::JsonIterator(std::string name)
{

    // -------------------------------------------------------------------
    //     Class JsonIterator
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using JsonIteratorType = JsonIterator<typename U>;

    pybind11::class_< JsonIteratorType, std::shared_ptr<JsonIteratorType> > ( scope, name.c_str() )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< pointer >(),
            pybind11::arg("object"),
            get_docstring("::genesis::utils::JsonIterator::JsonIterator (pointer object)")
        )
        .def(
            pybind11::init< const JsonIterator & >(),
            pybind11::arg("other"),
            get_docstring("::genesis::utils::JsonIterator::JsonIterator (const JsonIterator & other)")
        )

        // Public Member Functions

        .def(
            "key",
            ( JsonDocument::ObjectType::key_type ( JsonIteratorType::* )(  ) const )( &JsonIteratorType::key ),
            get_docstring("JsonDocument::ObjectType::key_type ::genesis::utils::JsonIterator::key () const")
        )
        .def(
            "value",
            ( reference ( JsonIteratorType::* )(  ) const )( &JsonIteratorType::value ),
            get_docstring("reference ::genesis::utils::JsonIterator::value () const")
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( +pybind11::self )
        .def( pybind11::self += pybind11::self )
        .def( -pybind11::self )
        .def( -pybind11::self )
        .def( pybind11::self -= pybind11::self )
        .def( pybind11::self < pybind11::self )
        .def( pybind11::self <= pybind11::self )
        .def( pybind11::self == pybind11::self )
        .def( pybind11::self > pybind11::self )
        .def( pybind11::self >= pybind11::self )
        .def(
            "__getitem__",
            ( reference ( JsonIteratorType::* )( difference_type ) const )( &JsonIteratorType::operator[] ),
            pybind11::arg("n"),
            get_docstring("reference ::genesis::utils::JsonIterator::operator[] (difference_type n) const")
        )
    ;
}
*/
