/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;
/*
PYTHON_EXPORT_CLASS( ::genesis::utils::XmlElement, scope )
{

    // -------------------------------------------------------------------
    //     Class XmlElement
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::XmlElement, std::shared_ptr<::genesis::utils::XmlElement> > ( scope, "XmlElement" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< const std::string & >(),
            pybind11::arg("tag")
        )

        // Public Member Functions

        // .def(
        //     "append_comment",
        //     ( XmlComment * ( ::genesis::utils::XmlElement::* )( const std::string & ))( &::genesis::utils::XmlElement::append_comment ),
        //     pybind11::arg("comment")
        // )
        // .def(
        //     "append_element",
        //     ( XmlElement * ( ::genesis::utils::XmlElement::* )( const std::string & ))( &::genesis::utils::XmlElement::append_element ),
        //     pybind11::arg("tag")
        // )
        // .def(
        //     "append_markup",
        //     ( XmlMarkup * ( ::genesis::utils::XmlElement::* )( const std::string & ))( &::genesis::utils::XmlElement::append_markup ),
        //     pybind11::arg("text")
        // )
        .def(
            "clear",
            ( void ( ::genesis::utils::XmlElement::* )(  ))( &::genesis::utils::XmlElement::clear )
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::XmlMarkup, scope )
{

    // -------------------------------------------------------------------
    //     Class XmlMarkup
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::XmlMarkup, std::shared_ptr<::genesis::utils::XmlMarkup> > ( scope, "XmlMarkup" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< const std::string & >(),
            pybind11::arg("content")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::XmlDocument, scope )
{

    // -------------------------------------------------------------------
    //     Class XmlDocument
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::XmlDocument, std::shared_ptr<::genesis::utils::XmlDocument> > ( scope, "XmlDocument" )

        // Public Member Functions

        .def(
            "clear",
            ( void ( ::genesis::utils::XmlDocument::* )(  ))( &::genesis::utils::XmlDocument::clear )
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::XmlComment, scope )
{

    // -------------------------------------------------------------------
    //     Class XmlComment
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::XmlComment, std::shared_ptr<::genesis::utils::XmlComment> > ( scope, "XmlComment" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< const std::string & >(),
            pybind11::arg("comment")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::XmlValue, scope )
{

    // -------------------------------------------------------------------
    //     Class XmlValue
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::XmlValue, std::shared_ptr<::genesis::utils::XmlValue> > ( scope, "XmlValue" )

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
            ( std::string ( ::genesis::utils::XmlValue::* )(  ) const )( &::genesis::utils::XmlValue::type_to_string )
        )
        .def_static(
            "type_to_string",
            ( std::string ( * )( const Type ))( &::genesis::utils::XmlValue::type_to_string ),
            pybind11::arg("t")
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_formats_xml_document_export, ::genesis::utils, scope )
{

    scope.def(
        "xml_value_to_comment",
        ( const XmlComment * ( * )( const XmlValue * ))( &::genesis::utils::xml_value_to_comment ),
            pybind11::arg("v")
    );

    scope.def(
        "xml_value_to_element",
        ( const XmlElement * ( * )( const XmlValue * ))( &::genesis::utils::xml_value_to_element ),
            pybind11::arg("v")
    );

    scope.def(
        "xml_value_to_markup",
        ( const XmlMarkup * ( * )( const XmlValue * ))( &::genesis::utils::xml_value_to_markup ),
            pybind11::arg("v")
    );
}
*/
