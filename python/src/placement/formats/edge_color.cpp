/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS( placement_formats_edge_color_export, ::genesis::placement, scope )
{

    scope.def(
        "placement_color_count_gradient",
        ( std::vector< utils::Color > ( * )( Sample const &, bool ))( &::genesis::placement::placement_color_count_gradient ),
            pybind11::arg("smp"),
            pybind11::arg("linear")=(bool)(false)
    );
}
