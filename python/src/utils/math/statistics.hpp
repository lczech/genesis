/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

template <typename T>
void PythonExportClass_::genesis::utils::MinMaxPair(std::string name)
{

    // -------------------------------------------------------------------
    //     Class MinMaxPair
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using MinMaxPairType = MinMaxPair<typename T>;

    pybind11::class_< MinMaxPairType, std::shared_ptr<MinMaxPairType> > ( scope, name.c_str() )
    ;
}

template<class ForwardIterator>
void python_export_function_utils_math_statistics_ForwardIterator ()
{
    scope.def(
        "mean_stddev",
        ( MeanStddevPair ( * )( ForwardIterator, ForwardIterator, double ))( &::genesis::utils::mean_stddev ),
            pybind11::arg("first"),
            pybind11::arg("last"),
            pybind11::arg("epsilon")=(double)(-1.0),
        get_docstring("MeanStddevPair ::genesis::utils::mean_stddev (ForwardIterator first, ForwardIterator last, double epsilon=-1.0)")
    );
}

template<class ForwardIteratorA, class ForwardIteratorB>
void python_export_function_utils_math_statistics_ForwardIteratorA_ForwardIteratorB ()
{
    scope.def(
        "pearson_correlation_coefficient",
        ( double ( * )( ForwardIteratorA, ForwardIteratorA, ForwardIteratorB, ForwardIteratorB ))( &::genesis::utils::pearson_correlation_coefficient ),
            pybind11::arg("first_a"),
            pybind11::arg("last_a"),
            pybind11::arg("first_b"),
            pybind11::arg("last_b"),
        get_docstring("double ::genesis::utils::pearson_correlation_coefficient (ForwardIteratorA first_a, ForwardIteratorA last_a, ForwardIteratorB first_b, ForwardIteratorB last_b)")
    );

    scope.def(
        "finite_pairs",
        ( std::pair< std::vector< double >, std::vector< double > > ( * )( ForwardIteratorA, ForwardIteratorA, ForwardIteratorB, ForwardIteratorB ))( &::genesis::utils::finite_pairs ),
            pybind11::arg("first_a"),
            pybind11::arg("last_a"),
            pybind11::arg("first_b"),
            pybind11::arg("last_b"),
        get_docstring("std::pair< std::vector< double >, std::vector< double > > ::genesis::utils::finite_pairs (ForwardIteratorA first_a, ForwardIteratorA last_a, ForwardIteratorB first_b, ForwardIteratorB last_b)")
    );
}

template<class RandomAccessIteratorA, class RandomAccessIteratorB>
void python_export_function_utils_math_statistics_RandomAccessIteratorA_RandomAccessIteratorB ()
{
    scope.def(
        "spearmans_rank_correlation_coefficient",
        ( double ( * )( RandomAccessIteratorA, RandomAccessIteratorA, RandomAccessIteratorB, RandomAccessIteratorB ))( &::genesis::utils::spearmans_rank_correlation_coefficient ),
            pybind11::arg("first_a"),
            pybind11::arg("last_a"),
            pybind11::arg("first_b"),
            pybind11::arg("last_b"),
        get_docstring("double ::genesis::utils::spearmans_rank_correlation_coefficient (RandomAccessIteratorA first_a, RandomAccessIteratorA last_a, RandomAccessIteratorB first_b, RandomAccessIteratorB last_b)")
    );
}

template<class RandomAccessIterator>
void python_export_function_utils_math_statistics_RandomAccessIterator ()
{
    scope.def(
        "quartiles",
        ( Quartiles ( * )( RandomAccessIterator, RandomAccessIterator ))( &::genesis::utils::quartiles ),
            pybind11::arg("first"),
            pybind11::arg("last")
    );

    scope.def(
        "median",
        ( double ( * )( RandomAccessIterator, RandomAccessIterator ))( &::genesis::utils::median ),
            pybind11::arg("first"),
            pybind11::arg("last"),
        get_docstring("double ::genesis::utils::median (RandomAccessIterator first, RandomAccessIterator last)")
    );

    scope.def(
        "median",
        ( double ( * )( RandomAccessIterator, RandomAccessIterator, size_t, size_t ))( &::genesis::utils::median ),
            pybind11::arg("first"),
            pybind11::arg("last"),
            pybind11::arg("l"),
            pybind11::arg("r"),
        get_docstring("double ::genesis::utils::median (RandomAccessIterator first, RandomAccessIterator last, size_t l, size_t r)")
    );

    scope.def(
        "ranking_fractional",
        ( std::vector< double > ( * )( RandomAccessIterator, RandomAccessIterator ))( &::genesis::utils::ranking_fractional ),
            pybind11::arg("first"),
            pybind11::arg("last"),
        get_docstring("std::vector< double > ::genesis::utils::ranking_fractional (RandomAccessIterator first, RandomAccessIterator last)")
    );

    scope.def(
        "ranking_dense",
        ( std::vector< size_t > ( * )( RandomAccessIterator, RandomAccessIterator ))( &::genesis::utils::ranking_dense ),
            pybind11::arg("first"),
            pybind11::arg("last"),
        get_docstring("std::vector< size_t > ::genesis::utils::ranking_dense (RandomAccessIterator first, RandomAccessIterator last)")
    );

    scope.def(
        "ranking_modified",
        ( std::vector< size_t > ( * )( RandomAccessIterator, RandomAccessIterator ))( &::genesis::utils::ranking_modified ),
            pybind11::arg("first"),
            pybind11::arg("last"),
        get_docstring("std::vector< size_t > ::genesis::utils::ranking_modified (RandomAccessIterator first, RandomAccessIterator last)")
    );

    scope.def(
        "ranking_ordinal",
        ( std::vector< size_t > ( * )( RandomAccessIterator, RandomAccessIterator ))( &::genesis::utils::ranking_ordinal ),
            pybind11::arg("first"),
            pybind11::arg("last"),
        get_docstring("std::vector< size_t > ::genesis::utils::ranking_ordinal (RandomAccessIterator first, RandomAccessIterator last)")
    );

    scope.def(
        "ranking_standard",
        ( std::vector< size_t > ( * )( RandomAccessIterator, RandomAccessIterator ))( &::genesis::utils::ranking_standard ),
            pybind11::arg("first"),
            pybind11::arg("last"),
        get_docstring("std::vector< size_t > ::genesis::utils::ranking_standard (RandomAccessIterator first, RandomAccessIterator last)")
    );
}