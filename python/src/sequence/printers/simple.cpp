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
            ( PrinterSimple & ( ::genesis::sequence::PrinterSimple::* )( std::map< char, std::string > const & ))( &::genesis::sequence::PrinterSimple::color_map ),
            pybind11::arg("value"),
            get_docstring("PrinterSimple & ::genesis::sequence::PrinterSimple::color_map (std::map< char, std::string > const & value)")
        )
        .def(
            "color_map",
            ( std::map< char, std::string > const & ( ::genesis::sequence::PrinterSimple::* )(  ) const )( &::genesis::sequence::PrinterSimple::color_map ),
            get_docstring("std::map< char, std::string > const & ::genesis::sequence::PrinterSimple::color_map () const")
        )
        .def(
            "color_mode",
            ( ::genesis::sequence::PrinterSimple::ColorMode ( ::genesis::sequence::PrinterSimple::* )(  ) const )( &::genesis::sequence::PrinterSimple::color_mode ),
            get_docstring("ColorMode ::genesis::sequence::PrinterSimple::color_mode () const")
        )
        .def(
            "color_mode",
            ( PrinterSimple & ( ::genesis::sequence::PrinterSimple::* )( ::genesis::sequence::PrinterSimple::ColorMode ))( &::genesis::sequence::PrinterSimple::color_mode ),
            pybind11::arg("value"),
            get_docstring("PrinterSimple & ::genesis::sequence::PrinterSimple::color_mode (ColorMode value)")
        )
        .def(
            "label_mode",
            ( ::genesis::sequence::PrinterSimple::LabelMode ( ::genesis::sequence::PrinterSimple::* )(  ) const )( &::genesis::sequence::PrinterSimple::label_mode ),
            get_docstring("LabelMode ::genesis::sequence::PrinterSimple::label_mode () const")
        )
        .def(
            "label_mode",
            ( PrinterSimple & ( ::genesis::sequence::PrinterSimple::* )( ::genesis::sequence::PrinterSimple::LabelMode ))( &::genesis::sequence::PrinterSimple::label_mode ),
            pybind11::arg("value"),
            get_docstring("PrinterSimple & ::genesis::sequence::PrinterSimple::label_mode (LabelMode value)")
        )
        .def(
            "length_limit",
            ( PrinterSimple & ( ::genesis::sequence::PrinterSimple::* )( size_t ))( &::genesis::sequence::PrinterSimple::length_limit ),
            pybind11::arg("value"),
            get_docstring("PrinterSimple & ::genesis::sequence::PrinterSimple::length_limit (size_t value)")
        )
        .def(
            "length_limit",
            ( size_t ( ::genesis::sequence::PrinterSimple::* )(  ) const )( &::genesis::sequence::PrinterSimple::length_limit ),
            get_docstring("size_t ::genesis::sequence::PrinterSimple::length_limit () const")
        )
        .def(
            "line_length",
            ( PrinterSimple & ( ::genesis::sequence::PrinterSimple::* )( size_t ))( &::genesis::sequence::PrinterSimple::line_length ),
            pybind11::arg("value"),
            get_docstring("PrinterSimple & ::genesis::sequence::PrinterSimple::line_length (size_t value)")
        )
        .def(
            "line_length",
            ( size_t ( ::genesis::sequence::PrinterSimple::* )(  ) const )( &::genesis::sequence::PrinterSimple::line_length ),
            get_docstring("size_t ::genesis::sequence::PrinterSimple::line_length () const")
        )
        .def(
            "print",
            ( std::string ( ::genesis::sequence::PrinterSimple::* )( Sequence const & ) const )( &::genesis::sequence::PrinterSimple::print ),
            pybind11::arg("seq"),
            get_docstring("std::string ::genesis::sequence::PrinterSimple::print (Sequence const & seq) const")
        )
        .def(
            "print",
            ( std::string ( ::genesis::sequence::PrinterSimple::* )( SequenceSet const & ) const )( &::genesis::sequence::PrinterSimple::print ),
            pybind11::arg("set"),
            get_docstring("std::string ::genesis::sequence::PrinterSimple::print (SequenceSet const & set) const")
        )
        .def(
            "print",
            ( void ( ::genesis::sequence::PrinterSimple::* )( std::ostream &, Sequence const & ) const )( &::genesis::sequence::PrinterSimple::print ),
            pybind11::arg("out"),
            pybind11::arg("seq"),
            get_docstring("void ::genesis::sequence::PrinterSimple::print (std::ostream & out, Sequence const & seq) const")
        )
        .def(
            "print",
            ( void ( ::genesis::sequence::PrinterSimple::* )( std::ostream &, SequenceSet const & ) const )( &::genesis::sequence::PrinterSimple::print ),
            pybind11::arg("out"),
            pybind11::arg("set"),
            get_docstring("void ::genesis::sequence::PrinterSimple::print (std::ostream & out, SequenceSet const & set) const")
        )
        .def(
            "sequence_limit",
            ( PrinterSimple & ( ::genesis::sequence::PrinterSimple::* )( size_t ))( &::genesis::sequence::PrinterSimple::sequence_limit ),
            pybind11::arg("value"),
            get_docstring("PrinterSimple & ::genesis::sequence::PrinterSimple::sequence_limit (size_t value)")
        )
        .def(
            "sequence_limit",
            ( size_t ( ::genesis::sequence::PrinterSimple::* )(  ) const )( &::genesis::sequence::PrinterSimple::sequence_limit ),
            get_docstring("size_t ::genesis::sequence::PrinterSimple::sequence_limit () const")
        )
    ;
}
