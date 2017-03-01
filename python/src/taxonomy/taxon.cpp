/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_CLASS( ::genesis::taxonomy::Taxon, scope )
{

    // -------------------------------------------------------------------
    //     Class Taxon
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::Taxon, std::shared_ptr<::genesis::taxonomy::Taxon> > ( scope, "Taxon" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< std::string const & >(),
            pybind11::arg("name")
        )
        .def(
            pybind11::init< Taxon const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        // .def(
        //     "data",
        //     ( ::genesis::taxonomy::Taxon::TaxonDataType & ( ::genesis::taxonomy::Taxon::* )(  ))( &::genesis::taxonomy::Taxon::data )
        // )
        // .def(
        //     "data",
        //     ( ::genesis::taxonomy::Taxon::TaxonDataType const & ( ::genesis::taxonomy::Taxon::* )(  ) const )( &::genesis::taxonomy::Taxon::data )
        // )
        // .def(
        //     "data_cast",
        //     ( ::genesis::taxonomy::Taxon::TaxonDataType * ( ::genesis::taxonomy::Taxon::* )(  ))( &::genesis::taxonomy::Taxon::data_cast )
        // )
        // .def(
        //     "data_cast",
        //     ( ::genesis::taxonomy::Taxon::TaxonDataType const * ( ::genesis::taxonomy::Taxon::* )(  ) const )( &::genesis::taxonomy::Taxon::data_cast )
        // )
        .def(
            "data_ptr",
            ( BaseTaxonData * ( ::genesis::taxonomy::Taxon::* )(  ))( &::genesis::taxonomy::Taxon::data_ptr )
        )
        .def(
            "data_ptr",
            ( BaseTaxonData const * ( ::genesis::taxonomy::Taxon::* )(  ) const )( &::genesis::taxonomy::Taxon::data_ptr )
        )
        .def(
            "has_data",
            ( bool ( ::genesis::taxonomy::Taxon::* )(  ) const )( &::genesis::taxonomy::Taxon::has_data )
        )
        .def(
            "name",
            ( std::string const & ( ::genesis::taxonomy::Taxon::* )(  ) const )( &::genesis::taxonomy::Taxon::name )
        )
        .def(
            "name",
            ( void ( ::genesis::taxonomy::Taxon::* )( std::string const & ))( &::genesis::taxonomy::Taxon::name ),
            pybind11::arg("value")
        )
        .def(
            "parent",
            ( Taxon * ( ::genesis::taxonomy::Taxon::* )(  ))( &::genesis::taxonomy::Taxon::parent )
        )
        .def(
            "parent",
            ( Taxon const * ( ::genesis::taxonomy::Taxon::* )(  ) const )( &::genesis::taxonomy::Taxon::parent )
        )
        .def(
            "rank",
            ( std::string const & ( ::genesis::taxonomy::Taxon::* )(  ) const )( &::genesis::taxonomy::Taxon::rank )
        )
        .def(
            "rank",
            ( void ( ::genesis::taxonomy::Taxon::* )( std::string const & ))( &::genesis::taxonomy::Taxon::rank ),
            pybind11::arg("value")
        )
        // .def(
        //     "reset_data",
        //     ( Taxon & ( ::genesis::taxonomy::Taxon::* )( std::unique_ptr< BaseTaxonData > ))( &::genesis::taxonomy::Taxon::reset_data ),
        //     pybind11::arg("data")
        // )
    ;
}

PYTHON_EXPORT_FUNCTIONS( taxonomy_taxon_export, ::genesis::taxonomy, scope )
{

    scope.def(
        "swap",
        ( void ( * )( Taxon &, Taxon & ))( &::genesis::taxonomy::swap ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );
}
