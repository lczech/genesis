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
PYTHON_EXPORT_CLASS (NexusBlock, "utils")
{

    // -------------------------------------------------------------------
    //     Class NexusBlock
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::NexusBlock > ( "NexusBlock", boost::python::init<  >(  ) )
        .def( boost::python::init< NexusBlock const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "block_name",
            ( std::string ( ::genesis::utils::NexusBlock::* )(  ) const )( &::genesis::utils::NexusBlock::block_name )
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::NexusBlock::* )( std::ostream & ) const )( &::genesis::utils::NexusBlock::to_stream ),
            ( boost::python::arg("os") )
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_io_nexus_block_export, "utils")
{

    boost::python::def(
        "operator<<",
        ( std::ostream & ( * )( std::ostream &, NexusBlock const & ))( &::genesis::utils::operator<< ),
        ( boost::python::arg("os"), boost::python::arg("block") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>()
    );
}
*/
