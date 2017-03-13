/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::NexusTaxa, scope )
{

    // -------------------------------------------------------------------
    //     Class NexusTaxa
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::NexusTaxa, std::shared_ptr<::genesis::utils::NexusTaxa> > ( scope, "NexusTaxa" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< NexusTaxa const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "add_taxa",
            ( void ( ::genesis::utils::NexusTaxa::* )( std::unordered_set< std::string > ))( &::genesis::utils::NexusTaxa::add_taxa ),
            pybind11::arg("taxa")
        )
        .def(
            "add_taxon",
            ( void ( ::genesis::utils::NexusTaxa::* )( std::string ))( &::genesis::utils::NexusTaxa::add_taxon ),
            pybind11::arg("name")
        )
        .def(
            "block_name",
            ( std::string ( ::genesis::utils::NexusTaxa::* )(  ) const )( &::genesis::utils::NexusTaxa::block_name )
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::NexusTaxa::* )(  ))( &::genesis::utils::NexusTaxa::clear )
        )
        .def(
            "empty",
            ( bool ( ::genesis::utils::NexusTaxa::* )(  ) const )( &::genesis::utils::NexusTaxa::empty )
        )
        .def(
            "erase_taxon",
            ( void ( ::genesis::utils::NexusTaxa::* )( std::string ))( &::genesis::utils::NexusTaxa::erase_taxon ),
            pybind11::arg("name")
        )
        .def(
            "has_taxon",
            ( bool ( ::genesis::utils::NexusTaxa::* )( std::string ) const )( &::genesis::utils::NexusTaxa::has_taxon ),
            pybind11::arg("name")
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::NexusTaxa::* )(  ) const )( &::genesis::utils::NexusTaxa::size )
        )
        .def(
            "swap",
            ( void ( ::genesis::utils::NexusTaxa::* )( NexusTaxa & ))( &::genesis::utils::NexusTaxa::swap ),
            pybind11::arg("other")
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::NexusTaxa::* )( std::ostream & ) const )( &::genesis::utils::NexusTaxa::to_stream ),
            pybind11::arg("os")
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::utils::NexusTaxa& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            pybind11::keep_alive<0, 1>()
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_formats_nexus_taxa_export, ::genesis::utils, scope )
{

    scope.def(
        "operator!=",
        ( bool ( * )( NexusTaxa const &, NexusTaxa const & ))( &::genesis::utils::operator!= ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "operator==",
        ( bool ( * )( NexusTaxa const &, NexusTaxa const & ))( &::genesis::utils::operator== ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "swap",
        ( void ( * )( NexusTaxa &, NexusTaxa & ))( &::genesis::utils::swap ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );
}
