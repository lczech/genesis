/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;
/*
PYTHON_EXPORT_CLASS( ::genesis::utils::NexusBlock, scope )
{

    // -------------------------------------------------------------------
    //     Class NexusBlock
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::NexusBlock, std::shared_ptr<::genesis::utils::NexusBlock> > ( scope, "NexusBlock" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< NexusBlock const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "block_name",
            ( std::string ( ::genesis::utils::NexusBlock::* )(  ) const )( &::genesis::utils::NexusBlock::block_name )
        )
        .def(
            "to_stream",
            ( void ( ::genesis::utils::NexusBlock::* )( std::ostream & ) const )( &::genesis::utils::NexusBlock::to_stream ),
            pybind11::arg("os")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::utils::NexusBlock const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
    ;
}
*/
