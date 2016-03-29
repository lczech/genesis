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
PYTHON_EXPORT_CLASS (Deserializer, "utils")
{

    // -------------------------------------------------------------------
    //     Class Deserializer
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::Deserializer > ( "Deserializer", boost::python::init< const std::string & >(( boost::python::arg("file_name") )) )
        .def( boost::python::init< std::istream & >(( boost::python::arg("instream") )) )

        // Public Member Functions

        .def(
            "bad",
            ( bool ( ::genesis::utils::Deserializer::* )(  ) const )( &::genesis::utils::Deserializer::bad )
        )
        .def(
            "close",
            ( void ( ::genesis::utils::Deserializer::* )(  ))( &::genesis::utils::Deserializer::close )
        )
        .def(
            "eof",
            ( bool ( ::genesis::utils::Deserializer::* )(  ) const )( &::genesis::utils::Deserializer::eof )
        )
        .def(
            "fail",
            ( bool ( ::genesis::utils::Deserializer::* )(  ) const )( &::genesis::utils::Deserializer::fail )
        )
        .def(
            "get_float",
            ( T ( ::genesis::utils::Deserializer::* )(  ))( &::genesis::utils::Deserializer::get_float ),
            get_docstring("T ::genesis::utils::Deserializer::get_float ()")
        )
        .def(
            "get_float",
            ( void ( ::genesis::utils::Deserializer::* )( T & ))( &::genesis::utils::Deserializer::get_float ),
            ( boost::python::arg("res") ),
            get_docstring("void ::genesis::utils::Deserializer::get_float (T & res)")
        )
        .def(
            "get_int",
            ( T ( ::genesis::utils::Deserializer::* )(  ))( &::genesis::utils::Deserializer::get_int ),
            get_docstring("T ::genesis::utils::Deserializer::get_int ()")
        )
        .def(
            "get_int",
            ( void ( ::genesis::utils::Deserializer::* )( T & ))( &::genesis::utils::Deserializer::get_int ),
            ( boost::python::arg("res") ),
            get_docstring("void ::genesis::utils::Deserializer::get_int (T & res)")
        )
        .def(
            "get_null",
            ( bool ( ::genesis::utils::Deserializer::* )( size_t ))( &::genesis::utils::Deserializer::get_null ),
            ( boost::python::arg("n") ),
            get_docstring("bool ::genesis::utils::Deserializer::get_null (size_t n)")
        )
        .def(
            "get_plain",
            ( T ( ::genesis::utils::Deserializer::* )(  ))( &::genesis::utils::Deserializer::get_plain ),
            get_docstring("T ::genesis::utils::Deserializer::get_plain ()")
        )
        .def(
            "get_plain",
            ( void ( ::genesis::utils::Deserializer::* )( T & ))( &::genesis::utils::Deserializer::get_plain ),
            ( boost::python::arg("res") ),
            get_docstring("void ::genesis::utils::Deserializer::get_plain (T & res)")
        )
        .def(
            "get_raw",
            ( void ( ::genesis::utils::Deserializer::* )( char *, size_t ))( &::genesis::utils::Deserializer::get_raw ),
            ( boost::python::arg("buffer"), boost::python::arg("n") ),
            get_docstring("void ::genesis::utils::Deserializer::get_raw (char * buffer, size_t n)")
        )
        .def(
            "get_raw_string",
            ( std::string ( ::genesis::utils::Deserializer::* )( size_t ))( &::genesis::utils::Deserializer::get_raw_string ),
            ( boost::python::arg("n") ),
            get_docstring("std::string ::genesis::utils::Deserializer::get_raw_string (size_t n)")
        )
        .def(
            "get_string",
            ( std::string ( ::genesis::utils::Deserializer::* )(  ))( &::genesis::utils::Deserializer::get_string ),
            get_docstring("std::string ::genesis::utils::Deserializer::get_string ()")
        )
        .def(
            "good",
            ( bool ( ::genesis::utils::Deserializer::* )(  ) const )( &::genesis::utils::Deserializer::good )
        )
        .def(
            "is_open",
            ( bool ( ::genesis::utils::Deserializer::* )(  ) const )( &::genesis::utils::Deserializer::is_open )
        )
        .def(
            "succeeded",
            ( bool ( ::genesis::utils::Deserializer::* )(  ) const )( &::genesis::utils::Deserializer::succeeded )
        )
    ;
}

