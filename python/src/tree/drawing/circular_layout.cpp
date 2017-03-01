/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::CircularLayout, scope )
{

    // -------------------------------------------------------------------
    //     Class CircularLayout
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::CircularLayout, std::shared_ptr<::genesis::tree::CircularLayout> > ( scope, "CircularLayout" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< Tree const & >(),
            pybind11::arg("tree")
        )
        .def(
            pybind11::init< CircularLayout const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "set_edge_strokes",
            ( void ( ::genesis::tree::CircularLayout::* )( std::vector< utils::SvgStroke > ))( &::genesis::tree::CircularLayout::set_edge_strokes ),
            pybind11::arg("strokes")
        )
        .def(
            "to_svg_document",
            ( utils::SvgDocument ( ::genesis::tree::CircularLayout::* )(  ) const )( &::genesis::tree::CircularLayout::to_svg_document )
        )
        .def(
            "tree",
            ( Tree & ( ::genesis::tree::CircularLayout::* )(  ))( &::genesis::tree::CircularLayout::tree )
        )
    ;
}
