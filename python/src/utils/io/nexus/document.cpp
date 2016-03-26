/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

/*
PYTHON_EXPORT_CLASS (NexusDocument, "utils")
{

    // -------------------------------------------------------------------
    //     Class NexusDocument
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::NexusDocument > ( "NexusDocument", boost::python::init<  >(  ) )
        .def( boost::python::init< NexusDocument const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "get_block",
            ( NexusBlock * ( ::genesis::utils::NexusDocument::* )( std::string ))( &::genesis::utils::NexusDocument::get_block ),
            ( boost::python::arg("block_name") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "get_block",
            ( NexusBlock const * ( ::genesis::utils::NexusDocument::* )( std::string ) const )( &::genesis::utils::NexusDocument::get_block ),
            ( boost::python::arg("block_name") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "has_block",
            ( bool ( ::genesis::utils::NexusDocument::* )( std::string ) const )( &::genesis::utils::NexusDocument::has_block ),
            ( boost::python::arg("block_name") )
        )
        .def(
            "set_block",
            ( NexusBlock * ( ::genesis::utils::NexusDocument::* )( std::unique_ptr< NexusBlock > ))( &::genesis::utils::NexusDocument::set_block ),
            ( boost::python::arg("block") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::utils::NexusDocument::begin, &::genesis::utils::NexusDocument::end )
        )
    ;
}
*/
