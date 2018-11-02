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
            pybind11::init< Tree const &, LayoutType const, bool >(),
            pybind11::arg("orig_tree"),
            pybind11::arg("drawing_type"),
            pybind11::arg("ladderize")=(bool)(true)
        )
        .def(
            pybind11::init< CircularLayout const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "radius",
            ( CircularLayout & ( ::genesis::tree::CircularLayout::* )( double const ))( &::genesis::tree::CircularLayout::radius ),
            pybind11::arg("value")
        )
        .def(
            "radius",
            ( double ( ::genesis::tree::CircularLayout::* )(  ) const )( &::genesis::tree::CircularLayout::radius )
        )
    ;
}
