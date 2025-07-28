#include <genesis/utils/math/statistics.hpp>
#include <iterator>
#include <memory>
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

void bind_genesis_utils_math_statistics(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::utils::math::weighted_arithmetic_mean(const class std::vector<double> &, const class
    // std::vector<double> &) file:genesis/utils/math/statistics.hpp line:611 function-signature:
    // double genesis::utils::math::weighted_arithmetic_mean(const class std::vector<double> &,
    // const class std::vector<double> &)(const class std::vector<double> &, const class
    // std::vector<double> &) file:genesis/utils/math/statistics.hpp line:611
    M( "genesis::utils::math" )
        .def(
            "weighted_arithmetic_mean",
            ( double ( * )( const class std::vector<double>&, const class std::vector<double>& ) ) &
                genesis::utils::math::weighted_arithmetic_mean,
            "Calculate the weighted arithmetic mean of a `std::vector` of `double` elements.\n\n "
            "\n weighted_arithmetic_mean( ForwardIterator first, ForwardIterator last ) for "
            "details.\n \n\n arithmetic_mean() for the unweighted version.\n \n\n geometric_mean() "
            "for a function that calculates the geometric mean, and\n \n\n harmonic_mean() for a "
            "function that calculates the harmonic mean.\n \n\n weighted_geometric_mean() for a "
            "function that calculates the weighted geometric mean, and\n \n\n "
            "weighted_harmonic_mean() for a function that calculates the weighted harmonic "
            "mean.\n\nC++: genesis::utils::math::weighted_arithmetic_mean(const class "
            "std::vector<double> &, const class std::vector<double> &) --> double",
            pybind11::arg( "values" ),
            pybind11::arg( "weights" )
        );

    // genesis::utils::math::geometric_mean(const class std::vector<double> &)
    // file:genesis/utils/math/statistics.hpp line:677 function-signature: double
    // genesis::utils::math::geometric_mean(const class std::vector<double> &)(const class
    // std::vector<double> &) file:genesis/utils/math/statistics.hpp line:677
    M( "genesis::utils::math" )
        .def(
            "geometric_mean",
            ( double ( * )( const class std::vector<double>& ) ) &
                genesis::utils::math::geometric_mean,
            "Calculate the geometric mean of a `std::vector` of `double` elements.\n\n \n "
            "geometric_mean( ForwardIterator first, ForwardIterator last ) for details.\n \n\n "
            "arithmetic_mean() for a function that calculates the arithmetic mean, and\n \n\n "
            "harmonic_mean() for a function that calculates the harmonic mean.\n\nC++: "
            "genesis::utils::math::geometric_mean(const class std::vector<double> &) --> double",
            pybind11::arg( "vec" )
        );

    // genesis::utils::math::weighted_geometric_mean(const class std::vector<double> &, const class
    // std::vector<double> &) file:genesis/utils/math/statistics.hpp line:770 function-signature:
    // double genesis::utils::math::weighted_geometric_mean(const class std::vector<double> &, const
    // class std::vector<double> &)(const class std::vector<double> &, const class
    // std::vector<double> &) file:genesis/utils/math/statistics.hpp line:770
    M( "genesis::utils::math" )
        .def(
            "weighted_geometric_mean",
            ( double ( * )( const class std::vector<double>&, const class std::vector<double>& ) ) &
                genesis::utils::math::weighted_geometric_mean,
            "Calculate the weighted geometric mean of a `std::vector` of `double` elements.\n\n \n "
            "weighted_geometric_mean( ForwardIterator first, ForwardIterator last ) for details.\n "
            "\n\n geometric_mean() for the unweighted version.\n \n\n arithmetic_mean() for a "
            "function that calculates the arithmetic mean, and\n \n\n harmonic_mean() for a "
            "function that calculates the harmonic mean.\n \n\n weighted_arithmetic_mean() for a "
            "function that calculates the weighted arithmetic mean, and\n \n\n "
            "weighted_harmonic_mean() for a function that calculates the weighted harmonic "
            "mean.\n\nC++: genesis::utils::math::weighted_geometric_mean(const class "
            "std::vector<double> &, const class std::vector<double> &) --> double",
            pybind11::arg( "values" ),
            pybind11::arg( "weights" )
        );

    // genesis::utils::math::HarmonicMeanZeroPolicy file:genesis/utils/math/statistics.hpp line:785
    pybind11::enum_<genesis::utils::math::HarmonicMeanZeroPolicy>(
        M( "genesis::utils::math" ),
        "HarmonicMeanZeroPolicy",
        "Select a policy on how to treat zeroes in the computation of harmonic_mean()\n and "
        "weighted_harmonic_mean()."
    )
        .value( "kThrow", genesis::utils::math::HarmonicMeanZeroPolicy::kThrow )
        .value( "kIgnore", genesis::utils::math::HarmonicMeanZeroPolicy::kIgnore )
        .value( "kReturnZero", genesis::utils::math::HarmonicMeanZeroPolicy::kReturnZero )
        .value( "kCorrection", genesis::utils::math::HarmonicMeanZeroPolicy::kCorrection );

    ;

    // genesis::utils::math::harmonic_mean(const class std::vector<double> &, enum
    // genesis::utils::math::HarmonicMeanZeroPolicy) file:genesis/utils/math/statistics.hpp line:913
    // function-signature: double genesis::utils::math::harmonic_mean(const class
    // std::vector<double> &, enum genesis::utils::math::HarmonicMeanZeroPolicy)(const class
    // std::vector<double> &, enum genesis::utils::math::HarmonicMeanZeroPolicy)
    // file:genesis/utils/math/statistics.hpp line:913
    M( "genesis::utils::math" )
        .def(
            "harmonic_mean",
            []( const class std::vector<double>& a0 ) -> double {
                return genesis::utils::math::harmonic_mean( a0 );
            },
            "",
            pybind11::arg( "vec" )
        );
    M( "genesis::utils::math" )
        .def(
            "harmonic_mean",
            ( double ( * )(
                const class std::vector<double>&, enum genesis::utils::math::HarmonicMeanZeroPolicy
            ) ) &
                genesis::utils::math::harmonic_mean,
            "Calculate the harmonic mean of a `std::vector` of `double` elements.\n\n \n "
            "harmonic_mean( ForwardIterator first, ForwardIterator last ) for details.\n \n\n "
            "arithmetic_mean() for a function that calculates the arithmetic mean, and\n \n\n "
            "geometric_mean() for a function that calculates the geometric mean.\n\nC++: "
            "genesis::utils::math::harmonic_mean(const class std::vector<double> &, enum "
            "genesis::utils::math::HarmonicMeanZeroPolicy) --> double",
            pybind11::arg( "vec" ),
            pybind11::arg( "zero_policy" )
        );

    // genesis::utils::math::weighted_harmonic_mean(const class std::vector<double> &, const class
    // std::vector<double> &, enum genesis::utils::math::HarmonicMeanZeroPolicy)
    // file:genesis/utils/math/statistics.hpp line:1052 function-signature: double
    // genesis::utils::math::weighted_harmonic_mean(const class std::vector<double> &, const class
    // std::vector<double> &, enum genesis::utils::math::HarmonicMeanZeroPolicy)(const class
    // std::vector<double> &, const class std::vector<double> &, enum
    // genesis::utils::math::HarmonicMeanZeroPolicy) file:genesis/utils/math/statistics.hpp
    // line:1052
    M( "genesis::utils::math" )
        .def(
            "weighted_harmonic_mean",
            []( const class std::vector<double>& a0, const class std::vector<double>& a1
            ) -> double { return genesis::utils::math::weighted_harmonic_mean( a0, a1 ); },
            "",
            pybind11::arg( "values" ),
            pybind11::arg( "weights" )
        );
    M( "genesis::utils::math" )
        .def(
            "weighted_harmonic_mean",
            ( double ( * )(
                const class std::vector<double>&,
                const class std::vector<double>&,
                enum genesis::utils::math::HarmonicMeanZeroPolicy
            ) ) &
                genesis::utils::math::weighted_harmonic_mean,
            "Calculate the weighted harmonic mean of a `std::vector` of `double` elements.\n\n \n "
            "weighted_harmonic_mean( ForwardIterator first, ForwardIterator last ) for details.\n "
            "\n\n harmonic_mean() for the unweighted version.\n \n\n arithmetic_mean() for a "
            "function that calculates the arithmetic mean, and\n \n\n geometric_mean() for a "
            "function that calculates the geometric mean.\n \n\n weighted_arithmetic_mean() for a "
            "function that calculates the weighted arithmetic mean, and\n \n\n "
            "weighted_geometric_mean() for a function that calculates the weighted geometric "
            "mean.\n\nC++: genesis::utils::math::weighted_harmonic_mean(const class "
            "std::vector<double> &, const class std::vector<double> &, enum "
            "genesis::utils::math::HarmonicMeanZeroPolicy) --> double",
            pybind11::arg( "values" ),
            pybind11::arg( "weights" ),
            pybind11::arg( "zero_policy" )
        );

    // genesis::utils::math::coefficient_of_variation(const struct
    // genesis::utils::math::MeanStddevPair &) file:genesis/utils/math/statistics.hpp line:1193
    // function-signature: double genesis::utils::math::coefficient_of_variation(const struct
    // genesis::utils::math::MeanStddevPair &)(const struct genesis::utils::math::MeanStddevPair &)
    // file:genesis/utils/math/statistics.hpp line:1193
    M( "genesis::utils::math" )
        .def(
            "coefficient_of_variation",
            ( double ( * )( const struct genesis::utils::math::MeanStddevPair& ) ) &
                genesis::utils::math::coefficient_of_variation,
            "Calculate the index of dispersion.\n\n The coefficient of variation (CV), also known "
            "as the relative standard deviation (RSD),\n is defined as the ratio of the standard "
            "deviation to the mean.\n See mean_stddev() to calculate those values.\n See "
            "https://en.wikipedia.org/wiki/Coefficient_of_variation for details.\n\nC++: "
            "genesis::utils::math::coefficient_of_variation(const struct "
            "genesis::utils::math::MeanStddevPair &) --> double",
            pybind11::arg( "ms" )
        );

    // genesis::utils::math::coefficient_of_variation(const class std::vector<struct
    // genesis::utils::math::MeanStddevPair> &) file:genesis/utils/math/statistics.hpp line:1201
    // function-signature: class std::vector<double>
    // genesis::utils::math::coefficient_of_variation(const class std::vector<struct
    // genesis::utils::math::MeanStddevPair> &)(const class std::vector<struct
    // genesis::utils::math::MeanStddevPair> &) file:genesis/utils/math/statistics.hpp line:1201
    M( "genesis::utils::math" )
        .def(
            "coefficient_of_variation",
            ( class std::vector<double>( * )( const class std::vector<
                                              struct genesis::utils::math::MeanStddevPair>& ) ) &
                genesis::utils::math::coefficient_of_variation,
            "MeanStddevPair const& ms )\n\nC++: "
            "genesis::utils::math::coefficient_of_variation(const class std::vector<struct "
            "genesis::utils::math::MeanStddevPair> &) --> class std::vector<double>",
            pybind11::arg( "ms" )
        );

    // genesis::utils::math::index_of_dispersion(const struct genesis::utils::math::MeanStddevPair
    // &) file:genesis/utils/math/statistics.hpp line:1219 function-signature: double
    // genesis::utils::math::index_of_dispersion(const struct genesis::utils::math::MeanStddevPair
    // &)(const struct genesis::utils::math::MeanStddevPair &)
    // file:genesis/utils/math/statistics.hpp line:1219
    M( "genesis::utils::math" )
        .def(
            "index_of_dispersion",
            ( double ( * )( const struct genesis::utils::math::MeanStddevPair& ) ) &
                genesis::utils::math::index_of_dispersion,
            "Calculate the index of dispersion.\n\n The index of dispersion, also known as the "
            "dispersion index, coefficient of dispersion,\n relative variance, variance-to-mean "
            "ratio (VMR) or Fano factor, is defined as the ratio of the\n variance to the mean. "
            "Variance is the square of the standard deviation.\n See mean_stddev() to calculate "
            "those values.\n See https://en.wikipedia.org/wiki/Index_of_dispersion for "
            "details.\n\nC++: genesis::utils::math::index_of_dispersion(const struct "
            "genesis::utils::math::MeanStddevPair &) --> double",
            pybind11::arg( "ms" )
        );

    // genesis::utils::math::index_of_dispersion(const class std::vector<struct
    // genesis::utils::math::MeanStddevPair> &) file:genesis/utils/math/statistics.hpp line:1227
    // function-signature: class std::vector<double> genesis::utils::math::index_of_dispersion(const
    // class std::vector<struct genesis::utils::math::MeanStddevPair> &)(const class
    // std::vector<struct genesis::utils::math::MeanStddevPair> &)
    // file:genesis/utils/math/statistics.hpp line:1227
    M( "genesis::utils::math" )
        .def(
            "index_of_dispersion",
            ( class std::vector<double>( * )( const class std::vector<
                                              struct genesis::utils::math::MeanStddevPair>& ) ) &
                genesis::utils::math::index_of_dispersion,
            "MeanStddevPair const& ms )\n\nC++: genesis::utils::math::index_of_dispersion(const "
            "class std::vector<struct genesis::utils::math::MeanStddevPair> &) --> class "
            "std::vector<double>",
            pybind11::arg( "ms" )
        );
}
