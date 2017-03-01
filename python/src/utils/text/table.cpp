/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::Table, scope )
{

    // -------------------------------------------------------------------
    //     Class Table
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::Table, std::shared_ptr<::genesis::utils::Table> > ( scope, "Table" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< Table const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "add_column",
            ( ::genesis::utils::Table::Column & ( ::genesis::utils::Table::* )( std::string ))( &::genesis::utils::Table::add_column ),
            pybind11::arg("label")=(std::string)("")
        )
        .def(
            "append",
            ( Table & ( ::genesis::utils::Table::* )( Style const &, std::string ))( &::genesis::utils::Table::append ),
            pybind11::arg("style"),
            pybind11::arg("value")
        )
        .def(
            "append",
            ( Table & ( ::genesis::utils::Table::* )( std::string ))( &::genesis::utils::Table::append ),
            pybind11::arg("value")
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::Table::* )(  ))( &::genesis::utils::Table::clear )
        )
        .def(
            "clear_content",
            ( void ( ::genesis::utils::Table::* )(  ))( &::genesis::utils::Table::clear_content )
        )
        .def(
            "length",
            ( size_t ( ::genesis::utils::Table::* )(  ) const )( &::genesis::utils::Table::length )
        )
        .def(
            "line_break",
            ( Table & ( ::genesis::utils::Table::* )(  ))( &::genesis::utils::Table::line_break )
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::Table::* )(  ) const )( &::genesis::utils::Table::to_string )
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::Table::* )( TableLayout const & ) const )( &::genesis::utils::Table::to_string ),
            pybind11::arg("layout")
        )
        .def(
            "write",
            ( void ( ::genesis::utils::Table::* )( std::ostream & ) const )( &::genesis::utils::Table::write ),
            pybind11::arg("out")
        )
        .def(
            "write",
            ( void ( ::genesis::utils::Table::* )( std::ostream &, TableLayout const & ) const )( &::genesis::utils::Table::write ),
            pybind11::arg("out"),
            pybind11::arg("layout")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::utils::Table const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
        .def(
            "__str__",
            []( ::genesis::utils::Table const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::TableLayout, scope )
{

    // -------------------------------------------------------------------
    //     Class TableLayout
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::TableLayout, std::shared_ptr<::genesis::utils::TableLayout> > ( scope, "TableLayout" )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_text_table_export, ::genesis::utils, scope )
{

    scope.def(
        "double_frame",
        ( TableLayout ( * )( bool ))( &::genesis::utils::double_frame ),
            pybind11::arg("condensed")=(bool)(false)
    );

    scope.def(
        "double_grid",
        ( TableLayout ( * )( bool ))( &::genesis::utils::double_grid ),
            pybind11::arg("condensed")=(bool)(false)
    );

    scope.def(
        "extended_frame",
        ( TableLayout ( * )( bool ))( &::genesis::utils::extended_frame ),
            pybind11::arg("condensed")=(bool)(false)
    );

    scope.def(
        "extended_grid",
        ( TableLayout ( * )( bool ))( &::genesis::utils::extended_grid ),
            pybind11::arg("condensed")=(bool)(false)
    );

    scope.def(
        "minimal_layout",
        ( TableLayout ( * )(  ))( &::genesis::utils::minimal_layout )
    );

    scope.def(
        "simple_frame",
        ( TableLayout ( * )( bool ))( &::genesis::utils::simple_frame ),
            pybind11::arg("condensed")=(bool)(false)
    );

    scope.def(
        "simple_grid",
        ( TableLayout ( * )( bool ))( &::genesis::utils::simple_grid ),
            pybind11::arg("condensed")=(bool)(false)
    );

    scope.def(
        "simple_layout",
        ( TableLayout ( * )( bool ))( &::genesis::utils::simple_layout ),
            pybind11::arg("condensed")=(bool)(false)
    );
}
