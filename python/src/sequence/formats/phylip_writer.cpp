/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_CLASS( ::genesis::sequence::PhylipWriter, scope )
{

    // -------------------------------------------------------------------
    //     Class PhylipWriter
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::sequence::PhylipWriter, std::shared_ptr<::genesis::sequence::PhylipWriter> > ( scope, "PhylipWriter" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< PhylipWriter const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "label_length",
            ( PhylipWriter & ( ::genesis::sequence::PhylipWriter::* )( size_t ))( &::genesis::sequence::PhylipWriter::label_length ),
            pybind11::arg("value")
        )
        .def(
            "label_length",
            ( size_t ( ::genesis::sequence::PhylipWriter::* )(  ) const )( &::genesis::sequence::PhylipWriter::label_length )
        )
        .def(
            "line_length",
            ( PhylipWriter & ( ::genesis::sequence::PhylipWriter::* )( size_t ))( &::genesis::sequence::PhylipWriter::line_length ),
            pybind11::arg("value")
        )
        .def(
            "line_length",
            ( size_t ( ::genesis::sequence::PhylipWriter::* )(  ) const )( &::genesis::sequence::PhylipWriter::line_length )
        )
        .def(
            "to_file",
            ( void ( ::genesis::sequence::PhylipWriter::* )( SequenceSet const &, std::string const & ) const )( &::genesis::sequence::PhylipWriter::to_file ),
            pybind11::arg("sset"),
            pybind11::arg("fn")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::sequence::PhylipWriter::* )( SequenceSet const &, std::ostream & ) const )( &::genesis::sequence::PhylipWriter::to_stream ),
            pybind11::arg("sset"),
            pybind11::arg("os")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::sequence::PhylipWriter::* )( SequenceSet const & ) const )( &::genesis::sequence::PhylipWriter::to_string ),
            pybind11::arg("sset")
        )
    ;
}
