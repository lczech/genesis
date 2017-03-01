/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_CLASS( ::genesis::taxonomy::TaxopathGenerator, scope )
{

    // -------------------------------------------------------------------
    //     Class TaxopathGenerator
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::TaxopathGenerator, std::shared_ptr<::genesis::taxonomy::TaxopathGenerator> > ( scope, "TaxopathGenerator" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< TaxopathGenerator const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "append_delimiter",
            ( TaxopathGenerator & ( ::genesis::taxonomy::TaxopathGenerator::* )( bool ))( &::genesis::taxonomy::TaxopathGenerator::append_delimiter ),
            pybind11::arg("value")
        )
        .def(
            "append_delimiter",
            ( bool ( ::genesis::taxonomy::TaxopathGenerator::* )(  ) const )( &::genesis::taxonomy::TaxopathGenerator::append_delimiter )
        )
        .def(
            "delimiter",
            ( TaxopathGenerator & ( ::genesis::taxonomy::TaxopathGenerator::* )( std::string const & ))( &::genesis::taxonomy::TaxopathGenerator::delimiter ),
            pybind11::arg("value")
        )
        .def(
            "delimiter",
            ( std::string ( ::genesis::taxonomy::TaxopathGenerator::* )(  ) const )( &::genesis::taxonomy::TaxopathGenerator::delimiter )
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::taxonomy::TaxopathGenerator::* )( Taxon const & ) const )( &::genesis::taxonomy::TaxopathGenerator::to_string ),
            pybind11::arg("taxon")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::taxonomy::TaxopathGenerator::* )( Taxopath const & ) const )( &::genesis::taxonomy::TaxopathGenerator::to_string ),
            pybind11::arg("taxopath")
        )
        .def(
            "trim_nested_duplicates",
            ( TaxopathGenerator & ( ::genesis::taxonomy::TaxopathGenerator::* )( bool ))( &::genesis::taxonomy::TaxopathGenerator::trim_nested_duplicates ),
            pybind11::arg("value")
        )
        .def(
            "trim_nested_duplicates",
            ( bool ( ::genesis::taxonomy::TaxopathGenerator::* )(  ) const )( &::genesis::taxonomy::TaxopathGenerator::trim_nested_duplicates )
        )
    ;
}
