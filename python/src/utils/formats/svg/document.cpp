/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::SvgDocument, scope )
{

    // -------------------------------------------------------------------
    //     Class SvgDocument
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::SvgDocument, std::shared_ptr<::genesis::utils::SvgDocument> > ( scope, "SvgDocument" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< SvgDocument const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        // .def(
        //     "add",
        //     ( self_type & ( ::genesis::utils::SvgDocument::* )( SvgObject && ))( &::genesis::utils::SvgDocument::add ),
        //     pybind11::arg("object")
        // )
        .def(
            "add",
            ( ::genesis::utils::SvgDocument & ( ::genesis::utils::SvgDocument::* )( SvgObject const & ))( &::genesis::utils::SvgDocument::add ),
            pybind11::arg("object"),
            get_docstring("self_type & ::genesis::utils::SvgDocument::add (SvgObject const & object)")
        )
        .def(
            "bounding_box",
            ( SvgBox ( ::genesis::utils::SvgDocument::* )(  ) const )( &::genesis::utils::SvgDocument::bounding_box )
        )
        .def(
            "write",
            ( void ( ::genesis::utils::SvgDocument::* )( std::ostream & ) const )( &::genesis::utils::SvgDocument::write ),
            pybind11::arg("out")
        )

        // Operators

        // .def(
        //     "__str__",
        //     []( ::genesis::utils::SvgDocument const& obj ) -> std::string {
        //         std::ostringstream s;
        //         s << obj;
        //         return s.str();
        //     }
        // )
        .def(
            "__str__",
            []( ::genesis::utils::SvgDocument const& obj ) -> std::string {
                std::ostringstream s;
                obj.write(s);
                return s.str();
            }
        )
    ;
}
