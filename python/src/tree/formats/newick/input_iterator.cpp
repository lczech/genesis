/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::NewickInputIterator, scope )
{

    // -------------------------------------------------------------------
    //     Class NewickInputIterator
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::NewickInputIterator, std::shared_ptr<::genesis::tree::NewickInputIterator> > ( scope, "NewickInputIterator" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< utils::InputStream & >(),
            pybind11::arg("in")
        )
        .def(
            pybind11::init< utils::InputStream &, NewickReader const & >(),
            pybind11::arg("in"),
            pybind11::arg("reader")
        )
        // .def(
        //     pybind11::init< ::genesis::tree::NewickInputIterator const & >(),
        //     pybind11::arg("arg")
        // )
        // .def(
        //     pybind11::init< ::genesis::tree::NewickInputIterator && >(),
        //     pybind11::arg("arg")
        // )

        // Public Member Functions

        .def(
            "dereference",
            ( Tree const & ( ::genesis::tree::NewickInputIterator::* )(  ) const )( &::genesis::tree::NewickInputIterator::dereference )
        )
        .def(
            "increment",
            ( void ( ::genesis::tree::NewickInputIterator::* )(  ))( &::genesis::tree::NewickInputIterator::increment )
        )
        .def(
            "reader",
            ( NewickReader & ( ::genesis::tree::NewickInputIterator::* )(  ))( &::genesis::tree::NewickInputIterator::reader ),
            get_docstring("NewickReader & ::genesis::tree::NewickInputIterator::reader ()")
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
    ;
}
