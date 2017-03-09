/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::HistogramAccumulator, scope )
{

    // -------------------------------------------------------------------
    //     Class HistogramAccumulator
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::HistogramAccumulator, std::shared_ptr<::genesis::utils::HistogramAccumulator> > ( scope, "HistogramAccumulator" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< const std::vector< double > &, double >(),
            pybind11::arg("values"),
            pybind11::arg("weight")=(double)(1.0)
        )
        .def(
            pybind11::init< const std::vector< std::pair< double, double >> & >(),
            pybind11::arg("weighted_values")
        )
        .def(
            pybind11::init< HistogramAccumulator const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "accumulate",
            ( void ( ::genesis::utils::HistogramAccumulator::* )( double, double ))( &::genesis::utils::HistogramAccumulator::accumulate ),
            pybind11::arg("x"),
            pybind11::arg("weight")
        )
        .def(
            "added_values",
            ( size_t ( ::genesis::utils::HistogramAccumulator::* )(  ) const )( &::genesis::utils::HistogramAccumulator::added_values )
        )
        .def(
            "build_uniform_ranges_histogram",
            ( Histogram ( ::genesis::utils::HistogramAccumulator::* )( size_t, bool ) const )( &::genesis::utils::HistogramAccumulator::build_uniform_ranges_histogram ),
            pybind11::arg("num_bins"),
            pybind11::arg("integer_ranges")=(bool)(false)
        )
        .def(
            "build_uniform_ranges_histogram",
            ( Histogram ( ::genesis::utils::HistogramAccumulator::* )( size_t, double, double ) const )( &::genesis::utils::HistogramAccumulator::build_uniform_ranges_histogram ),
            pybind11::arg("num_bins"),
            pybind11::arg("min"),
            pybind11::arg("max")
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::HistogramAccumulator::* )(  ))( &::genesis::utils::HistogramAccumulator::clear )
        )
        .def(
            "empty",
            ( bool ( ::genesis::utils::HistogramAccumulator::* )(  ) const )( &::genesis::utils::HistogramAccumulator::empty )
        )
        .def(
            "increment",
            ( void ( ::genesis::utils::HistogramAccumulator::* )( double ))( &::genesis::utils::HistogramAccumulator::increment ),
            pybind11::arg("x")
        )
        .def(
            "max",
            ( double ( ::genesis::utils::HistogramAccumulator::* )(  ) const )( &::genesis::utils::HistogramAccumulator::max )
        )
        .def(
            "min",
            ( double ( ::genesis::utils::HistogramAccumulator::* )(  ) const )( &::genesis::utils::HistogramAccumulator::min )
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::HistogramAccumulator::* )(  ) const )( &::genesis::utils::HistogramAccumulator::size )
        )
        .def(
            "swap",
            ( void ( ::genesis::utils::HistogramAccumulator::* )( HistogramAccumulator & ))( &::genesis::utils::HistogramAccumulator::swap ),
            pybind11::arg("other")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::utils::HistogramAccumulator const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::utils::HistogramAccumulator& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            pybind11::keep_alive<0, 1>()
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_math_histogram_accumulator_export, ::genesis::utils, scope )
{

    scope.def(
        "swap",
        ( void ( * )( HistogramAccumulator &, HistogramAccumulator & ))( &::genesis::utils::swap ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );
}
