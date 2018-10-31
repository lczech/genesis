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
            pybind11::init<  >(),
            get_docstring("::genesis::taxonomy::Taxon::Taxon ()")
        )
        .def(
            pybind11::init< std::string const &, std::string const &, std::string const & >(),
            pybind11::arg("name"),
            pybind11::arg("rank")=(std::string const &)(""),
            pybind11::arg("id")=(std::string const &)(""),
            get_docstring("::genesis::taxonomy::Taxon::Taxon (std::string const & name, std::string const & rank=\"\", std::string const & id=\"\")")
        )
        .def(
            pybind11::init< Taxon const & >(),
            pybind11::arg("arg"),
            get_docstring("::genesis::taxonomy::Taxon::Taxon (Taxon const & )")
        )

        // Public Member Functions

        // .def(
        //     "data",
        //     ( TaxonDataType & ( ::genesis::taxonomy::Taxon::* )(  ))( &::genesis::taxonomy::Taxon::data )
        // )
        // .def(
        //     "data",
        //     ( TaxonDataType const & ( ::genesis::taxonomy::Taxon::* )(  ) const )( &::genesis::taxonomy::Taxon::data )
        // )
        // .def(
        //     "data_cast",
        //     ( TaxonDataType * ( ::genesis::taxonomy::Taxon::* )(  ))( &::genesis::taxonomy::Taxon::data_cast )
        // )
        // .def(
        //     "data_cast",
        //     ( TaxonDataType const * ( ::genesis::taxonomy::Taxon::* )(  ) const )( &::genesis::taxonomy::Taxon::data_cast )
        // )
        .def(
            "data_ptr",
            ( BaseTaxonData * ( ::genesis::taxonomy::Taxon::* )(  ))( &::genesis::taxonomy::Taxon::data_ptr ),
            get_docstring("BaseTaxonData * ::genesis::taxonomy::Taxon::data_ptr ()")
        )
        .def(
            "data_ptr",
            ( BaseTaxonData const * ( ::genesis::taxonomy::Taxon::* )(  ) const )( &::genesis::taxonomy::Taxon::data_ptr ),
            get_docstring("BaseTaxonData const * ::genesis::taxonomy::Taxon::data_ptr () const")
        )
        .def(
            "has_data",
            ( bool ( ::genesis::taxonomy::Taxon::* )(  ) const )( &::genesis::taxonomy::Taxon::has_data ),
            get_docstring("bool ::genesis::taxonomy::Taxon::has_data () const")
        )
        .def(
            "id",
            ( std::string const & ( ::genesis::taxonomy::Taxon::* )(  ) const )( &::genesis::taxonomy::Taxon::id ),
            get_docstring("std::string const & ::genesis::taxonomy::Taxon::id () const")
        )
        .def(
            "id",
            ( void ( ::genesis::taxonomy::Taxon::* )( std::string const & ))( &::genesis::taxonomy::Taxon::id ),
            pybind11::arg("value")
        )
        .def(
            "name",
            ( std::string const & ( ::genesis::taxonomy::Taxon::* )(  ) const )( &::genesis::taxonomy::Taxon::name ),
            get_docstring("std::string const & ::genesis::taxonomy::Taxon::name () const")
        )
        .def(
            "name",
            ( void ( ::genesis::taxonomy::Taxon::* )( std::string const & ))( &::genesis::taxonomy::Taxon::name ),
            pybind11::arg("value"),
            get_docstring("void ::genesis::taxonomy::Taxon::name (std::string const & value)")
        )
        .def(
            "parent",
            ( Taxon * ( ::genesis::taxonomy::Taxon::* )(  ))( &::genesis::taxonomy::Taxon::parent ),
            get_docstring("Taxon * ::genesis::taxonomy::Taxon::parent ()")
        )
        .def(
            "parent",
            ( Taxon const * ( ::genesis::taxonomy::Taxon::* )(  ) const )( &::genesis::taxonomy::Taxon::parent ),
            get_docstring("Taxon const * ::genesis::taxonomy::Taxon::parent () const")
        )
        .def(
            "rank",
            ( std::string const & ( ::genesis::taxonomy::Taxon::* )(  ) const )( &::genesis::taxonomy::Taxon::rank ),
            get_docstring("std::string const & ::genesis::taxonomy::Taxon::rank () const")
        )
        .def(
            "rank",
            ( void ( ::genesis::taxonomy::Taxon::* )( std::string const & ))( &::genesis::taxonomy::Taxon::rank ),
            pybind11::arg("value")
        )
        // .def(
        //     "reset_data",
        //     ( Taxon & ( ::genesis::taxonomy::Taxon::* )( std::unique_ptr< BaseTaxonData > ))( &::genesis::taxonomy::Taxon::reset_data ),
        //     pybind11::arg("data"),
        //     get_docstring("Taxon & ::genesis::taxonomy::Taxon::reset_data (std::unique_ptr< BaseTaxonData > data)")
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
