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
PYTHON_EXPORT_CLASS( ::genesis::utils::JsonDocument, scope )
{

    // -------------------------------------------------------------------
    //     Class JsonDocument
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::JsonDocument, std::shared_ptr<::genesis::utils::JsonDocument> > ( scope, "JsonDocument" )
        .def(
            pybind11::init< std::nullptr_t >(),
            pybind11::arg("arg")=(std::nullptr_t)(nullptr),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (std::nullptr_t =nullptr)")
        )
        .def(
            pybind11::init< ValueType >(),
            pybind11::arg("type"),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (ValueType type)")
        )
        .def(
            pybind11::init< ArrayType const & >(),
            pybind11::arg("v"),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (ArrayType const & v)")
        )
        .def(
            pybind11::init< ObjectType const & >(),
            pybind11::arg("v"),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (ObjectType const & v)")
        )
        .def(
            pybind11::init< StringType const & >(),
            pybind11::arg("v"),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (StringType const & v)")
        )
        .def(
            pybind11::init< typename StringType::value_type const * >(),
            pybind11::arg("v"),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (typename StringType::value_type const * v)")
        )
        .def(
            pybind11::init< BooleanType >(),
            pybind11::arg("v"),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (BooleanType v)")
        )
        .def(
            pybind11::init< const NumberSignedType >(),
            pybind11::arg("val"),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (const NumberSignedType val)")
        )
        .def(
            pybind11::init< int const >(),
            pybind11::arg("val"),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (int const val)")
        )
        .def(
            pybind11::init< const NumberUnsignedType >(),
            pybind11::arg("val"),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (const NumberUnsignedType val)")
        )
        .def(
            pybind11::init< const NumberFloatType >(),
            pybind11::arg("val"),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (const NumberFloatType val)")
        )
        .def(
            pybind11::init< std::initializer_list< JsonDocument >, bool, ValueType >(),
            pybind11::arg("init"),
            pybind11::arg("type_deduction")=(bool)(true),
            pybind11::arg("manual_type"),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (std::initializer_list< JsonDocument > init, bool type_deduction=true, ValueType manual_type)")
        )
        .def(
            pybind11::init< size_t, JsonDocument const & >(),
            pybind11::arg("n"),
            pybind11::arg("val"),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (size_t n, JsonDocument const & val)")
        )
        .def(
            pybind11::init< const CompatibleArrayTypeT & >(),
            pybind11::arg("val")
        )
        .def(
            pybind11::init< CompatibleObjectType const & >(),
            pybind11::arg("val"),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (CompatibleObjectType const & val)")
        )
        .def(
            pybind11::init< const CompatibleStringType & >(),
            pybind11::arg("val"),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (const CompatibleStringType & val)")
        )
        .def(
            pybind11::init< JsonDocument const & >(),
            pybind11::arg("other"),
            get_docstring("::genesis::utils::JsonDocument::JsonDocument (JsonDocument const & other)")
        )

        // Public Member Functions

        .def(
            "at",
            ( JsonDocument & ( ::genesis::utils::JsonDocument::* )( size_t ))( &::genesis::utils::JsonDocument::at ),
            pybind11::arg("index")
        )
        .def(
            "at",
            ( JsonDocument & ( ::genesis::utils::JsonDocument::* )( typename ObjectType::key_type const & ))( &::genesis::utils::JsonDocument::at ),
            pybind11::arg("key")
        )
        .def(
            "at",
            ( JsonDocument const & ( ::genesis::utils::JsonDocument::* )( size_t ) const )( &::genesis::utils::JsonDocument::at ),
            pybind11::arg("index")
        )
        .def(
            "at",
            ( JsonDocument const & ( ::genesis::utils::JsonDocument::* )( typename ObjectType::key_type const & ) const )( &::genesis::utils::JsonDocument::at ),
            pybind11::arg("key")
        )
        .def(
            "cbegin",
            ( const_iterator ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::cbegin ),
            get_docstring("const_iterator ::genesis::utils::JsonDocument::cbegin () const")
        )
        .def(
            "cend",
            ( const_iterator ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::cend ),
            get_docstring("const_iterator ::genesis::utils::JsonDocument::cend () const")
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::JsonDocument::* )(  ))( &::genesis::utils::JsonDocument::clear ),
            get_docstring("void ::genesis::utils::JsonDocument::clear ()")
        )
        .def(
            "count",
            ( size_type ( ::genesis::utils::JsonDocument::* )( typename ObjectType::key_type ) const )( &::genesis::utils::JsonDocument::count ),
            pybind11::arg("key"),
            get_docstring("size_type ::genesis::utils::JsonDocument::count (typename ObjectType::key_type key) const")
        )
        .def(
            "emplace_back",
            ( void ( ::genesis::utils::JsonDocument::* )( Args &&... ))( &::genesis::utils::JsonDocument::emplace_back ),
            pybind11::arg("args")
        )
        .def(
            "empty",
            ( bool ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::empty )
        )
        .def(
            "find",
            ( const_iterator ( ::genesis::utils::JsonDocument::* )( typename ObjectType::key_type ) const )( &::genesis::utils::JsonDocument::find ),
            pybind11::arg("key"),
            get_docstring("const_iterator ::genesis::utils::JsonDocument::find (typename ObjectType::key_type key) const")
        )
        .def(
            "find",
            ( iterator ( ::genesis::utils::JsonDocument::* )( typename ObjectType::key_type ))( &::genesis::utils::JsonDocument::find ),
            pybind11::arg("key"),
            get_docstring("iterator ::genesis::utils::JsonDocument::find (typename ObjectType::key_type key)")
        )
        .def(
            "get_array",
            ( ArrayType & ( ::genesis::utils::JsonDocument::* )(  ))( &::genesis::utils::JsonDocument::get_array )
        )
        .def(
            "get_array",
            ( ArrayType const & ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::get_array )
        )
        .def(
            "get_boolean",
            ( BooleanType & ( ::genesis::utils::JsonDocument::* )(  ))( &::genesis::utils::JsonDocument::get_boolean )
        )
        .def(
            "get_boolean",
            ( BooleanType ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::get_boolean )
        )
        .def(
            "get_number",
            ( T ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::get_number )
        )
        .def(
            "get_number_float",
            ( NumberFloatType & ( ::genesis::utils::JsonDocument::* )(  ))( &::genesis::utils::JsonDocument::get_number_float )
        )
        .def(
            "get_number_float",
            ( NumberFloatType ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::get_number_float )
        )
        .def(
            "get_number_signed",
            ( NumberSignedType & ( ::genesis::utils::JsonDocument::* )(  ))( &::genesis::utils::JsonDocument::get_number_signed )
        )
        .def(
            "get_number_signed",
            ( NumberSignedType ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::get_number_signed )
        )
        .def(
            "get_number_unsigned",
            ( NumberUnsignedType & ( ::genesis::utils::JsonDocument::* )(  ))( &::genesis::utils::JsonDocument::get_number_unsigned )
        )
        .def(
            "get_number_unsigned",
            ( NumberUnsignedType ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::get_number_unsigned )
        )
        .def(
            "get_object",
            ( ObjectType & ( ::genesis::utils::JsonDocument::* )(  ))( &::genesis::utils::JsonDocument::get_object )
        )
        .def(
            "get_object",
            ( ObjectType const & ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::get_object )
        )
        .def(
            "get_string",
            ( StringType & ( ::genesis::utils::JsonDocument::* )(  ))( &::genesis::utils::JsonDocument::get_string )
        )
        .def(
            "get_string",
            ( StringType const & ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::get_string )
        )
        .def(
            "is_array",
            ( bool ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::is_array ),
            get_docstring("bool ::genesis::utils::JsonDocument::is_array () const")
        )
        .def(
            "is_boolean",
            ( bool ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::is_boolean ),
            get_docstring("bool ::genesis::utils::JsonDocument::is_boolean () const")
        )
        .def(
            "is_null",
            ( bool ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::is_null ),
            get_docstring("bool ::genesis::utils::JsonDocument::is_null () const")
        )
        .def(
            "is_number",
            ( bool ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::is_number ),
            get_docstring("bool ::genesis::utils::JsonDocument::is_number () const")
        )
        .def(
            "is_number_float",
            ( bool ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::is_number_float ),
            get_docstring("bool ::genesis::utils::JsonDocument::is_number_float () const")
        )
        .def(
            "is_number_integer",
            ( bool ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::is_number_integer ),
            get_docstring("bool ::genesis::utils::JsonDocument::is_number_integer () const")
        )
        .def(
            "is_number_signed",
            ( bool ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::is_number_signed ),
            get_docstring("bool ::genesis::utils::JsonDocument::is_number_signed () const")
        )
        .def(
            "is_number_unsigned",
            ( bool ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::is_number_unsigned ),
            get_docstring("bool ::genesis::utils::JsonDocument::is_number_unsigned () const")
        )
        .def(
            "is_object",
            ( bool ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::is_object ),
            get_docstring("bool ::genesis::utils::JsonDocument::is_object () const")
        )
        .def(
            "is_primitive",
            ( bool ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::is_primitive ),
            get_docstring("bool ::genesis::utils::JsonDocument::is_primitive () const")
        )
        .def(
            "is_string",
            ( bool ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::is_string ),
            get_docstring("bool ::genesis::utils::JsonDocument::is_string () const")
        )
        .def(
            "is_structured",
            ( bool ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::is_structured ),
            get_docstring("bool ::genesis::utils::JsonDocument::is_structured () const")
        )
        .def(
            "max_size",
            ( size_t ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::max_size )
        )
        .def(
            "push_back",
            ( void ( ::genesis::utils::JsonDocument::* )( JsonDocument && ))( &::genesis::utils::JsonDocument::push_back ),
            pybind11::arg("val"),
            get_docstring("void ::genesis::utils::JsonDocument::push_back (JsonDocument && val)")
        )
        .def(
            "push_back",
            ( void ( ::genesis::utils::JsonDocument::* )( JsonDocument const & ))( &::genesis::utils::JsonDocument::push_back ),
            pybind11::arg("val"),
            get_docstring("void ::genesis::utils::JsonDocument::push_back (JsonDocument const & val)")
        )
        .def(
            "push_back",
            ( void ( ::genesis::utils::JsonDocument::* )( typename ObjectType::value_type const & ))( &::genesis::utils::JsonDocument::push_back ),
            pybind11::arg("val"),
            get_docstring("void ::genesis::utils::JsonDocument::push_back (typename ObjectType::value_type const & val)")
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::size )
        )
        .def(
            "type",
            ( ValueType ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::type ),
            get_docstring("ValueType ::genesis::utils::JsonDocument::type () const")
        )
        .def(
            "type_name",
            ( std::string ( ::genesis::utils::JsonDocument::* )(  ) const )( &::genesis::utils::JsonDocument::type_name )
        )
        .def_static(
            "array",
            ( JsonDocument ( * )( std::initializer_list< JsonDocument > ))( &::genesis::utils::JsonDocument::array ),
            pybind11::arg("init")=(std::initializer_list< JsonDocument >)(std::initializer_list< ),
            get_docstring("static JsonDocument ::genesis::utils::JsonDocument::array (std::initializer_list< JsonDocument > init=std::initializer_list< )")
        )
        .def_static(
            "boolean",
            ( JsonDocument ( * )( BooleanType ))( &::genesis::utils::JsonDocument::boolean ),
            pybind11::arg("value"),
            get_docstring("static JsonDocument ::genesis::utils::JsonDocument::boolean (BooleanType value)")
        )
        .def_static(
            "number_float",
            ( JsonDocument ( * )( NumberFloatType ))( &::genesis::utils::JsonDocument::number_float ),
            pybind11::arg("value"),
            get_docstring("static JsonDocument ::genesis::utils::JsonDocument::number_float (NumberFloatType value)")
        )
        .def_static(
            "number_signed",
            ( JsonDocument ( * )( NumberSignedType ))( &::genesis::utils::JsonDocument::number_signed ),
            pybind11::arg("value"),
            get_docstring("static JsonDocument ::genesis::utils::JsonDocument::number_signed (NumberSignedType value)")
        )
        .def_static(
            "number_unsigned",
            ( JsonDocument ( * )( NumberUnsignedType ))( &::genesis::utils::JsonDocument::number_unsigned ),
            pybind11::arg("value"),
            get_docstring("static JsonDocument ::genesis::utils::JsonDocument::number_unsigned (NumberUnsignedType value)")
        )
        .def_static(
            "object",
            ( JsonDocument ( * )( std::initializer_list< JsonDocument > ))( &::genesis::utils::JsonDocument::object ),
            pybind11::arg("init")=(std::initializer_list< JsonDocument >)(std::initializer_list< ),
            get_docstring("static JsonDocument ::genesis::utils::JsonDocument::object (std::initializer_list< JsonDocument > init=std::initializer_list< )")
        )
        .def_static(
            "string",
            ( JsonDocument ( * )( std::string const & ))( &::genesis::utils::JsonDocument::string ),
            pybind11::arg("init"),
            get_docstring("static JsonDocument ::genesis::utils::JsonDocument::string (std::string const & init)")
        )

        // Operators

        .def(
            "__getitem__",
            ( JsonDocument & ( ::genesis::utils::JsonDocument::* )( size_t ))( &::genesis::utils::JsonDocument::operator[] ),
            pybind11::arg("index")
        )
        .def(
            "__getitem__",
            ( JsonDocument const & ( ::genesis::utils::JsonDocument::* )( size_t ) const )( &::genesis::utils::JsonDocument::operator[] ),
            pybind11::arg("index")
        )
        .def(
            "__getitem__",
            ( JsonDocument & ( ::genesis::utils::JsonDocument::* )( typename ObjectType::key_type const & ))( &::genesis::utils::JsonDocument::operator[] ),
            pybind11::arg("key")
        )
        .def(
            "__getitem__",
            ( JsonDocument const & ( ::genesis::utils::JsonDocument::* )( typename ObjectType::key_type const & ) const )( &::genesis::utils::JsonDocument::operator[] ),
            pybind11::arg("key")
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::utils::JsonDocument& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            pybind11::keep_alive<0, 1>()
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_formats_json_document_export, ::genesis::utils, scope )
{

    scope.def(
        "operator!=",
        ( bool ( * )( JsonDocument const &, JsonDocument const & ))( &::genesis::utils::operator!= ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "operator!=",
        ( bool ( * )( JsonDocument const &, std::nullptr_t ))( &::genesis::utils::operator!= ),
            pybind11::arg("v"),
            pybind11::arg("")
    );

    scope.def(
        "operator!=",
        ( bool ( * )( std::nullptr_t, JsonDocument const & ))( &::genesis::utils::operator!= ),
            pybind11::arg(""),
            pybind11::arg("v")
    );

    scope.def(
        "operator<",
        ( bool ( * )( JsonDocument const &, JsonDocument const & ))( &::genesis::utils::operator< ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "operator<=",
        ( bool ( * )( JsonDocument const &, JsonDocument const & ))( &::genesis::utils::operator<= ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "operator==",
        ( bool ( * )( JsonDocument const &, JsonDocument const & ))( &::genesis::utils::operator== ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "operator==",
        ( bool ( * )( JsonDocument const &, std::nullptr_t ))( &::genesis::utils::operator== ),
            pybind11::arg("v"),
            pybind11::arg("")
    );

    scope.def(
        "operator==",
        ( bool ( * )( std::nullptr_t, JsonDocument const & ))( &::genesis::utils::operator== ),
            pybind11::arg(""),
            pybind11::arg("v")
    );

    scope.def(
        "operator>",
        ( bool ( * )( JsonDocument const &, JsonDocument const & ))( &::genesis::utils::operator> ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "operator>=",
        ( bool ( * )( JsonDocument const &, JsonDocument const & ))( &::genesis::utils::operator>= ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );
}
*/
