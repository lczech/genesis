/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_CLASS( ::genesis::sequence::FastaInputIterator, scope )
{

    // -------------------------------------------------------------------
    //     Class FastaInputIterator
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::sequence::FastaInputIterator, std::shared_ptr<::genesis::sequence::FastaInputIterator> > ( scope, "FastaInputIterator" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< utils::InputStream & >(),
            pybind11::arg("in")
        )
        .def(
            pybind11::init< utils::InputStream &, FastaReader const & >(),
            pybind11::arg("in"),
            pybind11::arg("reader")
        )
        // .def(
        //     pybind11::init< self_type const & >(),
        //     pybind11::arg("arg")
        // )
        // .def(
        //     pybind11::init< self_type && >(),
        //     pybind11::arg("arg")
        // )

        // Public Member Functions

        .def(
            "dereference",
            ( Sequence const & ( ::genesis::sequence::FastaInputIterator::* )(  ) const )( &::genesis::sequence::FastaInputIterator::dereference )
        )
        .def(
            "increment",
            ( void ( ::genesis::sequence::FastaInputIterator::* )(  ))( &::genesis::sequence::FastaInputIterator::increment )
        )
        .def(
            "reader",
            ( FastaReader & ( ::genesis::sequence::FastaInputIterator::* )(  ))( &::genesis::sequence::FastaInputIterator::reader ),
            get_docstring("FastaReader & ::genesis::sequence::FastaInputIterator::reader ()")
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
    ;
}
