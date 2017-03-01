/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::BmpWriter, scope )
{

    // -------------------------------------------------------------------
    //     Class BmpWriter
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::BmpWriter, std::shared_ptr<::genesis::utils::BmpWriter> > ( scope, "BmpWriter" )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::utils::BmpWriter::BmpWriter ()")
        )
        .def(
            pybind11::init< BmpWriter const & >(),
            pybind11::arg("arg"),
            get_docstring("::genesis::utils::BmpWriter::BmpWriter (BmpWriter const & )")
        )

        // Public Member Functions

        .def(
            "to_file",
            ( void ( ::genesis::utils::BmpWriter::* )( Matrix< Color > const &, std::string const & ) const )( &::genesis::utils::BmpWriter::to_file ),
            pybind11::arg("image"),
            pybind11::arg("filename"),
            get_docstring("void ::genesis::utils::BmpWriter::to_file (Matrix< Color > const & image, std::string const & filename) const")
        )
        .def(
            "to_file",
            ( void ( ::genesis::utils::BmpWriter::* )( Matrix< unsigned char > const &, std::string const & ) const )( &::genesis::utils::BmpWriter::to_file ),
            pybind11::arg("image"),
            pybind11::arg("filename"),
            get_docstring("void ::genesis::utils::BmpWriter::to_file (Matrix< unsigned char > const & image, std::string const & filename) const")
        )
        .def(
            "to_file",
            ( void ( ::genesis::utils::BmpWriter::* )( Matrix< unsigned char > const &, std::vector< Color >, std::string const & ) const )( &::genesis::utils::BmpWriter::to_file ),
            pybind11::arg("image"),
            pybind11::arg("palette"),
            pybind11::arg("filename"),
            get_docstring("void ::genesis::utils::BmpWriter::to_file (Matrix< unsigned char > const & image, std::vector< Color > palette, std::string const & filename) const")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::BmpWriter::* )( Matrix< Color > const &, std::ostream & ) const )( &::genesis::utils::BmpWriter::to_stream ),
            pybind11::arg("image"),
            pybind11::arg("outstream"),
            get_docstring("void ::genesis::utils::BmpWriter::to_stream (Matrix< Color > const & image, std::ostream & outstream) const")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::BmpWriter::* )( Matrix< unsigned char > const &, std::ostream & ) const )( &::genesis::utils::BmpWriter::to_stream ),
            pybind11::arg("image"),
            pybind11::arg("outstream"),
            get_docstring("void ::genesis::utils::BmpWriter::to_stream (Matrix< unsigned char > const & image, std::ostream & outstream) const")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::BmpWriter::* )( Matrix< unsigned char > const &, std::vector< Color >, std::ostream & ) const )( &::genesis::utils::BmpWriter::to_stream ),
            pybind11::arg("image"),
            pybind11::arg("palette"),
            pybind11::arg("outstream"),
            get_docstring("void ::genesis::utils::BmpWriter::to_stream (Matrix< unsigned char > const & image, std::vector< Color > palette, std::ostream & outstream) const")
        )
    ;
}
