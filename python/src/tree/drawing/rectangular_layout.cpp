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
            pybind11::init< Tree const &, LayoutType const, bool >(),
            pybind11::arg("orig_tree"),
            pybind11::arg("drawing_type"),
            pybind11::arg("ladderize")=(bool)(true)
        )
        .def(
            pybind11::init< RectangularLayout const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "height",
            ( RectangularLayout & ( ::genesis::tree::RectangularLayout::* )( double const ))( &::genesis::tree::RectangularLayout::height ),
            pybind11::arg("value")
        )
        .def(
            "height",
            ( double ( ::genesis::tree::RectangularLayout::* )(  ) const )( &::genesis::tree::RectangularLayout::height )
        )
        .def(
            "width",
            ( RectangularLayout & ( ::genesis::tree::RectangularLayout::* )( double const ))( &::genesis::tree::RectangularLayout::width ),
            pybind11::arg("value")
        )
        .def(
            "width",
            ( double ( ::genesis::tree::RectangularLayout::* )(  ) const )( &::genesis::tree::RectangularLayout::width )
        )
    ;
}
