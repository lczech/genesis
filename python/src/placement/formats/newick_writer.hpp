/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename Base>
void PythonExportClass_::genesis::placement::PlacementTreeNewickWriterMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class PlacementTreeNewickWriterMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::placement;

    using PlacementTreeNewickWriterMixinType = PlacementTreeNewickWriterMixin<typename Base>;

    pybind11::class_< PlacementTreeNewickWriterMixinType, std::shared_ptr<PlacementTreeNewickWriterMixinType> > ( scope, name.c_str() )

        // Public Member Functions

        .def(
            "enable_edge_nums",
            ( bool ( PlacementTreeNewickWriterMixinType::* )(  ) const )( &PlacementTreeNewickWriterMixinType::enable_edge_nums )
        )
        .def(
            "enable_edge_nums",
            ( void ( PlacementTreeNewickWriterMixinType::* )( bool ))( &PlacementTreeNewickWriterMixinType::enable_edge_nums ),
            pybind11::arg("value")
        )
        .def(
            "enable_placement_counts",
            ( bool ( PlacementTreeNewickWriterMixinType::* )(  ) const )( &PlacementTreeNewickWriterMixinType::enable_placement_counts )
        )
        .def(
            "enable_placement_counts",
            ( void ( PlacementTreeNewickWriterMixinType::* )( bool ))( &PlacementTreeNewickWriterMixinType::enable_placement_counts ),
            pybind11::arg("value")
        )
        .def(
            "prepare_sample",
            ( void ( PlacementTreeNewickWriterMixinType::* )( Sample const & ))( &PlacementTreeNewickWriterMixinType::prepare_sample ),
            pybind11::arg("smp")
        )
    ;
}
