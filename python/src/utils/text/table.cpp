/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/utils/text/table.hpp"

using namespace text;

PYTHON_EXPORT_CLASS (Table, "utils.text")
{
    // -------------------------------------------------------------------
    //     Class Column
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::text::Table::Column > ( "Column", boost::python::init<  >(  ) )
        .def( boost::python::init< std::string >(( boost::python::arg("label") )) )
        // .def( boost::python::init< std::string, boost::python::optional< Justification > >(( boost::python::arg("label"), boost::python::arg("justify") )) )
        // .def( boost::python::init< Column const & >(( boost::python::arg("") )) )
        // .def( boost::python::init< Column && >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "append",
            ( void ( ::genesis::text::Table::Column::* )( std::string ))( &::genesis::text::Table::Column::append ),
            ( boost::python::arg("value") )
        )
        .def(
            "clear_content",
            ( void ( ::genesis::text::Table::Column::* )(  ))( &::genesis::text::Table::Column::clear_content )
        )
        // .def(
        //     "justify",
        //     ( Justification ( ::genesis::text::Table::Column::* )(  ) const )( &::genesis::text::Table::Column::justify )
        // )
        // .def(
        //     "justify",
        //     ( void ( ::genesis::text::Table::Column::* )( Justification ))( &::genesis::text::Table::Column::justify ),
        //     ( boost::python::arg("value") )
        // )
        .def(
            "label",
            ( std::string ( ::genesis::text::Table::Column::* )(  ) const )( &::genesis::text::Table::Column::label )
        )
        .def(
            "label",
            ( void ( ::genesis::text::Table::Column::* )( std::string ))( &::genesis::text::Table::Column::label ),
            ( boost::python::arg("value") )
        )
        .def(
            "length",
            ( size_t ( ::genesis::text::Table::Column::* )(  ) const )( &::genesis::text::Table::Column::length )
        )
        .def(
            "row",
            ( std::string ( ::genesis::text::Table::Column::* )( size_t ) const )( &::genesis::text::Table::Column::row ),
            ( boost::python::arg("i") )
        )
        .def(
            "shrink_width",
            ( void ( ::genesis::text::Table::Column::* )(  ))( &::genesis::text::Table::Column::shrink_width ),
            get_docstring("void ::genesis::text::Table::Column::shrink_width ()")
        )
        .def(
            "width",
            ( size_t ( ::genesis::text::Table::Column::* )(  ) const )( &::genesis::text::Table::Column::width )
        )
        .def(
            "width",
            ( void ( ::genesis::text::Table::Column::* )( size_t ))( &::genesis::text::Table::Column::width ),
            ( boost::python::arg("value") ),
            get_docstring("void ::genesis::text::Table::Column::width (size_t value)")
        )
        .def(
            "write_label",
            ( void ( ::genesis::text::Table::Column::* )( std::ostream & ) const )( &::genesis::text::Table::Column::write_label ),
            ( boost::python::arg("stream") )
        )
        .def(
            "write_row",
            ( void ( ::genesis::text::Table::Column::* )( std::ostream &, size_t ) const )( &::genesis::text::Table::Column::write_row ),
            ( boost::python::arg("stream"), boost::python::arg("row") )
        )

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Class Table
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::text::Table > ( "Table", boost::python::init<  >(  ) )
        // .def( boost::python::init< Table const & >(( boost::python::arg("") )) )
        // .def( boost::python::init< Table && >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "add_column",
            ( Table::Column & ( ::genesis::text::Table::* )( std::string ))( &::genesis::text::Table::add_column ),
            ( boost::python::arg("label")=(std::string)("") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("Column & ::genesis::text::Table::add_column (std::string label="")")
        )
        .def(
            "append",
            ( Table & ( ::genesis::text::Table::* )( std::string ))( &::genesis::text::Table::append ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            ( boost::python::arg("value") )
        )
        .def(
            "clear",
            ( void ( ::genesis::text::Table::* )(  ))( &::genesis::text::Table::clear ),
            get_docstring("void ::genesis::text::Table::clear ()")
        )
        .def(
            "clear_content",
            ( void ( ::genesis::text::Table::* )(  ))( &::genesis::text::Table::clear_content ),
            get_docstring("void ::genesis::text::Table::clear_content ()")
        )
        .def(
            "length",
            ( size_t ( ::genesis::text::Table::* )(  ) const )( &::genesis::text::Table::length )
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::text::Table::* )(  ) const )( &::genesis::text::Table::to_string )
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::text::Table::* )( Layout const & ) const )( &::genesis::text::Table::to_string ),
            ( boost::python::arg("layout") )
        )
        .def(
            "write",
            ( void ( ::genesis::text::Table::* )( std::ostream & ) const )( &::genesis::text::Table::write ),
            ( boost::python::arg("out") )
        )
        .def(
            "write",
            ( void ( ::genesis::text::Table::* )( std::ostream &, Layout const & ) const )( &::genesis::text::Table::write ),
            ( boost::python::arg("out"), boost::python::arg("layout") )
        )

        // Operators

        .def( boost::python::self_ns::str( boost::python::self ) )
    ;

    // -------------------------------------------------------------------
    //     Class Layout
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::text::Layout > ( "Layout" )

        // Operators

    ;

    // -------------------------------------------------------------------
    //     Default Table Layouts
    // -------------------------------------------------------------------

    boost::python::def(
        "minimal_layout",
        minimal_layout
    );

    boost::python::def(
        "simple_layout",
        simple_layout
    );
    boost::python::def(
        "simple_grid",
        simple_grid
    );
    boost::python::def(
        "simple_frame",
        simple_frame
    );

    boost::python::def(
        "extended_grid",
        extended_grid
    );
    boost::python::def(
        "simple_frame",
        simple_frame
    );

    boost::python::def(
        "double_grid",
        double_grid
    );
    boost::python::def(
        "double_frame",
        double_frame
    );
}
