/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS (HistogramAccumulator, "utils")
{

    // -------------------------------------------------------------------
    //     Class HistogramAccumulator
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::HistogramAccumulator > ( "HistogramAccumulator", boost::python::init<  >(  ) )
        .def( boost::python::init< const std::vector< double > &, boost::python::optional< double > >(( boost::python::arg("values"), boost::python::arg("weight")=(double)(1.0) )) )
        .def( boost::python::init< const std::vector< std::pair< double, double >> & >(( boost::python::arg("weighted_values") )) )
        .def( boost::python::init< HistogramAccumulator const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "accumulate",
            ( void ( ::genesis::utils::HistogramAccumulator::* )( double, double ))( &::genesis::utils::HistogramAccumulator::accumulate ),
            ( boost::python::arg("x"), boost::python::arg("weight") )
        )
        .def(
            "build_uniform_ranges_histogram",
            ( Histogram ( ::genesis::utils::HistogramAccumulator::* )( size_t, bool ) const )( &::genesis::utils::HistogramAccumulator::build_uniform_ranges_histogram ),
            ( boost::python::arg("num_bins"), boost::python::arg("integer_ranges")=(bool)(false) )
        )
        .def(
            "build_uniform_ranges_histogram",
            ( Histogram ( ::genesis::utils::HistogramAccumulator::* )( size_t, double, double ) const )( &::genesis::utils::HistogramAccumulator::build_uniform_ranges_histogram ),
            ( boost::python::arg("num_bins"), boost::python::arg("min"), boost::python::arg("max") )
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
            ( boost::python::arg("x") )
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
            ( boost::python::arg("other") )
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::utils::HistogramAccumulator::begin, &::genesis::utils::HistogramAccumulator::end )
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_math_histogram_accumulator_export, "utils")
{

    boost::python::def(
        "swap",
        ( void ( * )( HistogramAccumulator &, HistogramAccumulator & ))( &::genesis::utils::swap ),
        ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    );
}
