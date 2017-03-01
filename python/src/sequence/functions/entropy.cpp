/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_FUNCTIONS( sequence_functions_entropy_export, ::genesis::sequence, scope )
{

    scope.def(
        "operator|=",
        ( SiteEntropyOptions & ( * )( SiteEntropyOptions &, SiteEntropyOptions ))( &::genesis::sequence::operator|= ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
        get_docstring("SiteEntropyOptions & ::genesis::sequence::operator|= (SiteEntropyOptions & lhs, SiteEntropyOptions rhs)")
    );

    scope.def(
        "operator|",
        ( SiteEntropyOptions ( * )( SiteEntropyOptions, SiteEntropyOptions ))( &::genesis::sequence::operator| ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
        get_docstring("SiteEntropyOptions ::genesis::sequence::operator| (SiteEntropyOptions lhs, SiteEntropyOptions rhs)")
    );

    scope.def(
        "operator&",
        ( bool ( * )( SiteEntropyOptions, SiteEntropyOptions ))( &::genesis::sequence::operator& ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs"),
        get_docstring("bool ::genesis::sequence::operator& (SiteEntropyOptions lhs, SiteEntropyOptions rhs)")
    );

    scope.def(
        "absolute_entropy",
        ( double ( * )( SequenceCounts const &, SiteEntropyOptions ))( &::genesis::sequence::absolute_entropy ),
            pybind11::arg("counts"),
            pybind11::arg("per_site_options"),
        get_docstring("double ::genesis::sequence::absolute_entropy (SequenceCounts const & counts, SiteEntropyOptions per_site_options)")
    );

    scope.def(
        "averaged_entropy",
        ( double ( * )( SequenceCounts const &, bool, SiteEntropyOptions ))( &::genesis::sequence::averaged_entropy ),
            pybind11::arg("counts"),
            pybind11::arg("only_determined_sites")=(bool)(false),
            pybind11::arg("per_site_options"),
        get_docstring("double ::genesis::sequence::averaged_entropy (SequenceCounts const & counts, bool only_determined_sites=false, SiteEntropyOptions per_site_options)")
    );

    scope.def(
        "site_entropy",
        ( double ( * )( SequenceCounts const &, size_t, SiteEntropyOptions ))( &::genesis::sequence::site_entropy ),
            pybind11::arg("counts"),
            pybind11::arg("site_index"),
            pybind11::arg("options"),
        get_docstring("double ::genesis::sequence::site_entropy (SequenceCounts const & counts, size_t site_index, SiteEntropyOptions options)")
    );

    scope.def(
        "site_information",
        ( double ( * )( SequenceCounts const &, size_t, bool, SiteEntropyOptions ))( &::genesis::sequence::site_information ),
            pybind11::arg("counts"),
            pybind11::arg("site_index"),
            pybind11::arg("use_small_sample_correction")=(bool)(false),
            pybind11::arg("options"),
        get_docstring("double ::genesis::sequence::site_information (SequenceCounts const & counts, size_t site_index, bool use_small_sample_correction=false, SiteEntropyOptions options)")
    );
}
