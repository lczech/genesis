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

/*
PYTHON_EXPORT_CLASS (XmlElement, "utils")
{

    // -------------------------------------------------------------------
    //     Class XmlElement
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::XmlElement > ( "XmlElement", boost::python::init<  >(  ) )
        .def( boost::python::init< const std::string & >(( boost::python::arg("tag") )) )

        // Public Member Functions

        .def(
            "append_comment",
            ( XmlComment * ( ::genesis::utils::XmlElement::* )( const std::string & ))( &::genesis::utils::XmlElement::append_comment ),
            ( boost::python::arg("comment") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "append_element",
            ( XmlElement * ( ::genesis::utils::XmlElement::* )( const std::string & ))( &::genesis::utils::XmlElement::append_element ),
            ( boost::python::arg("tag") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "append_markup",
            ( XmlMarkup * ( ::genesis::utils::XmlElement::* )( const std::string & ))( &::genesis::utils::XmlElement::append_markup ),
            ( boost::python::arg("text") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::XmlElement::* )(  ))( &::genesis::utils::XmlElement::clear )
        )
    ;
}

PYTHON_EXPORT_CLASS (XmlMarkup, "utils")
{

    // -------------------------------------------------------------------
    //     Class XmlMarkup
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::XmlMarkup > ( "XmlMarkup", boost::python::init<  >(  ) )
        .def( boost::python::init< const std::string & >(( boost::python::arg("content") )) )
    ;
}

PYTHON_EXPORT_CLASS (XmlDocument, "utils")
{

    // -------------------------------------------------------------------
    //     Class XmlDocument
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::XmlDocument > ( "XmlDocument" )

        // Public Member Functions

        .def(
            "clear",
            ( void ( ::genesis::utils::XmlDocument::* )(  ))( &::genesis::utils::XmlDocument::clear )
        )
    ;
}

PYTHON_EXPORT_CLASS (XmlComment, "utils")
{

    // -------------------------------------------------------------------
    //     Class XmlComment
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::XmlComment > ( "XmlComment", boost::python::init<  >(  ) )
        .def( boost::python::init< const std::string & >(( boost::python::arg("comment") )) )
    ;
}

PYTHON_EXPORT_CLASS (XmlValue, "utils")
{

    // -------------------------------------------------------------------
    //     Class XmlValue
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::XmlValue > ( "XmlValue" )

        // Public Member Functions

        .def(
            "is_comment",
            ( bool ( ::genesis::utils::XmlValue::* )(  ))( &::genesis::utils::XmlValue::is_comment )
        )
        .def(
            "is_element",
            ( bool ( ::genesis::utils::XmlValue::* )(  ))( &::genesis::utils::XmlValue::is_element )
        )
        .def(
            "is_markup",
            ( bool ( ::genesis::utils::XmlValue::* )(  ))( &::genesis::utils::XmlValue::is_markup )
        )
        .def(
            "type",
            ( Type ( ::genesis::utils::XmlValue::* )(  ) const )( &::genesis::utils::XmlValue::type )
        )
        .def(
            "type_to_string",
            ( std::string ( * )( const Type ))( &::genesis::utils::XmlValue::type_to_string ),
            ( boost::python::arg("t") )
        )
        .staticmethod("type_to_string")
        .def(
            "type_to_string",
            ( std::string ( ::genesis::utils::XmlValue::* )(  ) const )( &::genesis::utils::XmlValue::type_to_string )
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_io_xml_document_export, "utils")
{

    boost::python::def(
        "xml_value_to_comment",
        ( const XmlComment * ( * )( const XmlValue * ))( &::genesis::utils::xml_value_to_comment ),
        ( boost::python::arg("v") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("const XmlComment * ::genesis::utils::xml_value_to_comment (const XmlValue * v)")
    );

    boost::python::def(
        "xml_value_to_element",
        ( const XmlElement * ( * )( const XmlValue * ))( &::genesis::utils::xml_value_to_element ),
        ( boost::python::arg("v") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("const XmlElement * ::genesis::utils::xml_value_to_element (const XmlValue * v)")
    );

    boost::python::def(
        "xml_value_to_markup",
        ( const XmlMarkup * ( * )( const XmlValue * ))( &::genesis::utils::xml_value_to_markup ),
        ( boost::python::arg("v") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("const XmlMarkup * ::genesis::utils::xml_value_to_markup (const XmlValue * v)")
    );
}
*/
