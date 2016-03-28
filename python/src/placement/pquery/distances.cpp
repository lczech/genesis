/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS(placement_pquery_distances_export, "placement")
{

    // boost::python::def(
    //     "distance",
    //     ( double ( * )( const Pquery &, const Pquery &, const utils::Matrix< double > & ))( &::genesis::placement::distance ),
    //     ( boost::python::arg("pqry_a"), boost::python::arg("pqry_b"), boost::python::arg("node_distance_matrix") )
    // );
    //
    // boost::python::def(
    //     "distance",
    //     ( double ( * )( const Pquery &, const PlacementTreeNode &, const utils::Matrix< double > & ))( &::genesis::placement::distance ),
    //     ( boost::python::arg("pquery"), boost::python::arg("node"), boost::python::arg("node_distance_matrix") )
    // );
    //
    // boost::python::def(
    //     "distance",
    //     ( double ( * )( const PqueryPlacement &, const PlacementTreeNode &, const utils::Matrix< double > & ))( &::genesis::placement::distance ),
    //     ( boost::python::arg("placement"), boost::python::arg("node"), boost::python::arg("node_distance_matrix") )
    // );
    //
    // boost::python::def(
    //     "distance",
    //     ( double ( * )( const PqueryPlacement &, const Pquery &, const utils::Matrix< double > & ))( &::genesis::placement::distance ),
    //     ( boost::python::arg("placement"), boost::python::arg("pquery"), boost::python::arg("node_distance_matrix") )
    // );
    //
    // boost::python::def(
    //     "distance",
    //     ( double ( * )( const PqueryPlacement &, const PqueryPlacement &, const utils::Matrix< double > & ))( &::genesis::placement::distance ),
    //     ( boost::python::arg("placement_a"), boost::python::arg("placement_b"), boost::python::arg("node_distance_matrix") )
    // );
    //
    // boost::python::def(
    //     "distance",
    //     ( double ( * )( const PqueryPlain &, const PqueryPlain &, const utils::Matrix< double > & ))( &::genesis::placement::distance ),
    //     ( boost::python::arg("pqry_a"), boost::python::arg("pqry_b"), boost::python::arg("node_distance_matrix") )
    // );
}
