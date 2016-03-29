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

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS (JplaceReader, "placement")
{

    // -------------------------------------------------------------------
    //     Class JplaceReader
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::JplaceReader > ( "JplaceReader", boost::python::init<  >(  ) )
        .def( boost::python::init< JplaceReader const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "check_version",
            ( bool ( * )( std::string const & ))( &::genesis::placement::JplaceReader::check_version ),
            ( boost::python::arg("version") ),
            get_docstring("static bool ::genesis::placement::JplaceReader::check_version (std::string const & version)")
        )
        .staticmethod("check_version")
        .def(
            "from_document",
            ( void ( ::genesis::placement::JplaceReader::* )( utils::JsonDocument const &, Sample & ) const )( &::genesis::placement::JplaceReader::from_document ),
            ( boost::python::arg("doc"), boost::python::arg("smp") ),
            get_docstring("void ::genesis::placement::JplaceReader::from_document (utils::JsonDocument const & doc, Sample & smp) const")
        )
        .def(
            "from_file",
            ( void ( ::genesis::placement::JplaceReader::* )( std::string const &, Sample & ) const )( &::genesis::placement::JplaceReader::from_file ),
            ( boost::python::arg("fn"), boost::python::arg("smp") ),
            get_docstring("void ::genesis::placement::JplaceReader::from_file (std::string const & fn, Sample & smp) const")
        )
        .def(
            "from_files",
            ( void ( ::genesis::placement::JplaceReader::* )( std::vector< std::string > const &, SampleSet & ) const )( &::genesis::placement::JplaceReader::from_files ),
            ( boost::python::arg("fns"), boost::python::arg("set") ),
            get_docstring("void ::genesis::placement::JplaceReader::from_files (std::vector< std::string > const & fns, SampleSet & set) const")
        )
        .def(
            "from_stream",
            ( void ( ::genesis::placement::JplaceReader::* )( std::istream &, Sample & ) const )( &::genesis::placement::JplaceReader::from_stream ),
            ( boost::python::arg("is"), boost::python::arg("smp") ),
            get_docstring("void ::genesis::placement::JplaceReader::from_stream (std::istream & is, Sample & smp) const")
        )
        .def(
            "from_string",
            ( void ( ::genesis::placement::JplaceReader::* )( std::string const &, Sample & ) const )( &::genesis::placement::JplaceReader::from_string ),
            ( boost::python::arg("jplace"), boost::python::arg("smp") ),
            get_docstring("void ::genesis::placement::JplaceReader::from_string (std::string const & jplace, Sample & smp) const")
        )
        .def(
            "from_strings",
            ( void ( ::genesis::placement::JplaceReader::* )( std::vector< std::string > const &, SampleSet & ) const )( &::genesis::placement::JplaceReader::from_strings ),
            ( boost::python::arg("jps"), boost::python::arg("set") ),
            get_docstring("void ::genesis::placement::JplaceReader::from_strings (std::vector< std::string > const & jps, SampleSet & set) const")
        )
        // .def(
        //     "invalid_number_behaviour",
        //     ( InvalidNumberBehaviour ( ::genesis::placement::JplaceReader::* )(  ) const )( &::genesis::placement::JplaceReader::invalid_number_behaviour ),
        //     get_docstring("InvalidNumberBehaviour ::genesis::placement::JplaceReader::invalid_number_behaviour () const")
        // )
        // .def(
        //     "invalid_number_behaviour",
        //     ( JplaceReader & ( ::genesis::placement::JplaceReader::* )( InvalidNumberBehaviour ))( &::genesis::placement::JplaceReader::invalid_number_behaviour ),
        //     ( boost::python::arg("val") ),
        //     boost::python::return_value_policy<boost::python::reference_existing_object>(),
        //     get_docstring("JplaceReader & ::genesis::placement::JplaceReader::invalid_number_behaviour (InvalidNumberBehaviour val)")
        // )
        .def(
            "version",
            ( std::string ( * )(  ))( &::genesis::placement::JplaceReader::version ),
            get_docstring("static std::string ::genesis::placement::JplaceReader::version ()")
        )
        .staticmethod("version")
    ;
}
