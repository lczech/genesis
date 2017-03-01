/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_FUNCTIONS( sequence_functions_consensus_export, ::genesis::sequence, scope )
{

    scope.def(
        "consensus_sequence_with_ambiguities",
        ( std::string ( * )( SequenceCounts const &, double, bool ))( &::genesis::sequence::consensus_sequence_with_ambiguities ),
            pybind11::arg("counts"),
            pybind11::arg("similarity_factor")=(double)(0.9),
            pybind11::arg("allow_gaps")=(bool)(true)
    );

    scope.def(
        "consensus_sequence_with_ambiguities",
        ( std::string ( * )( SequenceSet const &, double, bool ))( &::genesis::sequence::consensus_sequence_with_ambiguities ),
            pybind11::arg("sequences"),
            pybind11::arg("similarity_factor")=(double)(0.9),
            pybind11::arg("allow_gaps")=(bool)(true)
    );

    scope.def(
        "consensus_sequence_with_majorities",
        ( std::string ( * )( SequenceCounts const &, bool, char ))( &::genesis::sequence::consensus_sequence_with_majorities ),
            pybind11::arg("counts"),
            pybind11::arg("allow_gaps")=(bool)(true),
            pybind11::arg("gap_char")=(char)('-')
    );

    scope.def(
        "consensus_sequence_with_majorities",
        ( std::string ( * )( SequenceSet const &, bool ))( &::genesis::sequence::consensus_sequence_with_majorities ),
            pybind11::arg("sequences"),
            pybind11::arg("allow_gaps")=(bool)(true)
    );

    scope.def(
        "consensus_sequence_with_majorities",
        ( std::string ( * )( SequenceSet const &, std::string const &, bool, char ))( &::genesis::sequence::consensus_sequence_with_majorities ),
            pybind11::arg("sequences"),
            pybind11::arg("characters"),
            pybind11::arg("allow_gaps")=(bool)(true),
            pybind11::arg("gap_char")=(char)('-')
    );

    scope.def(
        "consensus_sequence_with_threshold",
        ( std::string ( * )( SequenceCounts const &, double, bool, bool ))( &::genesis::sequence::consensus_sequence_with_threshold ),
            pybind11::arg("counts"),
            pybind11::arg("frequency_threshold")=(double)(0.6),
            pybind11::arg("allow_gaps")=(bool)(true),
            pybind11::arg("use_ambiguities")=(bool)(true)
    );

    scope.def(
        "consensus_sequence_with_threshold",
        ( std::string ( * )( SequenceSet const &, double, bool, bool ))( &::genesis::sequence::consensus_sequence_with_threshold ),
            pybind11::arg("sequences"),
            pybind11::arg("frequency_threshold")=(double)(0.6),
            pybind11::arg("allow_gaps")=(bool)(true),
            pybind11::arg("use_ambiguities")=(bool)(true)
    );
}
