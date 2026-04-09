#include <genesis/util/math/moments.hpp>
#include <genesis/util/math/random.hpp>
#include <genesis/util/math/statistic.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <vector>

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

void bind_genesis_util_math_moments(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::math::Moments file:genesis/util/math/moments.hpp line:61
        pybind11::
            class_<genesis::util::math::Moments, std::shared_ptr<genesis::util::math::Moments>>
                cl( M( "genesis::util::math" ),
                    "Moments",
                    "Compute running mean and variance for an input.\n\n The class allows to keep "
                    "a running mean and variance for some input, without\n having to know the "
                    "number of elements beforehand, and in a single pass.\n\n For convenience, it "
                    "also keeps track of the total count of elements added, their sum,\n and their "
                    "minimum and maximum.\n\n This class is modelled after Knuth's algorithm in "
                    "TAOCP vol 2, 3rd edition, page 232.\n See also "
                    "https://www.johndcook.com/blog/standard_deviation/\n and "
                    "https://stackoverflow.com/q/2341340" );
        // function-signature: genesis::util::math::Moments::Moments(unsigned long, bool)(unsigned
        // long, bool) file:genesis/util/math/moments.hpp line:83
        cl.def( pybind11::init( []() { return new genesis::util::math::Moments(); } ), "doc" );
        // function-signature: genesis::util::math::Moments::Moments(unsigned long, bool)(unsigned
        // long, bool) file:genesis/util/math/moments.hpp line:83
        cl.def(
            pybind11::init( []( unsigned long const& a0 ) {
                return new genesis::util::math::Moments( a0 );
            } ),
            "doc",
            pybind11::arg( "ddof" )
        );
        // function-signature: genesis::util::math::Moments::Moments(unsigned long, bool)(unsigned
        // long, bool) file:genesis/util/math/moments.hpp line:83
        cl.def(
            pybind11::init<unsigned long, bool>(),
            pybind11::arg( "ddof" ),
            pybind11::arg( "ignore_nonfinite" )
        );

        // function-signature: genesis::util::math::Moments::Moments(const class std::vector<double>
        // &, unsigned long, bool)(const class std::vector<double> &, unsigned long, bool)
        // file:genesis/util/math/moments.hpp line:117
        cl.def(
            pybind11::init( []( const class std::vector<double>& a0 ) {
                return new genesis::util::math::Moments( a0 );
            } ),
            "doc",
            pybind11::arg( "values" )
        );
        // function-signature: genesis::util::math::Moments::Moments(const class std::vector<double>
        // &, unsigned long, bool)(const class std::vector<double> &, unsigned long, bool)
        // file:genesis/util/math/moments.hpp line:117
        cl.def(
            pybind11::init( []( const class std::vector<double>& a0, unsigned long const& a1 ) {
                return new genesis::util::math::Moments( a0, a1 );
            } ),
            "doc",
            pybind11::arg( "values" ),
            pybind11::arg( "ddof" )
        );
        // function-signature: genesis::util::math::Moments::Moments(const class std::vector<double>
        // &, unsigned long, bool)(const class std::vector<double> &, unsigned long, bool)
        // file:genesis/util/math/moments.hpp line:117
        cl.def(
            pybind11::init<const class std::vector<double>&, unsigned long, bool>(),
            pybind11::arg( "values" ),
            pybind11::arg( "ddof" ),
            pybind11::arg( "ignore_nonfinite" )
        );

        // function-signature: genesis::util::math::Moments::Moments(const class
        // genesis::util::math::Moments &)(const class genesis::util::math::Moments &)
        // file:genesis/util/math/moments.hpp line:126
        cl.def( pybind11::init( []( genesis::util::math::Moments const& o ) {
            return new genesis::util::math::Moments( o );
        } ) );
        // function-signature: class genesis::util::math::Moments &
        // genesis::util::math::Moments::operator=(const class genesis::util::math::Moments &)(const
        // class genesis::util::math::Moments &) file:genesis/util/math/moments.hpp line:129
        cl.def(
            "assign",
            ( class genesis::util::math::Moments &
              (genesis::util::math::Moments::*)(const class genesis::util::math::Moments&)) &
                genesis::util::math::Moments::operator=,
            "C++: genesis::util::math::Moments::operator=(const class genesis::util::math::Moments "
            "&) --> class genesis::util::math::Moments &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: void genesis::util::math::Moments::push(double)(double)
        // file:genesis/util/math/moments.hpp line:136
        cl.def(
            "push",
            ( void( genesis::util::math::Moments::* )( double ) ) &
                genesis::util::math::Moments::push,
            "C++: genesis::util::math::Moments::push(double) --> void",
            pybind11::arg( "val" )
        );
        // function-signature: void genesis::util::math::Moments::push(const class
        // std::vector<double> &)(const class std::vector<double> &)
        // file:genesis/util/math/moments.hpp line:171
        cl.def(
            "push",
            ( void( genesis::util::math::Moments::* )( const class std::vector<double>& ) ) &
                genesis::util::math::Moments::push,
            "C++: genesis::util::math::Moments::push(const class std::vector<double> &) --> void",
            pybind11::arg( "values" )
        );
        // function-signature: unsigned long genesis::util::math::Moments::count() const()
        // file:genesis/util/math/moments.hpp line:187
        cl.def(
            "count",
            ( unsigned long ( genesis::util::math::Moments::* )() const ) &
                genesis::util::math::Moments::count,
            "C++: genesis::util::math::Moments::count() const --> unsigned long"
        );
        // function-signature: double genesis::util::math::Moments::mean() const()
        // file:genesis/util/math/moments.hpp line:192
        cl.def(
            "mean",
            ( double( genesis::util::math::Moments::* )() const ) &
                genesis::util::math::Moments::mean,
            "C++: genesis::util::math::Moments::mean() const --> double"
        );
        // function-signature: double genesis::util::math::Moments::variance() const()
        // file:genesis/util/math/moments.hpp line:197
        cl.def(
            "variance",
            ( double( genesis::util::math::Moments::* )() const ) &
                genesis::util::math::Moments::variance,
            "C++: genesis::util::math::Moments::variance() const --> double"
        );
        // function-signature: double genesis::util::math::Moments::stddev() const()
        // file:genesis/util/math/moments.hpp line:202
        cl.def(
            "stddev",
            ( double( genesis::util::math::Moments::* )() const ) &
                genesis::util::math::Moments::stddev,
            "C++: genesis::util::math::Moments::stddev() const --> double"
        );
        // function-signature: double genesis::util::math::Moments::standard_deviation() const()
        // file:genesis/util/math/moments.hpp line:207
        cl.def(
            "standard_deviation",
            ( double( genesis::util::math::Moments::* )() const ) &
                genesis::util::math::Moments::standard_deviation,
            "C++: genesis::util::math::Moments::standard_deviation() const --> double"
        );
        // function-signature: double genesis::util::math::Moments::sum() const()
        // file:genesis/util/math/moments.hpp line:212
        cl.def(
            "sum",
            ( double( genesis::util::math::Moments::* )() const ) &
                genesis::util::math::Moments::sum,
            "C++: genesis::util::math::Moments::sum() const --> double"
        );
        // function-signature: double genesis::util::math::Moments::min() const()
        // file:genesis/util/math/moments.hpp line:217
        cl.def(
            "min",
            ( double( genesis::util::math::Moments::* )() const ) &
                genesis::util::math::Moments::min,
            "C++: genesis::util::math::Moments::min() const --> double"
        );
        // function-signature: double genesis::util::math::Moments::max() const()
        // file:genesis/util/math/moments.hpp line:222
        cl.def(
            "max",
            ( double( genesis::util::math::Moments::* )() const ) &
                genesis::util::math::Moments::max,
            "C++: genesis::util::math::Moments::max() const --> double"
        );
    }
    // genesis::util::math::permuted_congruential_generator() file:genesis/util/math/random.hpp
    // line:52 function-signature: unsigned int
    // genesis::util::math::permuted_congruential_generator()() file:genesis/util/math/random.hpp
    // line:52
    M( "genesis::util::math" )
        .def(
            "permuted_congruential_generator",
            ( unsigned int ( * )() ) & genesis::util::math::permuted_congruential_generator,
            "Fast random number generator for 32bit integers.\n\n See "
            "https://en.wikipedia.org/wiki/Permuted_congruential_generator for details.\n See "
            "permuted_congruential_generator_init() to set the seed.\n\nC++: "
            "genesis::util::math::permuted_congruential_generator() --> unsigned int"
        );

    // genesis::util::math::permuted_congruential_generator(unsigned int)
    // file:genesis/util/math/random.hpp line:59 function-signature: unsigned int
    // genesis::util::math::permuted_congruential_generator(unsigned int)(unsigned int)
    // file:genesis/util/math/random.hpp line:59
    M( "genesis::util::math" )
        .def(
            "permuted_congruential_generator",
            ( unsigned int ( * )( unsigned int ) ) &
                genesis::util::math::permuted_congruential_generator,
            "Fast random number generator for 32bit integers, for numbers in the range `[0, "
            "max]`.\n\n Note that the interval is closed, i.e.,  is inclusive.\n\nC++: "
            "genesis::util::math::permuted_congruential_generator(unsigned int) --> unsigned int",
            pybind11::arg( "max" )
        );

    // genesis::util::math::permuted_congruential_generator(unsigned int, unsigned int)
    // file:genesis/util/math/random.hpp line:66 function-signature: unsigned int
    // genesis::util::math::permuted_congruential_generator(unsigned int, unsigned int)(unsigned
    // int, unsigned int) file:genesis/util/math/random.hpp line:66
    M( "genesis::util::math" )
        .def(
            "permuted_congruential_generator",
            ( unsigned int ( * )( unsigned int, unsigned int ) ) &
                genesis::util::math::permuted_congruential_generator,
            "Fast random number generator for 32bit integers, for numbers in the range `[min, "
            "max]`.\n\n Note that the interval is closed, i.e.,  is inclusive.\n\nC++: "
            "genesis::util::math::permuted_congruential_generator(unsigned int, unsigned int) --> "
            "unsigned int",
            pybind11::arg( "min" ),
            pybind11::arg( "max" )
        );

    // genesis::util::math::permuted_congruential_generator_bool() file:genesis/util/math/random.hpp
    // line:71 function-signature: bool
    // genesis::util::math::permuted_congruential_generator_bool()()
    // file:genesis/util/math/random.hpp line:71
    M( "genesis::util::math" )
        .def(
            "permuted_congruential_generator_bool",
            ( bool ( * )() ) & genesis::util::math::permuted_congruential_generator_bool,
            "Fast random number generator for 32bit integers, for bool with 0.5 "
            "probability.\n\nC++: genesis::util::math::permuted_congruential_generator_bool() --> "
            "bool"
        );

    // genesis::util::math::permuted_congruential_generator_bool(unsigned int)
    // file:genesis/util/math/random.hpp line:80 function-signature: bool
    // genesis::util::math::permuted_congruential_generator_bool(unsigned int)(unsigned int)
    // file:genesis/util/math/random.hpp line:80
    M( "genesis::util::math" )
        .def(
            "permuted_congruential_generator_bool",
            ( bool ( * )( unsigned int ) ) &
                genesis::util::math::permuted_congruential_generator_bool,
            "Fast random number generator for 32bit integers, for bool with a given chance.\n\n "
            "This is returning a random bool, with a 1 in  possibility to be `true`.\n For "
            "instance, calling the function with `chance_one_in == 5` means a 1 in 5 chance,\n or "
            "probability of 0.2 for a `true` result.\n\nC++: "
            "genesis::util::math::permuted_congruential_generator_bool(unsigned int) --> bool",
            pybind11::arg( "chance_one_in" )
        );

    // genesis::util::math::permuted_congruential_generator_init(unsigned long)
    // file:genesis/util/math/random.hpp line:85 function-signature: void
    // genesis::util::math::permuted_congruential_generator_init(unsigned long)(unsigned long)
    // file:genesis/util/math/random.hpp line:85
    M( "genesis::util::math" )
        .def(
            "permuted_congruential_generator_init",
            ( void ( * )( unsigned long ) ) &
                genesis::util::math::permuted_congruential_generator_init,
            "Set the seed for permuted_congruential_generator().\n\nC++: "
            "genesis::util::math::permuted_congruential_generator_init(unsigned long) --> void",
            pybind11::arg( "seed" )
        );

    // genesis::util::math::select_without_replacement(unsigned long, unsigned long)
    // file:genesis/util/math/random.hpp line:102 function-signature: class std::vector<unsigned
    // long> genesis::util::math::select_without_replacement(unsigned long, unsigned long)(unsigned
    // long, unsigned long) file:genesis/util/math/random.hpp line:102
    M( "genesis::util::math" )
        .def(
            "select_without_replacement",
            ( class std::vector<unsigned long>( * )( unsigned long, unsigned long ) ) &
                genesis::util::math::select_without_replacement,
            "Select  many unique numbers out of the range `[ 0, n )`.\n\n In other words, select  "
            "numbers out of the sequence of  numbers, without replacement.\n\n The implementation "
            "is based on Algorithm 3.4.2S of Knuth's book Seminumeric Algorithms.\n\n \n Desired "
            "sample size.\n \n\n Total population size.\n \n\n  List of  unique numbers out of the "
            "range `[ 0, n )`.\n\nC++: genesis::util::math::select_without_replacement(unsigned "
            "long, unsigned long) --> class std::vector<unsigned long>",
            pybind11::arg( "k" ),
            pybind11::arg( "n" )
        );

    { // genesis::util::math::MinMaxPair file:genesis/util/math/statistic.hpp line:109
        pybind11::class_<
            genesis::util::math::MinMaxPair<double>,
            std::shared_ptr<genesis::util::math::MinMaxPair<double>>>
            cl( M( "genesis::util::math" ), "MinMaxPair_double_t", "" );
        // function-signature: genesis::util::math::MinMaxPair<double>::MinMaxPair()()
        // file:genesis/util/math/statistic.hpp line:109
        cl.def( pybind11::init( []() { return new genesis::util::math::MinMaxPair<double>(); } ) );
        // function-signature: genesis::util::math::MinMaxPair<double>::MinMaxPair(const struct
        // genesis::util::math::MinMaxPair<double> &)(const struct
        // genesis::util::math::MinMaxPair<double> &) file:genesis/util/math/statistic.hpp line:109
        cl.def( pybind11::init( []( genesis::util::math::MinMaxPair<double> const& o ) {
            return new genesis::util::math::MinMaxPair<double>( o );
        } ) );
        cl.def_readwrite( "min", &genesis::util::math::MinMaxPair<double>::min );
        cl.def_readwrite( "max", &genesis::util::math::MinMaxPair<double>::max );
    }
    { // genesis::util::math::MeanStddevPair file:genesis/util/math/statistic.hpp line:121
        pybind11::class_<
            genesis::util::math::MeanStddevPair,
            std::shared_ptr<genesis::util::math::MeanStddevPair>>
            cl( M( "genesis::util::math" ),
                "MeanStddevPair",
                "Store a mean and a standard deviation value.\n\n This notation is simply more "
                "readable than the std default of using a `pair<T, T>` for such\n types." );
        // function-signature: genesis::util::math::MeanStddevPair::MeanStddevPair()()
        // file:genesis/util/math/statistic.hpp line:121
        cl.def( pybind11::init( []() { return new genesis::util::math::MeanStddevPair(); } ) );
        // function-signature: genesis::util::math::MeanStddevPair::MeanStddevPair(const struct
        // genesis::util::math::MeanStddevPair &)(const struct genesis::util::math::MeanStddevPair
        // &) file:genesis/util/math/statistic.hpp line:121
        cl.def( pybind11::init( []( genesis::util::math::MeanStddevPair const& o ) {
            return new genesis::util::math::MeanStddevPair( o );
        } ) );
        cl.def_readwrite( "mean", &genesis::util::math::MeanStddevPair::mean );
        cl.def_readwrite( "stddev", &genesis::util::math::MeanStddevPair::stddev );
    }
    // genesis::util::math::mean_stddev(const class std::vector<double> &, double)
    // file:genesis/util/math/statistic.hpp line:477 function-signature: struct
    // genesis::util::math::MeanStddevPair genesis::util::math::mean_stddev(const class
    // std::vector<double> &, double)(const class std::vector<double> &, double)
    // file:genesis/util/math/statistic.hpp line:477
    M( "genesis::util::math" )
        .def(
            "mean_stddev",
            []( const class std::vector<double>& a0 ) -> genesis::util::math::MeanStddevPair {
                return genesis::util::math::mean_stddev( a0 );
            },
            "",
            pybind11::arg( "vec" )
        );
    M( "genesis::util::math" )
        .def(
            "mean_stddev",
            ( struct genesis::util::math::MeanStddevPair( * )(
                const class std::vector<double>&, double
            ) ) &
                genesis::util::math::mean_stddev,
            "Calculate the mean and standard deviation of a `std::vector` of `double` "
            "elements.\n\n \n mean_stddev( ForwardIterator first, ForwardIterator last, double "
            "epsilon ) for details.\n \n\n arithmetic_mean() for a function that only calculates "
            "the mean, and thus saves the effort\n of a second iteration over the range.\n\nC++: "
            "genesis::util::math::mean_stddev(const class std::vector<double> &, double) --> "
            "struct genesis::util::math::MeanStddevPair",
            pybind11::arg( "vec" ),
            pybind11::arg( "epsilon" )
        );

    // genesis::util::math::arithmetic_mean(const class std::vector<double> &)
    // file:genesis/util/math/statistic.hpp line:537 function-signature: double
    // genesis::util::math::arithmetic_mean(const class std::vector<double> &)(const class
    // std::vector<double> &) file:genesis/util/math/statistic.hpp line:537
    M( "genesis::util::math" )
        .def(
            "arithmetic_mean",
            ( double ( * )( const class std::vector<double>& ) ) &
                genesis::util::math::arithmetic_mean,
            "Calculate the arithmetic mean of a `std::vector` of `double` elements.\n\n \n "
            "arithmetic_mean( ForwardIterator first, ForwardIterator last ) for details.\n \n\n "
            "mean_stddev() for a function that simultaneously calculates the standard deviation.\n "
            "\n\n geometric_mean() for a function that calculates the geometric mean, and\n \n\n "
            "harmonic_mean() for a function that calculates the harmonic mean.\n\nC++: "
            "genesis::util::math::arithmetic_mean(const class std::vector<double> &) --> double",
            pybind11::arg( "vec" )
        );

    // genesis::util::math::weighted_arithmetic_mean(const class std::vector<double> &, const class
    // std::vector<double> &) file:genesis/util/math/statistic.hpp line:611 function-signature:
    // double genesis::util::math::weighted_arithmetic_mean(const class std::vector<double> &, const
    // class std::vector<double> &)(const class std::vector<double> &, const class
    // std::vector<double> &) file:genesis/util/math/statistic.hpp line:611
    M( "genesis::util::math" )
        .def(
            "weighted_arithmetic_mean",
            ( double ( * )( const class std::vector<double>&, const class std::vector<double>& ) ) &
                genesis::util::math::weighted_arithmetic_mean,
            "Calculate the weighted arithmetic mean of a `std::vector` of `double` elements.\n\n "
            "\n weighted_arithmetic_mean( ForwardIterator first, ForwardIterator last ) for "
            "details.\n \n\n arithmetic_mean() for the unweighted version.\n \n\n geometric_mean() "
            "for a function that calculates the geometric mean, and\n \n\n harmonic_mean() for a "
            "function that calculates the harmonic mean.\n \n\n weighted_geometric_mean() for a "
            "function that calculates the weighted geometric mean, and\n \n\n "
            "weighted_harmonic_mean() for a function that calculates the weighted harmonic "
            "mean.\n\nC++: genesis::util::math::weighted_arithmetic_mean(const class "
            "std::vector<double> &, const class std::vector<double> &) --> double",
            pybind11::arg( "values" ),
            pybind11::arg( "weights" )
        );
}