PYTHON_EXPORT_CLASS (Serializer, "utils")
{

    // -------------------------------------------------------------------
    //     Class Serializer
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::Serializer > ( "Serializer", boost::python::init< const std::string & >(( boost::python::arg("file_name") )) )
        .def( boost::python::init< std::ostream & >(( boost::python::arg("outstream") )) )

        // Public Member Functions

        .def(
            "bad",
            ( bool ( ::genesis::utils::Serializer::* )(  ) const )( &::genesis::utils::Serializer::bad )
        )
        .def(
            "close",
            ( void ( ::genesis::utils::Serializer::* )(  ))( &::genesis::utils::Serializer::close )
        )
        .def(
            "eof",
            ( bool ( ::genesis::utils::Serializer::* )(  ) const )( &::genesis::utils::Serializer::eof )
        )
        .def(
            "fail",
            ( bool ( ::genesis::utils::Serializer::* )(  ) const )( &::genesis::utils::Serializer::fail )
        )
        .def(
            "flush",
            ( void ( ::genesis::utils::Serializer::* )(  ))( &::genesis::utils::Serializer::flush )
        )
        .def(
            "good",
            ( bool ( ::genesis::utils::Serializer::* )(  ) const )( &::genesis::utils::Serializer::good )
        )
        .def(
            "is_open",
            ( bool ( ::genesis::utils::Serializer::* )(  ) const )( &::genesis::utils::Serializer::is_open )
        )
        .def(
            "put_float",
            ( void ( ::genesis::utils::Serializer::* )( const T ))( &::genesis::utils::Serializer::put_float ),
            ( boost::python::arg("v") ),
            get_docstring("void ::genesis::utils::Serializer::put_float (const T v)")
        )
        .def(
            "put_int",
            ( void ( ::genesis::utils::Serializer::* )( const T ))( &::genesis::utils::Serializer::put_int ),
            ( boost::python::arg("v") ),
            get_docstring("void ::genesis::utils::Serializer::put_int (const T v)")
        )
        .def(
            "put_null",
            ( void ( ::genesis::utils::Serializer::* )( const size_t ))( &::genesis::utils::Serializer::put_null ),
            ( boost::python::arg("n") ),
            get_docstring("void ::genesis::utils::Serializer::put_null (const size_t n)")
        )
        .def(
            "put_plain",
            ( void ( ::genesis::utils::Serializer::* )( const T ))( &::genesis::utils::Serializer::put_plain ),
            ( boost::python::arg("v") ),
            get_docstring("void ::genesis::utils::Serializer::put_plain (const T v)")
        )
        .def(
            "put_raw",
            ( void ( ::genesis::utils::Serializer::* )( char *, size_t ))( &::genesis::utils::Serializer::put_raw ),
            ( boost::python::arg("data"), boost::python::arg("n") ),
            get_docstring("void ::genesis::utils::Serializer::put_raw (char * data, size_t n)")
        )
        .def(
            "put_raw_string",
            ( void ( ::genesis::utils::Serializer::* )( const std::string & ))( &::genesis::utils::Serializer::put_raw_string ),
            ( boost::python::arg("v") ),
            get_docstring("void ::genesis::utils::Serializer::put_raw_string (const std::string & v)")
        )
        .def(
            "put_string",
            ( void ( ::genesis::utils::Serializer::* )( const std::string & ))( &::genesis::utils::Serializer::put_string ),
            ( boost::python::arg("v") ),
            get_docstring("void ::genesis::utils::Serializer::put_string (const std::string & v)")
        )
    ;
}
*/
