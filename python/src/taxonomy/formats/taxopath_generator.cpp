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
            pybind11::arg("value"),
            get_docstring("TaxopathGenerator & ::genesis::taxonomy::TaxopathGenerator::append_delimiter (bool value)")
        )
        .def(
            "append_delimiter",
            ( bool ( ::genesis::taxonomy::TaxopathGenerator::* )(  ) const )( &::genesis::taxonomy::TaxopathGenerator::append_delimiter ),
            get_docstring("bool ::genesis::taxonomy::TaxopathGenerator::append_delimiter () const")
        )
        .def(
            "delimiter",
            ( TaxopathGenerator & ( ::genesis::taxonomy::TaxopathGenerator::* )( std::string const & ))( &::genesis::taxonomy::TaxopathGenerator::delimiter ),
            pybind11::arg("value"),
            get_docstring("TaxopathGenerator & ::genesis::taxonomy::TaxopathGenerator::delimiter (std::string const & value)")
        )
        .def(
            "delimiter",
            ( std::string ( ::genesis::taxonomy::TaxopathGenerator::* )(  ) const )( &::genesis::taxonomy::TaxopathGenerator::delimiter ),
            get_docstring("std::string ::genesis::taxonomy::TaxopathGenerator::delimiter () const")
        )
        .def(
            "field",
            ( ::genesis::taxonomy::TaxopathGenerator::TaxonField ( ::genesis::taxonomy::TaxopathGenerator::* )(  ) const )( &::genesis::taxonomy::TaxopathGenerator::field ),
            get_docstring("TaxonField ::genesis::taxonomy::TaxopathGenerator::field () const")
        )
        .def(
            "field",
            ( TaxopathGenerator & ( ::genesis::taxonomy::TaxopathGenerator::* )( ::genesis::taxonomy::TaxopathGenerator::TaxonField ))( &::genesis::taxonomy::TaxopathGenerator::field ),
            pybind11::arg("value"),
            get_docstring("TaxopathGenerator & ::genesis::taxonomy::TaxopathGenerator::field (TaxonField value)")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::taxonomy::TaxopathGenerator::* )( Taxon const & ) const )( &::genesis::taxonomy::TaxopathGenerator::to_string ),
            pybind11::arg("taxon"),
            get_docstring("std::string ::genesis::taxonomy::TaxopathGenerator::to_string (Taxon const & taxon) const")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::taxonomy::TaxopathGenerator::* )( Taxopath const & ) const )( &::genesis::taxonomy::TaxopathGenerator::to_string ),
            pybind11::arg("taxopath"),
            get_docstring("std::string ::genesis::taxonomy::TaxopathGenerator::to_string (Taxopath const & taxopath) const")
        )
        .def(
            "trim_nested_duplicates",
            ( TaxopathGenerator & ( ::genesis::taxonomy::TaxopathGenerator::* )( bool ))( &::genesis::taxonomy::TaxopathGenerator::trim_nested_duplicates ),
            pybind11::arg("value"),
            get_docstring("TaxopathGenerator & ::genesis::taxonomy::TaxopathGenerator::trim_nested_duplicates (bool value)")
        )
        .def(
            "trim_nested_duplicates",
            ( bool ( ::genesis::taxonomy::TaxopathGenerator::* )(  ) const )( &::genesis::taxonomy::TaxopathGenerator::trim_nested_duplicates ),
            get_docstring("bool ::genesis::taxonomy::TaxopathGenerator::trim_nested_duplicates () const")
        )
    ;
}
