/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename Base>
void PythonExportClass_PlacementTreeNewickWriterMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class PlacementTreeNewickWriterMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::placement;

    using PlacementTreeNewickWriterMixinType = PlacementTreeNewickWriterMixin<typename Base>;

    boost::python::class_< PlacementTreeNewickWriterMixinType > ( name.c_str() )

        // Public Member Functions

        .def(
            "enable_edge_nums",
            ( bool ( PlacementTreeNewickWriterMixinType::* )(  ) const )( &PlacementTreeNewickWriterMixinType::enable_edge_nums )
        )
        .def(
            "enable_edge_nums",
            ( void ( PlacementTreeNewickWriterMixinType::* )( bool ))( &PlacementTreeNewickWriterMixinType::enable_edge_nums ),
            ( boost::python::arg("value") )
        )
        .def(
            "enable_placement_counts",
            ( bool ( PlacementTreeNewickWriterMixinType::* )(  ) const )( &PlacementTreeNewickWriterMixinType::enable_placement_counts )
        )
        .def(
            "enable_placement_counts",
            ( void ( PlacementTreeNewickWriterMixinType::* )( bool ))( &PlacementTreeNewickWriterMixinType::enable_placement_counts ),
            ( boost::python::arg("value") )
        )
        .def(
            "prepare_sample",
            ( void ( PlacementTreeNewickWriterMixinType::* )( Sample const & ))( &PlacementTreeNewickWriterMixinType::prepare_sample ),
            ( boost::python::arg("smp") )
        )
    ;
}
