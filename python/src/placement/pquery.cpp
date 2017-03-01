/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS( ::genesis::placement::Pquery, scope )
{

    // -------------------------------------------------------------------
    //     Class Pquery
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::Pquery, std::shared_ptr<::genesis::placement::Pquery> > ( scope, "Pquery" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< Pquery const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "add_name",
            ( PqueryName & ( ::genesis::placement::Pquery::* )( PqueryName const & ))( &::genesis::placement::Pquery::add_name ),
            pybind11::arg("other")
        )
        .def(
            "add_name",
            ( PqueryName & ( ::genesis::placement::Pquery::* )( std::string, double ))( &::genesis::placement::Pquery::add_name ),
            pybind11::arg("name")=(std::string)(""),
            pybind11::arg("multiplicity")=(double)(1.0)
        )
        .def(
            "add_placement",
            ( PqueryPlacement & ( ::genesis::placement::Pquery::* )( PlacementTreeEdge & ))( &::genesis::placement::Pquery::add_placement ),
            pybind11::arg("edge")
        )
        .def(
            "add_placement",
            ( PqueryPlacement & ( ::genesis::placement::Pquery::* )( PlacementTreeEdge &, PqueryPlacement const & ))( &::genesis::placement::Pquery::add_placement ),
            pybind11::arg("edge"),
            pybind11::arg("val")
        )
        .def(
            "add_placement",
            ( PqueryPlacement & ( ::genesis::placement::Pquery::* )( PqueryPlacement const & ))( &::genesis::placement::Pquery::add_placement ),
            pybind11::arg("val")
        )
        .def(
            "clear",
            ( void ( ::genesis::placement::Pquery::* )(  ))( &::genesis::placement::Pquery::clear )
        )
        .def(
            "clear_names",
            ( void ( ::genesis::placement::Pquery::* )(  ))( &::genesis::placement::Pquery::clear_names )
        )
        .def(
            "clear_placements",
            ( void ( ::genesis::placement::Pquery::* )(  ))( &::genesis::placement::Pquery::clear_placements )
        )
        .def(
            "name_at",
            ( PqueryName & ( ::genesis::placement::Pquery::* )( size_t ))( &::genesis::placement::Pquery::name_at ),
            pybind11::arg("index")
        )
        .def(
            "name_at",
            ( PqueryName const & ( ::genesis::placement::Pquery::* )( size_t ) const )( &::genesis::placement::Pquery::name_at ),
            pybind11::arg("index")
        )
        .def(
            "name_size",
            ( size_t ( ::genesis::placement::Pquery::* )(  ) const )( &::genesis::placement::Pquery::name_size )
        )
        // .def(
        //     "names",
        //     ( utils::Range< const_iterator_names > ( ::genesis::placement::Pquery::* )(  ) const )( &::genesis::placement::Pquery::names )
        // )
        // .def(
        //     "names",
        //     ( utils::Range< iterator_names > ( ::genesis::placement::Pquery::* )(  ))( &::genesis::placement::Pquery::names )
        // )
        .def(
            "placement_at",
            ( PqueryPlacement & ( ::genesis::placement::Pquery::* )( size_t ))( &::genesis::placement::Pquery::placement_at ),
            pybind11::arg("index")
        )
        .def(
            "placement_at",
            ( PqueryPlacement const & ( ::genesis::placement::Pquery::* )( size_t ) const )( &::genesis::placement::Pquery::placement_at ),
            pybind11::arg("index")
        )
        .def(
            "placement_size",
            ( size_t ( ::genesis::placement::Pquery::* )(  ) const )( &::genesis::placement::Pquery::placement_size )
        )
        // .def(
        //     "placements",
        //     ( utils::Range< const_iterator_placements > ( ::genesis::placement::Pquery::* )(  ) const )( &::genesis::placement::Pquery::placements )
        // )
        // .def(
        //     "placements",
        //     ( utils::Range< iterator_placements > ( ::genesis::placement::Pquery::* )(  ))( &::genesis::placement::Pquery::placements )
        // )
        .def(
            "remove_name_at",
            ( void ( ::genesis::placement::Pquery::* )( size_t ))( &::genesis::placement::Pquery::remove_name_at ),
            pybind11::arg("index")
        )
        .def(
            "remove_placement_at",
            ( void ( ::genesis::placement::Pquery::* )( size_t ))( &::genesis::placement::Pquery::remove_placement_at ),
            pybind11::arg("index")
        )

        // Iterators

        .def(
            "names",
            []( ::genesis::placement::Pquery& obj ){
                return pybind11::make_iterator( obj.begin_names(), obj.end_names() );            }
        )
        .def(
            "placements",
            []( ::genesis::placement::Pquery& obj ){
                return pybind11::make_iterator( obj.begin_placements(), obj.end_placements() );            }
        )
    ;
}
