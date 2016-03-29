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
PYTHON_EXPORT_CLASS (JsonValue, "utils")
{

    // -------------------------------------------------------------------
    //     Class JsonValue
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::JsonValue > ( "JsonValue" )

        // Public Member Functions

        .def(
            "is_array",
            ( bool ( ::genesis::utils::JsonValue::* )(  ))( &::genesis::utils::JsonValue::is_array )
        )
        .def(
            "is_bool",
            ( bool ( ::genesis::utils::JsonValue::* )(  ))( &::genesis::utils::JsonValue::is_bool )
        )
        .def(
            "is_null",
            ( bool ( ::genesis::utils::JsonValue::* )(  ))( &::genesis::utils::JsonValue::is_null )
        )
        .def(
            "is_number",
            ( bool ( ::genesis::utils::JsonValue::* )(  ))( &::genesis::utils::JsonValue::is_number )
        )
        .def(
            "is_object",
            ( bool ( ::genesis::utils::JsonValue::* )(  ))( &::genesis::utils::JsonValue::is_object )
        )
        .def(
            "is_string",
            ( bool ( ::genesis::utils::JsonValue::* )(  ))( &::genesis::utils::JsonValue::is_string )
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::JsonValue::* )(  ) const )( &::genesis::utils::JsonValue::to_string )
        )
        .def(
            "type",
            ( Type ( ::genesis::utils::JsonValue::* )(  ) const )( &::genesis::utils::JsonValue::type )
        )
        .def(
            "type_to_string",
            ( std::string ( * )( const Type ))( &::genesis::utils::JsonValue::type_to_string ),
            ( boost::python::arg("t") )
        )
        .staticmethod("type_to_string")
        .def(
            "type_to_string",
            ( std::string ( ::genesis::utils::JsonValue::* )(  ) const )( &::genesis::utils::JsonValue::type_to_string )
        )
    ;
}

PYTHON_EXPORT_CLASS (JsonValueNumber, "utils")
{

    // -------------------------------------------------------------------
    //     Class JsonValueNumber
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::JsonValueNumber > ( "JsonValueNumber", boost::python::init<  >(  ) )
        .def( boost::python::init< const double >(( boost::python::arg("v") )) )
        .def( boost::python::init< const std::string & >(( boost::python::arg("v") )) )

        // Public Member Functions

        .def(
            "to_string",
            ( std::string ( ::genesis::utils::JsonValueNumber::* )(  ) const )( &::genesis::utils::JsonValueNumber::to_string )
        )
    ;
}

PYTHON_EXPORT_CLASS (JsonValueString, "utils")
{

    // -------------------------------------------------------------------
    //     Class JsonValueString
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::JsonValueString > ( "JsonValueString", boost::python::init<  >(  ) )
        .def( boost::python::init< const std::string & >(( boost::python::arg("v") )) )

        // Public Member Functions

        .def(
            "to_string",
            ( std::string ( ::genesis::utils::JsonValueString::* )(  ) const )( &::genesis::utils::JsonValueString::to_string )
        )
    ;
}

PYTHON_EXPORT_CLASS (JsonValueBool, "utils")
{

    // -------------------------------------------------------------------
    //     Class JsonValueBool
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::JsonValueBool > ( "JsonValueBool", boost::python::init<  >(  ) )
        .def( boost::python::init< const bool >(( boost::python::arg("v") )) )
        .def( boost::python::init< const std::string & >(( boost::python::arg("v") )) )

        // Public Member Functions

        .def(
            "to_string",
            ( std::string ( ::genesis::utils::JsonValueBool::* )(  ) const )( &::genesis::utils::JsonValueBool::to_string )
        )
    ;
}

PYTHON_EXPORT_CLASS (JsonDocument, "utils")
{

    // -------------------------------------------------------------------
    //     Class JsonDocument
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::JsonDocument > ( "JsonDocument", boost::python::init<  >(  ) )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::dump )
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::to_string )
        )
        .def(
            "validate",
            ( bool ( ::genesis::utils::JsonDocument::* )(  ))( &::genesis::utils::JsonDocument::validate )
        )
    ;
}

