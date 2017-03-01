/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_CLASS( ::genesis::taxonomy::TaxonomyReader, scope )
{

    // -------------------------------------------------------------------
    //     Class TaxonomyReader
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::TaxonomyReader, std::shared_ptr<::genesis::taxonomy::TaxonomyReader> > ( scope, "TaxonomyReader" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< TaxonomyReader const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "csv_reader",
            ( utils::CsvReader & ( ::genesis::taxonomy::TaxonomyReader::* )(  ))( &::genesis::taxonomy::TaxonomyReader::csv_reader )
        )
        .def(
            "expect_strict_order",
            ( TaxonomyReader & ( ::genesis::taxonomy::TaxonomyReader::* )( bool ))( &::genesis::taxonomy::TaxonomyReader::expect_strict_order ),
            pybind11::arg("value")
        )
        .def(
            "expect_strict_order",
            ( bool ( ::genesis::taxonomy::TaxonomyReader::* )(  ) const )( &::genesis::taxonomy::TaxonomyReader::expect_strict_order )
        )
        .def(
            "from_file",
            ( void ( ::genesis::taxonomy::TaxonomyReader::* )( std::string const &, Taxonomy & ) const )( &::genesis::taxonomy::TaxonomyReader::from_file ),
            pybind11::arg("fn"),
            pybind11::arg("tax")
        )
        .def(
            "from_stream",
            ( void ( ::genesis::taxonomy::TaxonomyReader::* )( std::istream &, Taxonomy & ) const )( &::genesis::taxonomy::TaxonomyReader::from_stream ),
            pybind11::arg("is"),
            pybind11::arg("tax")
        )
        .def(
            "from_string",
            ( void ( ::genesis::taxonomy::TaxonomyReader::* )( std::string const &, Taxonomy & ) const )( &::genesis::taxonomy::TaxonomyReader::from_string ),
            pybind11::arg("is"),
            pybind11::arg("tax")
        )
        .def(
            "name_field_position",
            ( TaxonomyReader & ( ::genesis::taxonomy::TaxonomyReader::* )( int ))( &::genesis::taxonomy::TaxonomyReader::name_field_position ),
            pybind11::arg("value")
        )
        .def(
            "name_field_position",
            ( int ( ::genesis::taxonomy::TaxonomyReader::* )(  ) const )( &::genesis::taxonomy::TaxonomyReader::name_field_position )
        )
        .def(
            "parse_document",
            ( void ( ::genesis::taxonomy::TaxonomyReader::* )( utils::InputStream &, Taxonomy & ) const )( &::genesis::taxonomy::TaxonomyReader::parse_document ),
            pybind11::arg("it"),
            pybind11::arg("tax")
        )
        // .def(
        //     "parse_line",
        //     ( Line ( ::genesis::taxonomy::TaxonomyReader::* )( utils::InputStream & ) const )( &::genesis::taxonomy::TaxonomyReader::parse_line ),
        //     pybind11::arg("it")
        // )
        .def(
            "rank_field_position",
            ( TaxonomyReader & ( ::genesis::taxonomy::TaxonomyReader::* )( int ))( &::genesis::taxonomy::TaxonomyReader::rank_field_position ),
            pybind11::arg("value")
        )
        .def(
            "rank_field_position",
            ( int ( ::genesis::taxonomy::TaxonomyReader::* )(  ) const )( &::genesis::taxonomy::TaxonomyReader::rank_field_position )
        )
        .def(
            "taxopath_parser",
            ( TaxopathParser & ( ::genesis::taxonomy::TaxonomyReader::* )(  ))( &::genesis::taxonomy::TaxonomyReader::taxopath_parser )
        )
    ;
}
