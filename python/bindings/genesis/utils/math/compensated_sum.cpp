#include <genesis/utils/math/compensated_sum.hpp>
#include <genesis/utils/math/correlation.hpp>
#include <genesis/utils/math/ranking.hpp>
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

void bind_genesis_utils_math_compensated_sum(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
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
    // genesis::utils::math::ranking_standard(class __gnu_cxx::__normal_iterator<const double *,
    // class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >) file:genesis/utils/math/ranking.hpp line:62 function-signature: class
    // std::vector<unsigned long>
    // genesis::utils::math::ranking_standard<__gnu_cxx::__normal_iterator<const double *,
    // std::vector<double> >>(class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >)(class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >) file:genesis/utils/math/ranking.hpp line:62
    M( "genesis::utils::math" )
        .def(
            "ranking_standard",
            ( class std::vector<
                unsigned long>( * )( class __gnu_cxx::__normal_iterator<const double*, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double*, class std::vector<double> > )
            ) &
                genesis::utils::math::ranking_standard<
                    __gnu_cxx::__normal_iterator<const double*, std::vector<double> >>,
            "C++: genesis::utils::math::ranking_standard(class __gnu_cxx::__normal_iterator<const "
            "double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const "
            "double *, class std::vector<double> >) --> class std::vector<unsigned long>",
            pybind11::arg( "first" ),
            pybind11::arg( "last" )
        );

    // genesis::utils::math::ranking_standard(const class std::vector<double> &)
    // file:genesis/utils/math/ranking.hpp line:94 function-signature: class std::vector<unsigned
    // long> genesis::utils::math::ranking_standard(const class std::vector<double> &)(const class
    // std::vector<double> &) file:genesis/utils/math/ranking.hpp line:94
    M( "genesis::utils::math" )
        .def(
            "ranking_standard",
            ( class std::vector<unsigned long>( * )( const class std::vector<double>& ) ) &
                genesis::utils::math::ranking_standard,
            "RandomAccessIterator first, RandomAccessIterator last )\n\nC++: "
            "genesis::utils::math::ranking_standard(const class std::vector<double> &) --> class "
            "std::vector<unsigned long>",
            pybind11::arg( "vec" )
        );

    // genesis::utils::math::ranking_modified(class __gnu_cxx::__normal_iterator<const double *,
    // class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >) file:genesis/utils/math/ranking.hpp line:113 function-signature: class
    // std::vector<unsigned long>
    // genesis::utils::math::ranking_modified<__gnu_cxx::__normal_iterator<const double *,
    // std::vector<double> >>(class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >)(class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >) file:genesis/utils/math/ranking.hpp line:113
    M( "genesis::utils::math" )
        .def(
            "ranking_modified",
            ( class std::vector<
                unsigned long>( * )( class __gnu_cxx::__normal_iterator<const double*, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double*, class std::vector<double> > )
            ) &
                genesis::utils::math::ranking_modified<
                    __gnu_cxx::__normal_iterator<const double*, std::vector<double> >>,
            "C++: genesis::utils::math::ranking_modified(class __gnu_cxx::__normal_iterator<const "
            "double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const "
            "double *, class std::vector<double> >) --> class std::vector<unsigned long>",
            pybind11::arg( "first" ),
            pybind11::arg( "last" )
        );

    // genesis::utils::math::ranking_modified(const class std::vector<double> &)
    // file:genesis/utils/math/ranking.hpp line:152 function-signature: class std::vector<unsigned
    // long> genesis::utils::math::ranking_modified(const class std::vector<double> &)(const class
    // std::vector<double> &) file:genesis/utils/math/ranking.hpp line:152
    M( "genesis::utils::math" )
        .def(
            "ranking_modified",
            ( class std::vector<unsigned long>( * )( const class std::vector<double>& ) ) &
                genesis::utils::math::ranking_modified,
            "RandomAccessIterator first, RandomAccessIterator last )\n\nC++: "
            "genesis::utils::math::ranking_modified(const class std::vector<double> &) --> class "
            "std::vector<unsigned long>",
            pybind11::arg( "vec" )
        );

    // genesis::utils::math::ranking_dense(class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >) file:genesis/utils/math/ranking.hpp line:170 function-signature: class
    // std::vector<unsigned long>
    // genesis::utils::math::ranking_dense<__gnu_cxx::__normal_iterator<const double *,
    // std::vector<double> >>(class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >)(class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >) file:genesis/utils/math/ranking.hpp line:170
    M( "genesis::utils::math" )
        .def(
            "ranking_dense",
            ( class std::vector<
                unsigned long>( * )( class __gnu_cxx::__normal_iterator<const double*, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double*, class std::vector<double> > )
            ) &
                genesis::utils::math::ranking_dense<
                    __gnu_cxx::__normal_iterator<const double*, std::vector<double> >>,
            "C++: genesis::utils::math::ranking_dense(class __gnu_cxx::__normal_iterator<const "
            "double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const "
            "double *, class std::vector<double> >) --> class std::vector<unsigned long>",
            pybind11::arg( "first" ),
            pybind11::arg( "last" )
        );

    // genesis::utils::math::ranking_dense(const class std::vector<double> &)
    // file:genesis/utils/math/ranking.hpp line:202 function-signature: class std::vector<unsigned
    // long> genesis::utils::math::ranking_dense(const class std::vector<double> &)(const class
    // std::vector<double> &) file:genesis/utils/math/ranking.hpp line:202
    M( "genesis::utils::math" )
        .def(
            "ranking_dense",
            ( class std::vector<unsigned long>( * )( const class std::vector<double>& ) ) &
                genesis::utils::math::ranking_dense,
            "RandomAccessIterator first, RandomAccessIterator last )\n\nC++: "
            "genesis::utils::math::ranking_dense(const class std::vector<double> &) --> class "
            "std::vector<unsigned long>",
            pybind11::arg( "vec" )
        );

    // genesis::utils::math::ranking_ordinal(class __gnu_cxx::__normal_iterator<const double *,
    // class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >) file:genesis/utils/math/ranking.hpp line:220 function-signature: class
    // std::vector<unsigned long>
    // genesis::utils::math::ranking_ordinal<__gnu_cxx::__normal_iterator<const double *,
    // std::vector<double> >>(class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >)(class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >) file:genesis/utils/math/ranking.hpp line:220
    M( "genesis::utils::math" )
        .def(
            "ranking_ordinal",
            ( class std::vector<
                unsigned long>( * )( class __gnu_cxx::__normal_iterator<const double*, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double*, class std::vector<double> > )
            ) &
                genesis::utils::math::ranking_ordinal<
                    __gnu_cxx::__normal_iterator<const double*, std::vector<double> >>,
            "C++: genesis::utils::math::ranking_ordinal(class __gnu_cxx::__normal_iterator<const "
            "double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const "
            "double *, class std::vector<double> >) --> class std::vector<unsigned long>",
            pybind11::arg( "first" ),
            pybind11::arg( "last" )
        );

    // genesis::utils::math::ranking_ordinal(const class std::vector<double> &)
    // file:genesis/utils/math/ranking.hpp line:243 function-signature: class std::vector<unsigned
    // long> genesis::utils::math::ranking_ordinal(const class std::vector<double> &)(const class
    // std::vector<double> &) file:genesis/utils/math/ranking.hpp line:243
    M( "genesis::utils::math" )
        .def(
            "ranking_ordinal",
            ( class std::vector<unsigned long>( * )( const class std::vector<double>& ) ) &
                genesis::utils::math::ranking_ordinal,
            "RandomAccessIterator first, RandomAccessIterator last )\n\nC++: "
            "genesis::utils::math::ranking_ordinal(const class std::vector<double> &) --> class "
            "std::vector<unsigned long>",
            pybind11::arg( "vec" )
        );

    // genesis::utils::math::ranking_fractional(class __gnu_cxx::__normal_iterator<const double *,
    // class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >) file:genesis/utils/math/ranking.hpp line:263 function-signature: class
    // std::vector<double>
    // genesis::utils::math::ranking_fractional<__gnu_cxx::__normal_iterator<const double *,
    // std::vector<double> >>(class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >)(class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >) file:genesis/utils/math/ranking.hpp line:263
    M( "genesis::utils::math" )
        .def(
            "ranking_fractional",
            ( class std::vector<
                double>( * )( class __gnu_cxx::__normal_iterator<const double*, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double*, class std::vector<double> > )
            ) &
                genesis::utils::math::ranking_fractional<
                    __gnu_cxx::__normal_iterator<const double*, std::vector<double> >>,
            "C++: genesis::utils::math::ranking_fractional(class "
            "__gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class "
            "__gnu_cxx::__normal_iterator<const double *, class std::vector<double> >) --> class "
            "std::vector<double>",
            pybind11::arg( "first" ),
            pybind11::arg( "last" )
        );

    // genesis::utils::math::ranking_fractional(const class std::vector<double> &)
    // file:genesis/utils/math/ranking.hpp line:322 function-signature: class std::vector<double>
    // genesis::utils::math::ranking_fractional(const class std::vector<double> &)(const class
    // std::vector<double> &) file:genesis/utils/math/ranking.hpp line:322
    M( "genesis::utils::math" )
        .def(
            "ranking_fractional",
            ( class std::vector<double>( * )( const class std::vector<double>& ) ) &
                genesis::utils::math::ranking_fractional,
            "RandomAccessIterator first, RandomAccessIterator last )\n\nC++: "
            "genesis::utils::math::ranking_fractional(const class std::vector<double> &) --> class "
            "std::vector<double>",
            pybind11::arg( "vec" )
        );

    // genesis::utils::math::pearson_correlation_coefficient(class
    // __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class
    // __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class
    // __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class
    // __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >)
    // file:genesis/utils/math/correlation.hpp line:71 function-signature: double
    // genesis::utils::math::pearson_correlation_coefficient<__gnu_cxx::__normal_iterator<const
    // double *, std::vector<double> >,__gnu_cxx::__normal_iterator<const double *,
    // std::vector<double> >>(class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >)(class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class
    // std::vector<double> >) file:genesis/utils/math/correlation.hpp line:71
    M( "genesis::utils::math" )
        .def(
            "pearson_correlation_coefficient",
            (
                double ( * )( class __gnu_cxx::__normal_iterator<const double*, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double*, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double*, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double*, class std::vector<double> > )
            ) &
                genesis::utils::math::pearson_correlation_coefficient<
                    __gnu_cxx::__normal_iterator<const double*, std::vector<double> >,
                    __gnu_cxx::__normal_iterator<const double*, std::vector<double> >>,
            "C++: genesis::utils::math::pearson_correlation_coefficient(class "
            "__gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class "
            "__gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class "
            "__gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class "
            "__gnu_cxx::__normal_iterator<const double *, class std::vector<double> >) --> double",
            pybind11::arg( "first_a" ),
            pybind11::arg( "last_a" ),
            pybind11::arg( "first_b" ),
            pybind11::arg( "last_b" )
        );
}
