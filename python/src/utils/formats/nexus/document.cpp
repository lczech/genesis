/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::NexusDocument, scope )
{

    // -------------------------------------------------------------------
    //     Class NexusDocument
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::NexusDocument, std::shared_ptr<::genesis::utils::NexusDocument> > ( scope, "NexusDocument" )
        .def(
            pybind11::init<  >()
        )

        // Public Member Functions

        .def(
            "get_block",
            ( NexusBlock * ( ::genesis::utils::NexusDocument::* )( std::string ))( &::genesis::utils::NexusDocument::get_block ),
            pybind11::arg("block_name")
        )
        .def(
            "get_block",
            ( NexusBlock const * ( ::genesis::utils::NexusDocument::* )( std::string ) const )( &::genesis::utils::NexusDocument::get_block ),
            pybind11::arg("block_name")
        )
        .def(
            "has_block",
            ( bool ( ::genesis::utils::NexusDocument::* )( std::string ) const )( &::genesis::utils::NexusDocument::has_block ),
            pybind11::arg("block_name")
        )
        // .def(
        //     "set_block",
        //     ( NexusBlock * ( ::genesis::utils::NexusDocument::* )( std::unique_ptr< NexusBlock > ))( &::genesis::utils::NexusDocument::set_block ),
        //     pybind11::arg("block")
        // )

        // Iterators

        // .def(
        //     "__iter__",
        //     []( ::genesis::utils::NexusDocument& obj ){
        //         return pybind11::make_iterator( obj.begin(), obj.end() );
        //     },
        //     pybind11::keep_alive<0, 1>()
        // )
    ;
}