PYTHON_EXPORT_CLASS (JsonValueArray, "utils")
{

    // -------------------------------------------------------------------
    //     Class JsonValueArray
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::JsonValueArray > ( "JsonValueArray", boost::python::init<  >(  ) )

        // Public Member Functions

        .def(
            "add",
            ( void ( ::genesis::utils::JsonValueArray::* )( JsonValue * ))( &::genesis::utils::JsonValueArray::add ),
            ( boost::python::arg("value") ),
            get_docstring("void ::genesis::utils::JsonValueArray::add (JsonValue * value)")
        )
        .def(
            "at",
            ( JsonValue * ( ::genesis::utils::JsonValueArray::* )( const std::size_t ) const )( &::genesis::utils::JsonValueArray::at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("JsonValue * ::genesis::utils::JsonValueArray::at (const std::size_t index) const")
        )
        .def(
            "cbegin",
            ( const_iterator ( ::genesis::utils::JsonValueArray::* )(  ) const )( &::genesis::utils::JsonValueArray::cbegin ),
            get_docstring("const_iterator ::genesis::utils::JsonValueArray::cbegin () const")
        )
        .def(
            "cend",
            ( const_iterator ( ::genesis::utils::JsonValueArray::* )(  ) const )( &::genesis::utils::JsonValueArray::cend ),
            get_docstring("const_iterator ::genesis::utils::JsonValueArray::cend () const")
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::JsonValueArray::* )(  ))( &::genesis::utils::JsonValueArray::clear ),
            get_docstring("void ::genesis::utils::JsonValueArray::clear ()")
        )
        .def(
            "empty",
            ( bool ( ::genesis::utils::JsonValueArray::* )(  ) const )( &::genesis::utils::JsonValueArray::empty ),
            get_docstring("bool ::genesis::utils::JsonValueArray::empty () const")
        )
        .def(
            "pop_back",
            ( void ( ::genesis::utils::JsonValueArray::* )(  ))( &::genesis::utils::JsonValueArray::pop_back )
        )
        .def(
            "push_back",
            ( void ( ::genesis::utils::JsonValueArray::* )( JsonValue * ))( &::genesis::utils::JsonValueArray::push_back ),
            ( boost::python::arg("value") )
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::JsonValueArray::* )(  ) const )( &::genesis::utils::JsonValueArray::size ),
            get_docstring("size_t ::genesis::utils::JsonValueArray::size () const")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::JsonValueArray::* )(  ) const )( &::genesis::utils::JsonValueArray::to_string )
        )

        // Operators

        .def(
            "__getitem__",
            ( JsonValue * ( ::genesis::utils::JsonValueArray::* )( const std::size_t ) const )( &::genesis::utils::JsonValueArray::operator[] ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("JsonValue * ::genesis::utils::JsonValueArray::operator[] (const std::size_t index) const")
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::utils::JsonValueArray::begin, &::genesis::utils::JsonValueArray::end )
        )
    ;
}

