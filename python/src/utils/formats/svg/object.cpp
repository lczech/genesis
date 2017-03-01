/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;
/*
PYTHON_EXPORT_CLASS( ::genesis::utils::SvgObject, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgObject
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgObject, std::shared_ptr<::genesis::utils::SvgObject> > ( scope, "SvgObject" )
        .def(
            pybind11::init< T const & >(),
            pybind11::arg("object")
        )
        .def(
            pybind11::init< SvgObject const & >(),
            pybind11::arg("other")
        )

        // Public Member Functions

        .def(
            "bounding_box",
            ( SvgBox ( ::genesis::utils::SvgObject::* )(  ) const )( &::genesis::utils::SvgObject::bounding_box )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::SvgObject::* )( std::ostream &, size_t, SvgDrawingOptions const & ) const )( &::genesis::utils::SvgObject::write ),
            pybind11::arg("out"),
            pybind11::arg("indent")=(size_t)(0),
            pybind11::arg("options")
        )
    ;
}
*/
