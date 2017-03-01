/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::RectangularLayout, scope )
{

    // -------------------------------------------------------------------
    //     Class RectangularLayout
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::RectangularLayout, std::shared_ptr<::genesis::tree::RectangularLayout> > ( scope, "RectangularLayout" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< Tree const & >(),
            pybind11::arg("tree")
        )
        .def(
            pybind11::init< RectangularLayout const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "set_edge_strokes",
            ( void ( ::genesis::tree::RectangularLayout::* )( std::vector< utils::SvgStroke > ))( &::genesis::tree::RectangularLayout::set_edge_strokes ),
            pybind11::arg("strokes")
        )
        .def(
            "to_svg_document",
            ( utils::SvgDocument ( ::genesis::tree::RectangularLayout::* )(  ) const )( &::genesis::tree::RectangularLayout::to_svg_document )
        )
        .def(
            "tree",
            ( Tree & ( ::genesis::tree::RectangularLayout::* )(  ))( &::genesis::tree::RectangularLayout::tree )
        )
    ;
}
