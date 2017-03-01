/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_FUNCTIONS( sequence_functions_stats_export, ::genesis::sequence, scope )
{

    scope.def(
        "gapyness",
        ( double ( * )( SequenceSet const &, std::string const & ))( &::genesis::sequence::gapyness ),
            pybind11::arg("set"),
            pybind11::arg("gap_chars"),
        get_docstring("double ::genesis::sequence::gapyness (SequenceSet const & set, std::string const & gap_chars)")
    );

    scope.def(
        "count_chars",
        ( size_t ( * )( SequenceSet const &, std::string const & ))( &::genesis::sequence::count_chars ),
            pybind11::arg("set"),
            pybind11::arg("chars"),
        get_docstring("size_t ::genesis::sequence::count_chars (SequenceSet const & set, std::string const & chars)")
    );

    scope.def(
        "gap_site_count",
        ( size_t ( * )( SequenceCounts const & ))( &::genesis::sequence::gap_site_count ),
            pybind11::arg("counts")
    );

    scope.def(
        "base_frequencies",
        ( std::map< char, double > ( * )( Sequence const &, std::string const & ))( &::genesis::sequence::base_frequencies ),
            pybind11::arg("seq"),
            pybind11::arg("plain_chars"),
        get_docstring("std::map< char, double > ::genesis::sequence::base_frequencies (Sequence const & seq, std::string const & plain_chars)")
    );

    scope.def(
        "base_frequencies",
        ( std::map< char, double > ( * )( SequenceSet const &, std::string const & ))( &::genesis::sequence::base_frequencies ),
            pybind11::arg("set"),
            pybind11::arg("plain_chars"),
        get_docstring("std::map< char, double > ::genesis::sequence::base_frequencies (SequenceSet const & set, std::string const & plain_chars)")
    );

    scope.def(
        "site_histogram",
        ( std::map< char, size_t > ( * )( Sequence const & ))( &::genesis::sequence::site_histogram ),
            pybind11::arg("seq"),
        get_docstring("std::map< char, size_t > ::genesis::sequence::site_histogram (Sequence const & seq)")
    );

    scope.def(
        "site_histogram",
        ( std::map< char, size_t > ( * )( SequenceSet const & ))( &::genesis::sequence::site_histogram ),
            pybind11::arg("set"),
        get_docstring("std::map< char, size_t > ::genesis::sequence::site_histogram (SequenceSet const & set)")
    );
}
