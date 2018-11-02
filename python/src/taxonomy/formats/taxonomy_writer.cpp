/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_CLASS( ::genesis::taxonomy::TaxonomyWriter, scope )
{

    // -------------------------------------------------------------------
    //     Class TaxonomyWriter
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::TaxonomyWriter, std::shared_ptr<::genesis::taxonomy::TaxonomyWriter> > ( scope, "TaxonomyWriter" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< TaxonomyWriter const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "taxopath_generator",
            ( TaxopathGenerator & ( ::genesis::taxonomy::TaxonomyWriter::* )(  ))( &::genesis::taxonomy::TaxonomyWriter::taxopath_generator )
        )
        .def(
            "to_file",
            ( void ( ::genesis::taxonomy::TaxonomyWriter::* )( Taxonomy const &, std::string const & ) const )( &::genesis::taxonomy::TaxonomyWriter::to_file ),
            pybind11::arg("tax"),
            pybind11::arg("fn")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::taxonomy::TaxonomyWriter::* )( Taxonomy const &, std::ostream & ) const )( &::genesis::taxonomy::TaxonomyWriter::to_stream ),
            pybind11::arg("tax"),
            pybind11::arg("os")
        )
        .def(
            "to_string",
            ( std::string ( ::genesis::taxonomy::TaxonomyWriter::* )( Taxonomy const & ) const )( &::genesis::taxonomy::TaxonomyWriter::to_string ),
            pybind11::arg("tax")
        )
        .def(
            "write_ids",
            ( TaxonomyWriter & ( ::genesis::taxonomy::TaxonomyWriter::* )( bool ))( &::genesis::taxonomy::TaxonomyWriter::write_ids ),
            pybind11::arg("value")
        )
        .def(
            "write_ids",
            ( bool ( ::genesis::taxonomy::TaxonomyWriter::* )(  ) const )( &::genesis::taxonomy::TaxonomyWriter::write_ids )
        )
        .def(
            "write_ranks",
            ( TaxonomyWriter & ( ::genesis::taxonomy::TaxonomyWriter::* )( bool ))( &::genesis::taxonomy::TaxonomyWriter::write_ranks ),
            pybind11::arg("value")
        )
        .def(
            "write_ranks",
            ( bool ( ::genesis::taxonomy::TaxonomyWriter::* )(  ) const )( &::genesis::taxonomy::TaxonomyWriter::write_ranks )
        )
    ;
}
