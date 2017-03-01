/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_CLASS( ::genesis::sequence::PrinterSimple, scope )
{

    // -------------------------------------------------------------------
    //     Class PrinterSimple
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::sequence::PrinterSimple, std::shared_ptr<::genesis::sequence::PrinterSimple> > ( scope, "PrinterSimple" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< PrinterSimple const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "color_map",
            ( PrinterSimple & ( ::genesis::sequence::PrinterSimple::* )( std::map< char, std::string > ))( &::genesis::sequence::PrinterSimple::color_map ),
            pybind11::arg("value")
        )
        .def(
            "color_map",
            ( std::map< char, std::string > ( ::genesis::sequence::PrinterSimple::* )(  ) const )( &::genesis::sequence::PrinterSimple::color_map )
        )
        // .def(
        //     "color_mode",
        //     ( ColorMode ( ::genesis::sequence::PrinterSimple::* )(  ) const )( &::genesis::sequence::PrinterSimple::color_mode )
        // )
        // .def(
        //     "color_mode",
        //     ( PrinterSimple & ( ::genesis::sequence::PrinterSimple::* )( ColorMode ))( &::genesis::sequence::PrinterSimple::color_mode ),
        //     pybind11::arg("value")
        // )
        // .def(
        //     "label_mode",
        //     ( LabelMode ( ::genesis::sequence::PrinterSimple::* )(  ) const )( &::genesis::sequence::PrinterSimple::label_mode )
        // )
        // .def(
        //     "label_mode",
        //     ( PrinterSimple & ( ::genesis::sequence::PrinterSimple::* )( LabelMode ))( &::genesis::sequence::PrinterSimple::label_mode ),
        //     pybind11::arg("value")
        // )
        .def(
            "length_limit",
            ( PrinterSimple & ( ::genesis::sequence::PrinterSimple::* )( size_t ))( &::genesis::sequence::PrinterSimple::length_limit ),
            pybind11::arg("value")
        )
        .def(
            "length_limit",
            ( size_t ( ::genesis::sequence::PrinterSimple::* )(  ) const )( &::genesis::sequence::PrinterSimple::length_limit )
        )
        .def(
            "line_length",
            ( PrinterSimple & ( ::genesis::sequence::PrinterSimple::* )( size_t ))( &::genesis::sequence::PrinterSimple::line_length ),
            pybind11::arg("value")
        )
        .def(
            "line_length",
            ( size_t ( ::genesis::sequence::PrinterSimple::* )(  ) const )( &::genesis::sequence::PrinterSimple::line_length )
        )
        .def(
            "pprint",
            ( std::string ( ::genesis::sequence::PrinterSimple::* )( Sequence const & ) const )( &::genesis::sequence::PrinterSimple::print ),
            pybind11::arg("seq")
        )
        .def(
            "pprint",
            ( std::string ( ::genesis::sequence::PrinterSimple::* )( SequenceSet const & ) const )( &::genesis::sequence::PrinterSimple::print ),
            pybind11::arg("set")
        )
        // .def(
        //     "print",
        //     ( void ( ::genesis::sequence::PrinterSimple::* )( std::ostream &, Sequence const & ) const )( &::genesis::sequence::PrinterSimple::print ),
        //     pybind11::arg("out"),
        //     pybind11::arg("seq")
        // )
        // .def(
        //     "print",
        //     ( void ( ::genesis::sequence::PrinterSimple::* )( std::ostream &, SequenceSet const & ) const )( &::genesis::sequence::PrinterSimple::print ),
        //     pybind11::arg("out"),
        //     pybind11::arg("set")
        // )
        .def(
            "sequence_limit",
            ( PrinterSimple & ( ::genesis::sequence::PrinterSimple::* )( size_t ))( &::genesis::sequence::PrinterSimple::sequence_limit ),
            pybind11::arg("value")
        )
        .def(
            "sequence_limit",
            ( size_t ( ::genesis::sequence::PrinterSimple::* )(  ) const )( &::genesis::sequence::PrinterSimple::sequence_limit )
        )
    ;
}
