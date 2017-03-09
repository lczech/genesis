/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::Histogram, scope )
{

    // -------------------------------------------------------------------
    //     Class Histogram
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::Histogram, std::shared_ptr<::genesis::utils::Histogram> > ( scope, "Histogram" )
        .def(
            pybind11::init< size_t >(),
            pybind11::arg("num_bins")
        )
        .def(
            pybind11::init< size_t, double, double >(),
            pybind11::arg("num_bins"),
            pybind11::arg("range_min"),
            pybind11::arg("range_max")
        )
        .def(
            pybind11::init< const std::vector< double > & >(),
            pybind11::arg("ranges")
        )
        .def(
            pybind11::init< Histogram const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "accumulate",
            ( int ( ::genesis::utils::Histogram::* )( double, double ))( &::genesis::utils::Histogram::accumulate ),
            pybind11::arg("x"),
            pybind11::arg("weight")
        )
        .def(
            "accumulate_bin",
            ( void ( ::genesis::utils::Histogram::* )( size_t, double ))( &::genesis::utils::Histogram::accumulate_bin ),
            pybind11::arg("bin"),
            pybind11::arg("weight")
        )
        .def(
            "at",
            ( double & ( ::genesis::utils::Histogram::* )( size_t ))( &::genesis::utils::Histogram::at ),
            pybind11::arg("bin_num")
        )
        .def(
            "at",
            ( double ( ::genesis::utils::Histogram::* )( size_t ) const )( &::genesis::utils::Histogram::at ),
            pybind11::arg("bin_num")
        )
        .def(
            "bin_midpoint",
            ( double ( ::genesis::utils::Histogram::* )( size_t ) const )( &::genesis::utils::Histogram::bin_midpoint ),
            pybind11::arg("bin_num")
        )
        .def(
            "bin_range",
            ( std::pair< double, double > ( ::genesis::utils::Histogram::* )( size_t ) const )( &::genesis::utils::Histogram::bin_range ),
            pybind11::arg("bin_num")
        )
        .def(
            "bin_width",
            ( double ( ::genesis::utils::Histogram::* )( size_t ) const )( &::genesis::utils::Histogram::bin_width ),
            pybind11::arg("bin_num")
        )
        .def(
            "bins",
            ( size_t ( ::genesis::utils::Histogram::* )(  ) const )( &::genesis::utils::Histogram::bins )
        )
        // .def(
        //     "cbegin",
        //     ( const_iterator ( ::genesis::utils::Histogram::* )(  ) const )( &::genesis::utils::Histogram::cbegin )
        // )
        // .def(
        //     "cend",
        //     ( const_iterator ( ::genesis::utils::Histogram::* )(  ) const )( &::genesis::utils::Histogram::cend )
        // )
        .def(
            "check_range",
            ( bool ( ::genesis::utils::Histogram::* )( double ) const )( &::genesis::utils::Histogram::check_range ),
            pybind11::arg("x")
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::Histogram::* )(  ))( &::genesis::utils::Histogram::clear )
        )
        .def(
            "find_bin",
            ( int ( ::genesis::utils::Histogram::* )( double ) const )( &::genesis::utils::Histogram::find_bin ),
            pybind11::arg("x")
        )
        .def(
            "increment",
            ( int ( ::genesis::utils::Histogram::* )( double ))( &::genesis::utils::Histogram::increment ),
            pybind11::arg("x")
        )
        .def(
            "increment_bin",
            ( void ( ::genesis::utils::Histogram::* )( size_t ))( &::genesis::utils::Histogram::increment_bin ),
            pybind11::arg("bin")
        )
        // .def(
        //     "out_of_range_behaviour",
        //     ( OutOfRangeBehaviour ( ::genesis::utils::Histogram::* )(  ) const )( &::genesis::utils::Histogram::out_of_range_behaviour )
        // )
        // .def(
        //     "out_of_range_behaviour",
        //     ( void ( ::genesis::utils::Histogram::* )( OutOfRangeBehaviour ))( &::genesis::utils::Histogram::out_of_range_behaviour ),
        //     pybind11::arg("v")
        // )
        .def(
            "range_max",
            ( double ( ::genesis::utils::Histogram::* )(  ) const )( &::genesis::utils::Histogram::range_max )
        )
        .def(
            "range_min",
            ( double ( ::genesis::utils::Histogram::* )(  ) const )( &::genesis::utils::Histogram::range_min )
        )
        .def(
            "set_ranges",
            ( void ( ::genesis::utils::Histogram::* )( const std::vector< double > & ))( &::genesis::utils::Histogram::set_ranges ),
            pybind11::arg("ranges")
        )
        .def(
            "set_uniform_ranges",
            ( void ( ::genesis::utils::Histogram::* )( const double, const double ))( &::genesis::utils::Histogram::set_uniform_ranges ),
            pybind11::arg("min"),
            pybind11::arg("max")
        )

        // Operators

        // .def( pybind11::self == pybind11::self )
        .def(
            "__getitem__",
            ( double & ( ::genesis::utils::Histogram::* )( size_t ))( &::genesis::utils::Histogram::operator[] ),
            pybind11::arg("bin_num")
        )
        .def(
            "__getitem__",
            ( double ( ::genesis::utils::Histogram::* )( size_t ) const )( &::genesis::utils::Histogram::operator[] ),
            pybind11::arg("bin_num")
        )
        .def(
            "__str__",
            []( ::genesis::utils::Histogram const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::utils::Histogram& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            pybind11::keep_alive<0, 1>()
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_math_histogram_export, ::genesis::utils, scope )
{

    scope.def(
        "equal_ranges",
        ( bool ( * )( Histogram const &, Histogram const & ))( &::genesis::utils::equal_ranges ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "swap",
        ( void ( * )( Histogram &, Histogram & ))( &::genesis::utils::swap ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );
}
