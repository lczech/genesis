#include <genesis/utils/math/binomial.hpp>
#include <genesis/utils/math/common.hpp>
#include <genesis/utils/math/compensated_sum.hpp>
#include <sstream> // __str__

#include <functional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

#ifndef BINDER_PYBIND11_TYPE_CASTER
#    define BINDER_PYBIND11_TYPE_CASTER
PYBIND11_DECLARE_HOLDER_TYPE( T, std::shared_ptr<T>, false )
PYBIND11_DECLARE_HOLDER_TYPE( T, T*, false )
PYBIND11_MAKE_OPAQUE( std::shared_ptr<void> )
#endif

void bind_genesis_utils_math_binomial(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::utils::math::log_factorial(unsigned long) file:genesis/utils/math/binomial.hpp
    // line:53 function-signature: double genesis::utils::math::log_factorial(unsigned
    // long)(unsigned long) file:genesis/utils/math/binomial.hpp line:53
    M( "genesis::utils::math" )
        .def(
            "log_factorial",
            ( double ( * )( unsigned long ) ) & genesis::utils::math::log_factorial,
            "Return the logarithm (base e) of the factorial of  that is `log(n!)`.\n\n This is "
            "implemented via a precise lookup table for `n < 1024`, and uses Stirling's "
            "approximation\n for larger values, with a slight error in the order of less than "
            "1/(360 * n^3), that is,\n less than 2.59e-12 for all `n >= 1024`.\n See "
            "https://www.johndcook.com/blog/2010/08/16/how-to-compute-log-factorial/ for "
            "details.\n\nC++: genesis::utils::math::log_factorial(unsigned long) --> double",
            pybind11::arg( "n" )
        );

    // genesis::utils::math::binomial_coefficient_int(unsigned long, unsigned long)
    // file:genesis/utils/math/binomial.hpp line:61 function-signature: unsigned long
    // genesis::utils::math::binomial_coefficient_int(unsigned long, unsigned long)(unsigned long,
    // unsigned long) file:genesis/utils/math/binomial.hpp line:61
    M( "genesis::utils::math" )
        .def(
            "binomial_coefficient_int",
            ( unsigned long ( * )( unsigned long, unsigned long ) ) &
                genesis::utils::math::binomial_coefficient_int,
            "Compute the binomial coefficient, that is  choose  for two integer numbers.\n\n The "
            "function throws for invalid arguments (n or k equal to 0, or k larger than n),\n or "
            "on overflow. For all `n < 63`, this does not overflow with 64 bit numbers.\n\nC++: "
            "genesis::utils::math::binomial_coefficient_int(unsigned long, unsigned long) --> "
            "unsigned long",
            pybind11::arg( "n" ),
            pybind11::arg( "k" )
        );

    // genesis::utils::math::log_binomial_coefficient(unsigned long, unsigned long)
    // file:genesis/utils/math/binomial.hpp line:72 function-signature: double
    // genesis::utils::math::log_binomial_coefficient(unsigned long, unsigned long)(unsigned long,
    // unsigned long) file:genesis/utils/math/binomial.hpp line:72
    M( "genesis::utils::math" )
        .def(
            "log_binomial_coefficient",
            ( double ( * )( unsigned long, unsigned long ) ) &
                genesis::utils::math::log_binomial_coefficient,
            "Compute the logarithm (base e) of the binomial coefficient, that is  choose \n for "
            "two integer numbers, for large numbers.\n\n See binomial_coefficient() for details. "
            "This function here is used in the internal\n computatation, and offered for cases "
            "where working with binomials in log space is more\n convenient due to the handling of "
            "numbers with very large and very small exponents,\n such as in "
            "binomial_distribution().\n\nC++: "
            "genesis::utils::math::log_binomial_coefficient(unsigned long, unsigned long) --> "
            "double",
            pybind11::arg( "n" ),
            pybind11::arg( "k" )
        );

    // genesis::utils::math::binomial_coefficient(unsigned long, unsigned long)
    // file:genesis/utils/math/binomial.hpp line:100 function-signature: double
    // genesis::utils::math::binomial_coefficient(unsigned long, unsigned long)(unsigned long,
    // unsigned long) file:genesis/utils/math/binomial.hpp line:100
    M( "genesis::utils::math" )
        .def(
            "binomial_coefficient",
            ( double ( * )( unsigned long, unsigned long ) ) &
                genesis::utils::math::binomial_coefficient,
            "Compute the binomial coefficient, that is  choose  for two integer numbers,\n for "
            "large numbers.\n\n The precise integer function binomial_coefficient_int() can only "
            "handle values up to `n == 62`\n (for non-small `k` at least). That is, at `n == 63, k "
            "== 29`, we have exhausted the range\n of 64bit numbers.\n\n To also be able to work "
            "with larger  and  we here offer an approximation that returns\n a `double` value "
            "instead, and hence can compute binomial coefficients of up to `n == 1029`\n "
            "(exclusive) for all values of `k`, which for `k == n/2` is about as large as double "
            "precision\n allows. The approximation is not quite up to double precision, and "
            "returns values with an\n error roughly in the range of e-12 for large numbers.\n\n "
            "For `n > 1029`, we attempt to compute this, which will work for smaller values of "
            "`k`. In case\n this overflows the `double` precision range, positive infinity is "
            "returned instead.\n The first such combination is `n==1030` and `k==500`, causing "
            "`inf` to be returned.\n\n The returned values for all  and  that are also valid with "
            "the integer version\n binomial_coefficient_int() yield identical results (up to float "
            "precision). That is, in particular\n for all `n < 63`, as well as for larger `n` with "
            "small `k`.\n Note furthermore that we use a lookup table of factorials for the bulk "
            "of the computation here.\n Hence, for larger numbers, this function is also faster "
            "than explicity computing the values.\n\n See also log_binomial_coefficient() for the "
            "log-space variant of this function.\n\nC++: "
            "genesis::utils::math::binomial_coefficient(unsigned long, unsigned long) --> double",
            pybind11::arg( "n" ),
            pybind11::arg( "k" )
        );

    // genesis::utils::math::log_binomial_distribution(unsigned long, unsigned long, double)
    // file:genesis/utils/math/binomial.hpp line:110 function-signature: double
    // genesis::utils::math::log_binomial_distribution(unsigned long, unsigned long,
    // double)(unsigned long, unsigned long, double) file:genesis/utils/math/binomial.hpp line:110
    M( "genesis::utils::math" )
        .def(
            "log_binomial_distribution",
            ( double ( * )( unsigned long, unsigned long, double ) ) &
                genesis::utils::math::log_binomial_distribution,
            "Compute the logarithm (base e) of the probability mass function for a binomial "
            "distribution.\n\n See binomial_distribution() for details.\n\nC++: "
            "genesis::utils::math::log_binomial_distribution(unsigned long, unsigned long, double) "
            "--> double",
            pybind11::arg( "k" ),
            pybind11::arg( "n" ),
            pybind11::arg( "p" )
        );

    // genesis::utils::math::binomial_distribution(unsigned long, unsigned long, double)
    // file:genesis/utils/math/binomial.hpp line:122 function-signature: double
    // genesis::utils::math::binomial_distribution(unsigned long, unsigned long, double)(unsigned
    // long, unsigned long, double) file:genesis/utils/math/binomial.hpp line:122
    M( "genesis::utils::math" )
        .def(
            "binomial_distribution",
            ( double ( * )( unsigned long, unsigned long, double ) ) &
                genesis::utils::math::binomial_distribution,
            "Compute the probability mass function for a binomial distribution.\n\n Note that we "
            "reverse the order of  and  here compared to binomial_coefficient() here,\n in order "
            "to comply with common notation.\n\n We are working in log-space for the main part of "
            "the computation, and only translate back\n at the very end. For large `n`, we are "
            "attempting to compute the distribution, but it might\n underflow. See "
            "binomial_coefficient() for details.\n\nC++: "
            "genesis::utils::math::binomial_distribution(unsigned long, unsigned long, double) --> "
            "double",
            pybind11::arg( "k" ),
            pybind11::arg( "n" ),
            pybind11::arg( "p" )
        );

    // genesis::utils::math::circumference(double) file:genesis/utils/math/common.hpp line:58
    // function-signature: double genesis::utils::math::circumference(double)(double)
    // file:genesis/utils/math/common.hpp line:58
    M( "genesis::utils::math" )
        .def(
            "circumference",
            ( double ( * )( double ) ) & genesis::utils::math::circumference,
            "C++: genesis::utils::math::circumference(double) --> double",
            pybind11::arg( "radius" )
        );

    // genesis::utils::math::almost_equal_relative(double, double, double)
    // file:genesis/utils/math/common.hpp line:158 function-signature: bool
    // genesis::utils::math::almost_equal_relative(double, double, double)(double, double, double)
    // file:genesis/utils/math/common.hpp line:158
    M( "genesis::utils::math" )
        .def(
            "almost_equal_relative",
            []( double const& a0, double const& a1 ) -> bool {
                return genesis::utils::math::almost_equal_relative( a0, a1 );
            },
            "",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" )
        );
    M( "genesis::utils::math" )
        .def(
            "almost_equal_relative",
            ( bool ( * )( double, double, double ) ) & genesis::utils::math::almost_equal_relative,
            "Check whether two doubles are almost equal, using a relative epsilon to compare "
            "them.\n\nC++: genesis::utils::math::almost_equal_relative(double, double, double) --> "
            "bool",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" ),
            pybind11::arg( "max_rel_diff" )
        );

    // genesis::utils::math::round_to(double, unsigned long) file:genesis/utils/math/common.hpp
    // line:176 function-signature: double genesis::utils::math::round_to(double, unsigned
    // long)(double, unsigned long) file:genesis/utils/math/common.hpp line:176
    M( "genesis::utils::math" )
        .def(
            "round_to",
            ( double ( * )( double, unsigned long ) ) & genesis::utils::math::round_to,
            "Retun the value of `x`, rounded to the decimal digit given by "
            "`accuracy_order`.\n\nC++: genesis::utils::math::round_to(double, unsigned long) --> "
            "double",
            pybind11::arg( "x" ),
            pybind11::arg( "accuracy_order" )
        );

    // genesis::utils::math::int_pow(unsigned long, unsigned char)
    // file:genesis/utils/math/common.hpp line:192 function-signature: unsigned long
    // genesis::utils::math::int_pow(unsigned long, unsigned char)(unsigned long, unsigned char)
    // file:genesis/utils/math/common.hpp line:192
    M( "genesis::utils::math" )
        .def(
            "int_pow",
            ( unsigned long ( * )( unsigned long, unsigned char ) ) & genesis::utils::math::int_pow,
            "Calculate the power `base^exp` for positive integer values.\n\n Remark: This "
            "overflows quite easily. The function does not check whether the desired power\n "
            "actually fits within `size_t`. Use is_valid_int_pow() to check for this first.\n\n "
            "Rationale for this function: One could argue that int powers are not really useful, "
            "particularly\n because of the fast overflow. However, using doubles leads to rounding "
            "errors, at least for\n bigger numbers. So, within the valid range, this function is "
            "more accurate. Also, it is faster.\n\nC++: genesis::utils::math::int_pow(unsigned "
            "long, unsigned char) --> unsigned long",
            pybind11::arg( "base" ),
            pybind11::arg( "exp" )
        );

    // genesis::utils::math::is_valid_int_pow(unsigned long, unsigned long)
    // file:genesis/utils/math/common.hpp line:212 function-signature: bool
    // genesis::utils::math::is_valid_int_pow(unsigned long, unsigned long)(unsigned long, unsigned
    // long) file:genesis/utils/math/common.hpp line:212
    M( "genesis::utils::math" )
        .def(
            "is_valid_int_pow",
            ( bool ( * )( unsigned long, unsigned long ) ) & genesis::utils::math::is_valid_int_pow,
            "Return whether the given power can be stored within a `size_t`.\n\n Use int_pow() to "
            "calculate the actual value of the power.\n\nC++: "
            "genesis::utils::math::is_valid_int_pow(unsigned long, unsigned long) --> bool",
            pybind11::arg( "base" ),
            pybind11::arg( "exp" )
        );

    // genesis::utils::math::squared(double) file:genesis/utils/math/common.hpp line:224
    // function-signature: double genesis::utils::math::squared(double)(double)
    // file:genesis/utils/math/common.hpp line:224
    M( "genesis::utils::math" )
        .def(
            "squared",
            ( double ( * )( double ) ) & genesis::utils::math::squared,
            "Square of a number.\n\n Simply \n, but faster than `std::pow` for the low exponent. "
            "Meant as an abbreviation\n where the argument is not already a variable, e.g., "
            "`squared( x - 1.0 )`,\n without the need to store the intermediate argument "
            "term.\n\nC++: genesis::utils::math::squared(double) --> double",
            pybind11::arg( "x" )
        );

    // genesis::utils::math::cubed(double) file:genesis/utils/math/common.hpp line:236
    // function-signature: double genesis::utils::math::cubed(double)(double)
    // file:genesis/utils/math/common.hpp line:236
    M( "genesis::utils::math" )
        .def(
            "cubed",
            ( double ( * )( double ) ) & genesis::utils::math::cubed,
            "Cube of a number.\n\n Simply \n, but faster than `std::pow` for the low exponent. "
            "Meant as an abbreviation\n where the argument is not already a variable, e.g., "
            "`cubed( x - 1.0 )`,\n without the need to store the intermediate argument "
            "term.\n\nC++: genesis::utils::math::cubed(double) --> double",
            pybind11::arg( "x" )
        );

    { // genesis::utils::math::KahanSummation file:genesis/utils/math/compensated_sum.hpp line:55
        pybind11::class_<
            genesis::utils::math::KahanSummation,
            std::shared_ptr<genesis::utils::math::KahanSummation>>
            cl( M( "genesis::utils::math" ),
                "KahanSummation",
                "Tag for tag dispatching the algorithm in CompensatedSum. See there for details." );
        // function-signature: genesis::utils::math::KahanSummation::KahanSummation(const struct
        // genesis::utils::math::KahanSummation &)(const struct genesis::utils::math::KahanSummation
        // &) file:genesis/utils/math/compensated_sum.hpp line:55
        cl.def( pybind11::init( []( genesis::utils::math::KahanSummation const& o ) {
            return new genesis::utils::math::KahanSummation( o );
        } ) );
        // function-signature: genesis::utils::math::KahanSummation::KahanSummation()()
        // file:genesis/utils/math/compensated_sum.hpp line:55
        cl.def( pybind11::init( []() { return new genesis::utils::math::KahanSummation(); } ) );
        // function-signature: struct genesis::utils::math::KahanSummation &
        // genesis::utils::math::KahanSummation::operator=(const struct
        // genesis::utils::math::KahanSummation &)(const struct genesis::utils::math::KahanSummation
        // &) file:genesis/utils/math/compensated_sum.hpp line:55
        cl.def(
            "assign",
            ( struct genesis::utils::math::KahanSummation &
              (genesis::utils::math::KahanSummation::*)(const struct genesis::utils::math::
                                                            KahanSummation&)) &
                genesis::utils::math::KahanSummation::operator=,
            "C++: genesis::utils::math::KahanSummation::operator=(const struct "
            "genesis::utils::math::KahanSummation &) --> struct "
            "genesis::utils::math::KahanSummation &",
            pybind11::return_value_policy::automatic,
            pybind11::arg( "" )
        );
    }
    { // genesis::utils::math::NeumaierSummation file:genesis/utils/math/compensated_sum.hpp line:60
        pybind11::class_<
            genesis::utils::math::NeumaierSummation,
            std::shared_ptr<genesis::utils::math::NeumaierSummation>>
            cl( M( "genesis::utils::math" ),
                "NeumaierSummation",
                "Tag for tag dispatching the algorithm in CompensatedSum. See there for details." );
        // function-signature: genesis::utils::math::NeumaierSummation::NeumaierSummation(const
        // struct genesis::utils::math::NeumaierSummation &)(const struct
        // genesis::utils::math::NeumaierSummation &) file:genesis/utils/math/compensated_sum.hpp
        // line:60
        cl.def( pybind11::init( []( genesis::utils::math::NeumaierSummation const& o ) {
            return new genesis::utils::math::NeumaierSummation( o );
        } ) );
        // function-signature: genesis::utils::math::NeumaierSummation::NeumaierSummation()()
        // file:genesis/utils/math/compensated_sum.hpp line:60
        cl.def( pybind11::init( []() { return new genesis::utils::math::NeumaierSummation(); } ) );
        // function-signature: struct genesis::utils::math::NeumaierSummation &
        // genesis::utils::math::NeumaierSummation::operator=(const struct
        // genesis::utils::math::NeumaierSummation &)(const struct
        // genesis::utils::math::NeumaierSummation &) file:genesis/utils/math/compensated_sum.hpp
        // line:60
        cl.def(
            "assign",
            ( struct genesis::utils::math::NeumaierSummation &
              (genesis::utils::math::NeumaierSummation::*)(const struct genesis::utils::math::
                                                               NeumaierSummation&)) &
                genesis::utils::math::NeumaierSummation::operator=,
            "C++: genesis::utils::math::NeumaierSummation::operator=(const struct "
            "genesis::utils::math::NeumaierSummation &) --> struct "
            "genesis::utils::math::NeumaierSummation &",
            pybind11::return_value_policy::automatic,
            pybind11::arg( "" )
        );
    }
    { // genesis::utils::math::KleinSummation file:genesis/utils/math/compensated_sum.hpp line:65
        pybind11::class_<
            genesis::utils::math::KleinSummation,
            std::shared_ptr<genesis::utils::math::KleinSummation>>
            cl( M( "genesis::utils::math" ),
                "KleinSummation",
                "Tag for tag dispatching the algorithm in CompensatedSum. See there for details." );
        // function-signature: genesis::utils::math::KleinSummation::KleinSummation()()
        // file:genesis/utils/math/compensated_sum.hpp line:65
        cl.def( pybind11::init( []() { return new genesis::utils::math::KleinSummation(); } ) );
        // function-signature: genesis::utils::math::KleinSummation::KleinSummation(const struct
        // genesis::utils::math::KleinSummation &)(const struct genesis::utils::math::KleinSummation
        // &) file:genesis/utils/math/compensated_sum.hpp line:65
        cl.def( pybind11::init( []( genesis::utils::math::KleinSummation const& o ) {
            return new genesis::utils::math::KleinSummation( o );
        } ) );
        // function-signature: struct genesis::utils::math::KleinSummation &
        // genesis::utils::math::KleinSummation::operator=(const struct
        // genesis::utils::math::KleinSummation &)(const struct genesis::utils::math::KleinSummation
        // &) file:genesis/utils/math/compensated_sum.hpp line:65
        cl.def(
            "assign",
            ( struct genesis::utils::math::KleinSummation &
              (genesis::utils::math::KleinSummation::*)(const struct genesis::utils::math::
                                                            KleinSummation&)) &
                genesis::utils::math::KleinSummation::operator=,
            "C++: genesis::utils::math::KleinSummation::operator=(const struct "
            "genesis::utils::math::KleinSummation &) --> struct "
            "genesis::utils::math::KleinSummation &",
            pybind11::return_value_policy::automatic,
            pybind11::arg( "" )
        );
    }
}
