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
            pybind11::init<  >(),
            get_docstring("::genesis::sequence::FastaInputIterator::FastaInputIterator ()")
        )
        .def(
            pybind11::init< FastaReader const & >(),
            pybind11::arg("reader"),
            get_docstring("::genesis::sequence::FastaInputIterator::FastaInputIterator (FastaReader const & reader)")
        )
        .def(
            pybind11::init< ::genesis::sequence::FastaInputIterator::self_type const & >(),
            pybind11::arg("arg")
        )
        // .def(
        //     pybind11::init< ::genesis::sequence::FastaInputIterator::self_type && >(),
        //     pybind11::arg("arg")
        // )

        // Public Member Functions

        .def(
            "dereference",
            ( ::genesis::sequence::FastaInputIterator::value_type const & ( ::genesis::sequence::FastaInputIterator::* )(  ) const )( &::genesis::sequence::FastaInputIterator::dereference )
        )
        .def(
            "from_file",
            ( ::genesis::sequence::FastaInputIterator::self_type & ( ::genesis::sequence::FastaInputIterator::* )( std::string const & ))( &::genesis::sequence::FastaInputIterator::from_file ),
            pybind11::arg("file_name"),
            get_docstring("self_type & ::genesis::sequence::FastaInputIterator::from_file (std::string const & file_name)")
        )
        .def(
            "from_stream",
            ( ::genesis::sequence::FastaInputIterator::self_type & ( ::genesis::sequence::FastaInputIterator::* )( std::istream & ))( &::genesis::sequence::FastaInputIterator::from_stream ),
            pybind11::arg("input_stream"),
            get_docstring("self_type & ::genesis::sequence::FastaInputIterator::from_stream (std::istream & input_stream)")
        )
        .def(
            "from_string",
            ( ::genesis::sequence::FastaInputIterator::self_type & ( ::genesis::sequence::FastaInputIterator::* )( std::string const & ))( &::genesis::sequence::FastaInputIterator::from_string ),
            pybind11::arg("input_string"),
            get_docstring("self_type & ::genesis::sequence::FastaInputIterator::from_string (std::string const & input_string)")
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
