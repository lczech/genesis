/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgGroup, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgGroup
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgGroup, std::shared_ptr<::genesis::utils::SvgGroup> > ( scope, "SvgGroup" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< SvgGroup const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        // .def(
        //     "add",
        //     ( ::genesis::utils::SvgGroup & ( ::genesis::utils::SvgGroup::* )( SvgObject && ))( &::genesis::utils::SvgGroup::add ),
        //     pybind11::arg("object")
        // )
        .def(
            "add",
            ( ::genesis::utils::SvgGroup & ( ::genesis::utils::SvgGroup::* )( SvgObject const & ))( &::genesis::utils::SvgGroup::add ),
            pybind11::arg("object"),
            get_docstring("self_type & ::genesis::utils::SvgGroup::add (SvgObject const & object)")
        )
        .def(
            "bounding_box",
            ( SvgBox ( ::genesis::utils::SvgGroup::* )(  ) const )( &::genesis::utils::SvgGroup::bounding_box )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::SvgGroup::* )( std::ostream &, size_t, SvgDrawingOptions const & ) const )( &::genesis::utils::SvgGroup::write ),
            pybind11::arg("out"),
            pybind11::arg("indent")=(size_t)(0),
            pybind11::arg("options")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::utils::SvgGroup const& obj ) -> std::string {
                std::ostringstream s;
                obj.write(s);
                return s.str();
            }
        )
        // .def(
        //     "__str__",
        //     []( ::genesis::utils::SvgGroup const& obj ) -> std::string {
        //         std::ostringstream s;
        //         s << obj;
        //         return s.str();
        //     }
        // )
    ;
}
