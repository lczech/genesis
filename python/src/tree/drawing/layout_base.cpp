/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::LayoutBase, scope )
{

    // -------------------------------------------------------------------
    //     Class LayoutBase
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::LayoutBase, std::shared_ptr<::genesis::tree::LayoutBase> > ( scope, "LayoutBase" )
        // .def(
        //     pybind11::init<  >()
        // )
        // .def(
        //     pybind11::init< Tree const &, LayoutType const, bool >(),
        //     pybind11::arg("orig_tree"),
        //     pybind11::arg("drawing_type"),
        //     pybind11::arg("ladderize")=(bool)(true)
        // )
        // .def(
        //     pybind11::init< LayoutBase const & >(),
        //     pybind11::arg("arg")
        // )

        // Public Member Functions

        .def(
            "set_edge_distance_strokes",
            ( void ( ::genesis::tree::LayoutBase::* )( std::vector< utils::SvgStroke > const & ))( &::genesis::tree::LayoutBase::set_edge_distance_strokes ),
            pybind11::arg("strokes")
        )
        .def(
            "set_edge_distance_strokes",
            ( void ( ::genesis::tree::LayoutBase::* )( utils::SvgStroke const & ))( &::genesis::tree::LayoutBase::set_edge_distance_strokes ),
            pybind11::arg("stroke")
        )
        .def(
            "set_edge_shapes",
            ( void ( ::genesis::tree::LayoutBase::* )( std::vector< utils::SvgGroup > const & ))( &::genesis::tree::LayoutBase::set_edge_shapes ),
            pybind11::arg("shapes")
        )
        .def(
            "set_edge_shapes",
            ( void ( ::genesis::tree::LayoutBase::* )( utils::SvgGroup const & ))( &::genesis::tree::LayoutBase::set_edge_shapes ),
            pybind11::arg("shape")
        )
        .def(
            "set_edge_spreading_strokes",
            ( void ( ::genesis::tree::LayoutBase::* )( std::vector< utils::SvgStroke > const & ))( &::genesis::tree::LayoutBase::set_edge_spreading_strokes ),
            pybind11::arg("strokes")
        )
        .def(
            "set_edge_spreading_strokes",
            ( void ( ::genesis::tree::LayoutBase::* )( utils::SvgStroke const & ))( &::genesis::tree::LayoutBase::set_edge_spreading_strokes ),
            pybind11::arg("stroke")
        )
        .def(
            "set_edge_strokes",
            ( void ( ::genesis::tree::LayoutBase::* )( std::vector< utils::SvgStroke > const & ))( &::genesis::tree::LayoutBase::set_edge_strokes ),
            pybind11::arg("strokes")
        )
        .def(
            "set_edge_strokes",
            ( void ( ::genesis::tree::LayoutBase::* )( utils::SvgStroke const & ))( &::genesis::tree::LayoutBase::set_edge_strokes ),
            pybind11::arg("stroke")
        )
        .def(
            "set_node_shapes",
            ( void ( ::genesis::tree::LayoutBase::* )( std::vector< utils::SvgGroup > const & ))( &::genesis::tree::LayoutBase::set_node_shapes ),
            pybind11::arg("shapes")
        )
        .def(
            "set_node_shapes",
            ( void ( ::genesis::tree::LayoutBase::* )( utils::SvgGroup const & ))( &::genesis::tree::LayoutBase::set_node_shapes ),
            pybind11::arg("shape")
        )
        .def(
            "text_template",
            ( utils::SvgText & ( ::genesis::tree::LayoutBase::* )(  ))( &::genesis::tree::LayoutBase::text_template )
        )
        .def(
            "text_template",
            ( utils::SvgText const & ( ::genesis::tree::LayoutBase::* )(  ) const )( &::genesis::tree::LayoutBase::text_template )
        )
        .def(
            "text_template",
            ( void ( ::genesis::tree::LayoutBase::* )( utils::SvgText const & ))( &::genesis::tree::LayoutBase::text_template ),
            pybind11::arg("tt")
        )
        .def(
            "to_svg_document",
            ( utils::SvgDocument ( ::genesis::tree::LayoutBase::* )(  ) const )( &::genesis::tree::LayoutBase::to_svg_document )
        )
        .def(
            "tree",
            ( Tree const & ( ::genesis::tree::LayoutBase::* )(  ) const )( &::genesis::tree::LayoutBase::tree )
        )
        .def(
            "tree",
            ( void ( ::genesis::tree::LayoutBase::* )( Tree const &, bool ))( &::genesis::tree::LayoutBase::tree ),
            pybind11::arg("orig_tree"),
            pybind11::arg("ladderize")=(bool)(true)
        )
        .def(
            "type",
            ( LayoutType ( ::genesis::tree::LayoutBase::* )(  ) const )( &::genesis::tree::LayoutBase::type )
        )
        .def(
            "type",
            ( void ( ::genesis::tree::LayoutBase::* )( LayoutType const ))( &::genesis::tree::LayoutBase::type ),
            pybind11::arg("drawing_type")
        )
    ;
}
