/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/utils/text/table.hpp"

using namespace utils;

PYTHON_EXPORT_CLASS (Table, "utils.text")
{
    // -------------------------------------------------------------------
    //     Class Column
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::Table::Column > ( "Column", boost::python::init<  >(  ) )
        .def( boost::python::init< std::string >(( boost::python::arg("label") )) )
        // .def( boost::python::init< std::string, boost::python::optional< Justification > >(( boost::python::arg("label"), boost::python::arg("justify") )) )
        // .def( boost::python::init< Column const & >(( boost::python::arg("") )) )
        // .def( boost::python::init< Column && >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "append",
            ( void ( ::genesis::utils::Table::Column::* )( std::string ))( &::genesis::utils::Table::Column::append ),
            ( boost::python::arg("value") )
        )
        .def(
            "clear_content",
            ( void ( ::genesis::utils::Table::Column::* )(  ))( &::genesis::utils::Table::Column::clear_content )
        )
        // .def(
        //     "justify",
        //     ( Justification ( ::genesis::utils::Table::Column::* )(  ) const )( &::genesis::utils::Table::Column::justify )
        // )
        // .def(
        //     "justify",
        //     ( void ( ::genesis::utils::Table::Column::* )( Justification ))( &::genesis::utils::Table::Column::justify ),
        //     ( boost::python::arg("value") )
        // )
        .def(
            "label",
            ( std::string ( ::genesis::utils::Table::Column::* )(  ) const )( &::genesis::utils::Table::Column::label )
        )
        .def(
            "label",
            ( void ( ::genesis::utils::Table::Column::* )( std::string ))( &::genesis::utils::Table::Column::label ),
            ( boost::python::arg("value") )
        )
        .def(
            "length",
            ( size_t ( ::genesis::utils::Table::Column::* )(  ) const )( &::genesis::utils::Table::Column::length )
        )
        .def(
            "row",
            ( std::string ( ::genesis::utils::Table::Column::* )( size_t ) const )( &::genesis::utils::Table::Column::row ),
            ( boost::python::arg("i") )
        )
        .def(
            "shrink_width",
            ( void ( ::genesis::utils::Table::Column::* )(  ))( &::genesis::utils::Table::Column::shrink_width ),
            get_docstring("void ::genesis::utils::Table::Column::shrink_width ()")
        )
        .def(
            "width",
            ( size_t ( ::genesis::utils::Table::Column::* )(  ) const )( &::genesis::utils::Table::Column::width )
        )
        .def(
            "width",
            ( void ( ::genesis::utils::Table::Column::* )( size_t ))( &::genesis::utils::Table::Column::width ),
            ( boost::python::arg("value") ),
            get_docstring("void ::genesis::utils::Table::Column::width (size_t value)")
        )
        .def(
            "write_label",
            ( void ( ::genesis::utils::Table::Column::* )( std::ostream & ) const )( &::genesis::utils::Table::Column::write_label ),
            ( boost::python::arg("stream") )
        )
        .def(
            "write_row",
            ( void ( ::genesis::utils::Table::Column::* )( std::ostream &, size_t ) const )( &::genesis::utils::Table::Column::write_row ),
            ( boost::python::arg("stream"), boost::python::arg("row") )
        )

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Class Table
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::Table > ( "Table", boost::python::init<  >(  ) )
        // .def( boost::python::init< Table const & >(( boost::python::arg("") )) )
        // .def( boost::python::init< Table && >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "add_column",
            ( Table::Column & ( ::genesis::utils::Table::* )( std::string ))( &::genesis::utils::Table::add_column ),
            ( boost::python::arg("label")=(std::string)("") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("Column & ::genesis::utils::Table::add_column (std::string label="")")
        )
        .def(
            "append",
            ( Table & ( ::genesis::utils::Table::* )( std::string ))( &::genesis::utils::Table::append ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            ( boost::python::arg("value") )
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::Table::* )(  ))( &::genesis::utils::Table::clear ),
            get_docstring("void ::genesis::utils::Table::clear ()")
        )
        .def(
            "clear_content",
            ( void ( ::genesis::utils::Table::* )(  ))( &::genesis::utils::Table::clear_content ),
            get_docstring("void ::genesis::utils::Table::clear_content ()")
        )
        .def(
            "length",
            ( size_t ( ::genesis::utils::Table::* )(  ) const )( &::genesis::utils::Table::length )
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::Table::* )(  ) const )( &::genesis::utils::Table::to_string )
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::Table::* )( TableLayout const & ) const )( &::genesis::utils::Table::to_string ),
            ( boost::python::arg("layout") )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::Table::* )( std::ostream & ) const )( &::genesis::utils::Table::write ),
            ( boost::python::arg("out") )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::Table::* )( std::ostream &, TableLayout const & ) const )( &::genesis::utils::Table::write ),
            ( boost::python::arg("out"), boost::python::arg("layout") )
        )

        // Operators

        .def( boost::python::self_ns::str( boost::python::self ) )
    ;

    // -------------------------------------------------------------------
    //     Class TableLayout
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::TableLayout > ( "TableLayout" )

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Default Table TableLayouts
    // -------------------------------------------------------------------

    boost::python::def(
        "minimal_layout",
        minimal_layout
    );

    boost::python::def(
        "simple_layout",
        simple_layout,
        boost::python::arg("condensed")=(bool)(false)
    );
    boost::python::def(
        "simple_grid",
        simple_grid,
        boost::python::arg("condensed")=(bool)(false)
    );
    boost::python::def(
        "simple_frame",
        simple_frame,
        boost::python::arg("condensed")=(bool)(false)
    );

    boost::python::def(
        "extended_grid",
        extended_grid,
        boost::python::arg("condensed")=(bool)(false)
    );
    boost::python::def(
        "extended_frame",
        extended_frame,
        boost::python::arg("condensed")=(bool)(false)
    );

    boost::python::def(
        "double_grid",
        double_grid,
        boost::python::arg("condensed")=(bool)(false)
    );
    boost::python::def(
        "double_frame",
        double_frame,
        boost::python::arg("condensed")=(bool)(false)
    );
}
