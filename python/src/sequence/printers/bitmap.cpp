/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_CLASS( ::genesis::sequence::PrinterBitmap, scope )
{

    // -------------------------------------------------------------------
    //     Class PrinterBitmap
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::sequence::PrinterBitmap, std::shared_ptr<::genesis::sequence::PrinterBitmap> > ( scope, "PrinterBitmap" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< PrinterBitmap const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "color_map",
            ( PrinterBitmap & ( ::genesis::sequence::PrinterBitmap::* )( std::map< char, utils::Color > ))( &::genesis::sequence::PrinterBitmap::color_map ),
            pybind11::arg("value"),
            get_docstring("PrinterBitmap & ::genesis::sequence::PrinterBitmap::color_map (std::map< char, utils::Color > value)")
        )
        .def(
            "color_map",
            ( std::map< char, utils::Color > const & ( ::genesis::sequence::PrinterBitmap::* )(  ) const )( &::genesis::sequence::PrinterBitmap::color_map ),
            get_docstring("std::map< char, utils::Color > const & ::genesis::sequence::PrinterBitmap::color_map () const")
        )
        .def(
            "pixel_height_per_char",
            ( PrinterBitmap & ( ::genesis::sequence::PrinterBitmap::* )( size_t ))( &::genesis::sequence::PrinterBitmap::pixel_height_per_char ),
            pybind11::arg("value")
        )
        .def(
            "pixel_height_per_char",
            ( size_t ( ::genesis::sequence::PrinterBitmap::* )(  ) const )( &::genesis::sequence::PrinterBitmap::pixel_height_per_char )
        )
        .def(
            "pixel_width_per_char",
            ( PrinterBitmap & ( ::genesis::sequence::PrinterBitmap::* )( size_t ))( &::genesis::sequence::PrinterBitmap::pixel_width_per_char ),
            pybind11::arg("value")
        )
        .def(
            "pixel_width_per_char",
            ( size_t ( ::genesis::sequence::PrinterBitmap::* )(  ) const )( &::genesis::sequence::PrinterBitmap::pixel_width_per_char )
        )
        .def(
            "to_file",
            ( void ( ::genesis::sequence::PrinterBitmap::* )( SequenceSet const &, std::string const & ) const )( &::genesis::sequence::PrinterBitmap::to_file ),
            pybind11::arg("set"),
            pybind11::arg("filename")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::sequence::PrinterBitmap::* )( SequenceSet const &, std::ostream & ) const )( &::genesis::sequence::PrinterBitmap::to_stream ),
            pybind11::arg("set"),
            pybind11::arg("outstream")
        )
    ;
}
