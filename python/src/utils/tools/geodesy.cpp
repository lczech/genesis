/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::GeoCoordinate, scope )
{

    // -------------------------------------------------------------------
    //     Class GeoCoordinate
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::GeoCoordinate, std::shared_ptr<::genesis::utils::GeoCoordinate> > ( scope, "GeoCoordinate" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< double, double >(),
            pybind11::arg("lat"),
            pybind11::arg("lon")
        )
        .def(
            pybind11::init< GeoCoordinate const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "latitude",
            ( GeoCoordinate & ( ::genesis::utils::GeoCoordinate::* )( double ))( &::genesis::utils::GeoCoordinate::latitude ),
            pybind11::arg("value"),
            get_docstring("GeoCoordinate & ::genesis::utils::GeoCoordinate::latitude (double value)")
        )
        .def(
            "latitude",
            ( double ( ::genesis::utils::GeoCoordinate::* )(  ) const )( &::genesis::utils::GeoCoordinate::latitude ),
            get_docstring("double ::genesis::utils::GeoCoordinate::latitude () const")
        )
        .def(
            "longitude",
            ( GeoCoordinate & ( ::genesis::utils::GeoCoordinate::* )( double ))( &::genesis::utils::GeoCoordinate::longitude ),
            pybind11::arg("value"),
            get_docstring("GeoCoordinate & ::genesis::utils::GeoCoordinate::longitude (double value)")
        )
        .def(
            "longitude",
            ( double ( ::genesis::utils::GeoCoordinate::* )(  ) const )( &::genesis::utils::GeoCoordinate::longitude ),
            get_docstring("double ::genesis::utils::GeoCoordinate::longitude () const")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::utils::GeoCoordinate const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
    ;
}
