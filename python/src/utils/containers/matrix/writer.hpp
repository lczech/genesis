/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename T>
void PythonExportClass_::genesis::utils::MatrixWriter(std::string name)
{

    // -------------------------------------------------------------------
    //     Class MatrixWriter
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using MatrixWriterType = MatrixWriter<typename T>;

    pybind11::class_< MatrixWriterType, std::shared_ptr<MatrixWriterType> > ( scope, name.c_str() )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< MatrixWriter const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "format",
            ( Format ( MatrixWriterType::* )(  ) const )( &MatrixWriterType::format )
        )
        .def(
            "format",
            ( MatrixWriter & ( MatrixWriterType::* )( Format ))( &MatrixWriterType::format ),
            pybind11::arg("value")
        )
        .def(
            "separator_string",
            ( MatrixWriter & ( MatrixWriterType::* )( std::string const & ))( &MatrixWriterType::separator_string ),
            pybind11::arg("value")
        )
        .def(
            "separator_string",
            ( std::string const & ( MatrixWriterType::* )(  ) const )( &MatrixWriterType::separator_string )
        )
        .def(
            "to_file",
            ( void ( MatrixWriterType::* )( Matrix< T > const &, std::string const &, std::vector< std::string > const &, std::vector< std::string > const &, std::string const & ) const )( &MatrixWriterType::to_file ),
            pybind11::arg("mat"),
            pybind11::arg("fn"),
            pybind11::arg("row_names")=(std::vector< std::string > const &)({}),
            pybind11::arg("col_names")=(std::vector< std::string > const &)({}),
            pybind11::arg("corner")=(std::string const &)("")
        )
        .def(
            "to_stream",
            ( void ( MatrixWriterType::* )( Matrix< T > const &, std::ostream &, std::vector< std::string > const &, std::vector< std::string > const &, std::string const & ) const )( &MatrixWriterType::to_stream ),
            pybind11::arg("mat"),
            pybind11::arg("os"),
            pybind11::arg("row_names")=(std::vector< std::string > const &)({}),
            pybind11::arg("col_names")=(std::vector< std::string > const &)({}),
            pybind11::arg("corner")=(std::string const &)("")
        )
        .def(
            "to_string",
            ( std::string ( MatrixWriterType::* )( Matrix< T > const &, std::vector< std::string > const &, std::vector< std::string > const &, std::string const & ) const )( &MatrixWriterType::to_string ),
            pybind11::arg("mat"),
            pybind11::arg("row_names")=(std::vector< std::string > const &)({}),
            pybind11::arg("col_names")=(std::vector< std::string > const &)({}),
            pybind11::arg("corner")=(std::string const &)("")
        )
        .def(
            "write_value_functor",
            ( MatrixWriter & ( MatrixWriterType::* )( std::function< std::string(T const &)> ))( &MatrixWriterType::write_value_functor ),
            pybind11::arg("functor")
        )
    ;
}
