/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_CLASS( ::genesis::taxonomy::TaxopathParser, scope )
{

    // -------------------------------------------------------------------
    //     Class TaxopathParser
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::TaxopathParser, std::shared_ptr<::genesis::taxonomy::TaxopathParser> > ( scope, "TaxopathParser" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< TaxopathParser const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "delimiters",
            ( TaxopathParser & ( ::genesis::taxonomy::TaxopathParser::* )( std::string const & ))( &::genesis::taxonomy::TaxopathParser::delimiters ),
            pybind11::arg("value")
        )
        .def(
            "delimiters",
            ( std::string ( ::genesis::taxonomy::TaxopathParser::* )(  ) const )( &::genesis::taxonomy::TaxopathParser::delimiters )
        )
        .def(
            "from_string",
            ( Taxopath ( ::genesis::taxonomy::TaxopathParser::* )( std::string const & ) const )( &::genesis::taxonomy::TaxopathParser::from_string ),
            pybind11::arg("taxopath")
        )
        .def(
            "from_taxon",
            ( Taxopath ( ::genesis::taxonomy::TaxopathParser::* )( Taxon const & ) const )( &::genesis::taxonomy::TaxopathParser::from_taxon ),
            pybind11::arg("taxon")
        )
        .def(
            "remove_trailing_delimiter",
            ( TaxopathParser & ( ::genesis::taxonomy::TaxopathParser::* )( bool ))( &::genesis::taxonomy::TaxopathParser::remove_trailing_delimiter ),
            pybind11::arg("value")
        )
        .def(
            "remove_trailing_delimiter",
            ( bool ( ::genesis::taxonomy::TaxopathParser::* )(  ) const )( &::genesis::taxonomy::TaxopathParser::remove_trailing_delimiter )
        )
        .def(
            "trim_whitespaces",
            ( TaxopathParser & ( ::genesis::taxonomy::TaxopathParser::* )( bool ))( &::genesis::taxonomy::TaxopathParser::trim_whitespaces ),
            pybind11::arg("value")
        )
        .def(
            "trim_whitespaces",
            ( bool ( ::genesis::taxonomy::TaxopathParser::* )(  ) const )( &::genesis::taxonomy::TaxopathParser::trim_whitespaces )
        )
    ;
}
