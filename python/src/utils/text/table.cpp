/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS (Table, "utils")
{

    // -------------------------------------------------------------------
    //     Class Table
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::Table > ( "Table", boost::python::init<  >(  ) )
        .def( boost::python::init< Table const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "add_column",
            ( ::genesis::utils::Table::Column & ( ::genesis::utils::Table::* )( std::string ))( &::genesis::utils::Table::add_column ),
            ( boost::python::arg("label")=(std::string)("") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("Column & ::genesis::utils::Table::add_column (std::string label="")")
        )
        .def(
            "append",
            ( Table & ( ::genesis::utils::Table::* )( Style const &, std::string ))( &::genesis::utils::Table::append ),
            ( boost::python::arg("style"), boost::python::arg("value") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "append",
            ( Table & ( ::genesis::utils::Table::* )( std::string ))( &::genesis::utils::Table::append ),
            ( boost::python::arg("value") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
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
}

PYTHON_EXPORT_CLASS (TableLayout, "utils")
{

    // -------------------------------------------------------------------
    //     Class TableLayout
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::TableLayout > ( "TableLayout" )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_text_table_export, "utils")
{

    boost::python::def(
        "double_frame",
        ( TableLayout ( * )( bool ))( &::genesis::utils::double_frame ),
        ( boost::python::arg("condensed") )
    );

    boost::python::def(
        "double_grid",
        ( TableLayout ( * )( bool ))( &::genesis::utils::double_grid ),
        ( boost::python::arg("condensed") )
    );

    boost::python::def(
        "extended_frame",
        ( TableLayout ( * )( bool ))( &::genesis::utils::extended_frame ),
        ( boost::python::arg("condensed") )
    );

    boost::python::def(
        "extended_grid",
        ( TableLayout ( * )( bool ))( &::genesis::utils::extended_grid ),
        ( boost::python::arg("condensed") )
    );

    boost::python::def(
        "minimal_layout",
        ( TableLayout ( * )(  ))( &::genesis::utils::minimal_layout )
    );

    boost::python::def(
        "simple_frame",
        ( TableLayout ( * )( bool ))( &::genesis::utils::simple_frame ),
        ( boost::python::arg("condensed") )
    );

    boost::python::def(
        "simple_grid",
        ( TableLayout ( * )( bool ))( &::genesis::utils::simple_grid ),
        ( boost::python::arg("condensed") )
    );

    boost::python::def(
        "simple_layout",
        ( TableLayout ( * )( bool ))( &::genesis::utils::simple_layout ),
        ( boost::python::arg("condensed") )
    );

    // boost::python::def(
    //     "operator<<",
    //     ( std::ostream & ( * )( std::ostream &, Table const & ))( &::genesis::utils::operator<< ),
    //     ( boost::python::arg("out"), boost::python::arg("table") ),
    //     boost::python::return_value_policy<boost::python::reference_existing_object>()
    // );

    // boost::python::def(
    //     "operator<<",
    //     ( std::ostream & ( * )( std::ostream &, TableLayout::Binder const & ))( &::genesis::utils::operator<< ),
    //     ( boost::python::arg("out"), boost::python::arg("binder") ),
    //     boost::python::return_value_policy<boost::python::reference_existing_object>()
    // );
}