PYTHON_EXPORT_CLASS (JsonValueObject, "utils")
{

    // -------------------------------------------------------------------
    //     Class JsonValueObject
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::JsonValueObject > ( "JsonValueObject", boost::python::init<  >(  ) )

        // Public Member Functions

        .def(
            "at",
            ( JsonValue * ( ::genesis::utils::JsonValueObject::* )( const std::string & ) const )( &::genesis::utils::JsonValueObject::at ),
            ( boost::python::arg("name") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("JsonValue * ::genesis::utils::JsonValueObject::at (const std::string & name) const")
        )
        .def(
            "cbegin",
            ( const_iterator ( ::genesis::utils::JsonValueObject::* )(  ) const )( &::genesis::utils::JsonValueObject::cbegin ),
            get_docstring("const_iterator ::genesis::utils::JsonValueObject::cbegin () const")
        )
        .def(
            "cend",
            ( const_iterator ( ::genesis::utils::JsonValueObject::* )(  ) const )( &::genesis::utils::JsonValueObject::cend ),
            get_docstring("const_iterator ::genesis::utils::JsonValueObject::cend () const")
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::JsonValueObject::* )(  ))( &::genesis::utils::JsonValueObject::clear ),
            get_docstring("void ::genesis::utils::JsonValueObject::clear ()")
        )
        .def(
            "empty",
            ( bool ( ::genesis::utils::JsonValueObject::* )(  ) const )( &::genesis::utils::JsonValueObject::empty ),
            get_docstring("bool ::genesis::utils::JsonValueObject::empty () const")
        )
        .def(
            "erase",
            ( bool ( ::genesis::utils::JsonValueObject::* )( const std::string ))( &::genesis::utils::JsonValueObject::erase ),
            ( boost::python::arg("name") ),
            get_docstring("bool ::genesis::utils::JsonValueObject::erase (const std::string name)")
        )
        .def(
            "get",
            ( JsonValue * ( ::genesis::utils::JsonValueObject::* )( const std::string & ) const )( &::genesis::utils::JsonValueObject::get ),
            ( boost::python::arg("name") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("JsonValue * ::genesis::utils::JsonValueObject::get (const std::string & name) const")
        )
        .def(
            "has",
            ( bool ( ::genesis::utils::JsonValueObject::* )( const std::string & ) const )( &::genesis::utils::JsonValueObject::has ),
            ( boost::python::arg("name") ),
            get_docstring("bool ::genesis::utils::JsonValueObject::has (const std::string & name) const")
        )
        .def(
            "set",
            ( void ( ::genesis::utils::JsonValueObject::* )( const std::string &, JsonValue * ))( &::genesis::utils::JsonValueObject::set ),
            ( boost::python::arg("name"), boost::python::arg("value") ),
            get_docstring("void ::genesis::utils::JsonValueObject::set (const std::string & name, JsonValue * value)")
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::JsonValueObject::* )(  ) const )( &::genesis::utils::JsonValueObject::size ),
            get_docstring("size_t ::genesis::utils::JsonValueObject::size () const")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::utils::JsonValueObject::* )(  ) const )( &::genesis::utils::JsonValueObject::to_string )
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::utils::JsonValueObject::begin, &::genesis::utils::JsonValueObject::end )
        )
    ;
}

PYTHON_EXPORT_CLASS (JsonValueNull, "utils")
{

    // -------------------------------------------------------------------
    //     Class JsonValueNull
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::JsonValueNull > ( "JsonValueNull", boost::python::init<  >(  ) )
        .def( boost::python::init< const std::string & >(( boost::python::arg("v") )) )

        // Public Member Functions

        .def(
            "to_string",
            ( std::string ( ::genesis::utils::JsonValueNull::* )(  ) const )( &::genesis::utils::JsonValueNull::to_string )
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_io_json_document_export, "utils")
{

    boost::python::def(
        "json_value_to_array",
        ( JsonValueArray * ( * )( const JsonValue * ))( &::genesis::utils::json_value_to_array ),
        ( boost::python::arg("v") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("JsonValueArray * ::genesis::utils::json_value_to_array (const JsonValue * v)")
    );

    boost::python::def(
        "json_value_to_bool",
        ( JsonValueBool * ( * )( const JsonValue * ))( &::genesis::utils::json_value_to_bool ),
        ( boost::python::arg("v") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("JsonValueBool * ::genesis::utils::json_value_to_bool (const JsonValue * v)")
    );

    boost::python::def(
        "json_value_to_null",
        ( JsonValueNull * ( * )( const JsonValue * ))( &::genesis::utils::json_value_to_null ),
        ( boost::python::arg("v") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("JsonValueNull * ::genesis::utils::json_value_to_null (const JsonValue * v)")
    );

    boost::python::def(
        "json_value_to_number",
        ( JsonValueNumber * ( * )( const JsonValue * ))( &::genesis::utils::json_value_to_number ),
        ( boost::python::arg("v") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("JsonValueNumber * ::genesis::utils::json_value_to_number (const JsonValue * v)")
    );

    boost::python::def(
        "json_value_to_object",
        ( JsonValueObject * ( * )( const JsonValue * ))( &::genesis::utils::json_value_to_object ),
        ( boost::python::arg("v") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("JsonValueObject * ::genesis::utils::json_value_to_object (const JsonValue * v)")
    );

    boost::python::def(
        "json_value_to_string",
        ( JsonValueString * ( * )( const JsonValue * ))( &::genesis::utils::json_value_to_string ),
        ( boost::python::arg("v") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("JsonValueString * ::genesis::utils::json_value_to_string (const JsonValue * v)")
    );
}
*/
