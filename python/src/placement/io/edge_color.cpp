/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS(placement_io_edge_color_export, "placement")
{

    boost::python::def(
        "placement_color_count_gradient",
        ( std::vector< utils::Color > ( * )( Sample const &, bool ))( &::genesis::placement::placement_color_count_gradient ),
        ( boost::python::arg("smp"), boost::python::arg("linear") ),
        get_docstring("std::vector< utils::Color > ::genesis::placement::placement_color_count_gradient (Sample const & smp, bool linear)")
    );
}
