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
        "consensus_sequence_cavener",
        ( std::string ( * )( SequenceSet const &, bool ))( &::genesis::sequence::consensus_sequence_cavener ),
            pybind11::arg("sequences"),
            pybind11::arg("allow_gaps")=(bool)(true),
        get_docstring("std::string ::genesis::sequence::consensus_sequence_cavener (SequenceSet const & sequences, bool allow_gaps=true)")
    );

    scope.def(
        "consensus_sequence_cavener",
        ( std::string ( * )( SiteCounts const &, bool ))( &::genesis::sequence::consensus_sequence_cavener ),
            pybind11::arg("counts"),
            pybind11::arg("allow_gaps")=(bool)(true),
        get_docstring("std::string ::genesis::sequence::consensus_sequence_cavener (SiteCounts const & counts, bool allow_gaps=true)")
    );

    scope.def(
        "consensus_sequence_with_ambiguities",
        ( std::string ( * )( SequenceSet const &, double, bool ))( &::genesis::sequence::consensus_sequence_with_ambiguities ),
            pybind11::arg("sequences"),
            pybind11::arg("similarity_factor")=(double)(0.9),
            pybind11::arg("allow_gaps")=(bool)(true),
        get_docstring("std::string ::genesis::sequence::consensus_sequence_with_ambiguities (SequenceSet const & sequences, double similarity_factor=0.9, bool allow_gaps=true)")
    );

    scope.def(
        "consensus_sequence_with_ambiguities",
        ( std::string ( * )( SiteCounts const &, double, bool ))( &::genesis::sequence::consensus_sequence_with_ambiguities ),
            pybind11::arg("counts"),
            pybind11::arg("similarity_factor")=(double)(0.9),
            pybind11::arg("allow_gaps")=(bool)(true),
        get_docstring("std::string ::genesis::sequence::consensus_sequence_with_ambiguities (SiteCounts const & counts, double similarity_factor=0.9, bool allow_gaps=true)")
    );

    scope.def(
        "consensus_sequence_with_majorities",
        ( std::string ( * )( SequenceSet const &, bool ))( &::genesis::sequence::consensus_sequence_with_majorities ),
            pybind11::arg("sequences"),
            pybind11::arg("allow_gaps")=(bool)(true),
        get_docstring("std::string ::genesis::sequence::consensus_sequence_with_majorities (SequenceSet const & sequences, bool allow_gaps=true)")
    );

    scope.def(
        "consensus_sequence_with_majorities",
        ( std::string ( * )( SequenceSet const &, std::string const &, bool, char ))( &::genesis::sequence::consensus_sequence_with_majorities ),
            pybind11::arg("sequences"),
            pybind11::arg("characters"),
            pybind11::arg("allow_gaps")=(bool)(true),
            pybind11::arg("gap_char")=(char)('-'),
        get_docstring("std::string ::genesis::sequence::consensus_sequence_with_majorities (SequenceSet const & sequences, std::string const & characters, bool allow_gaps=true, char gap_char='-')")
    );

    scope.def(
        "consensus_sequence_with_majorities",
        ( std::string ( * )( SiteCounts const &, bool, char ))( &::genesis::sequence::consensus_sequence_with_majorities ),
            pybind11::arg("counts"),
            pybind11::arg("allow_gaps")=(bool)(true),
            pybind11::arg("gap_char")=(char)('-'),
        get_docstring("std::string ::genesis::sequence::consensus_sequence_with_majorities (SiteCounts const & counts, bool allow_gaps=true, char gap_char='-')")
    );

    scope.def(
        "consensus_sequence_with_threshold",
        ( std::string ( * )( SequenceSet const &, double, bool, bool ))( &::genesis::sequence::consensus_sequence_with_threshold ),
            pybind11::arg("sequences"),
            pybind11::arg("frequency_threshold")=(double)(0.6),
            pybind11::arg("allow_gaps")=(bool)(true),
            pybind11::arg("use_ambiguities")=(bool)(true),
        get_docstring("std::string ::genesis::sequence::consensus_sequence_with_threshold (SequenceSet const & sequences, double frequency_threshold=0.6, bool allow_gaps=true, bool use_ambiguities=true)")
    );

    scope.def(
        "consensus_sequence_with_threshold",
        ( std::string ( * )( SiteCounts const &, double, bool, bool ))( &::genesis::sequence::consensus_sequence_with_threshold ),
            pybind11::arg("counts"),
            pybind11::arg("frequency_threshold")=(double)(0.6),
            pybind11::arg("allow_gaps")=(bool)(true),
            pybind11::arg("use_ambiguities")=(bool)(true),
        get_docstring("std::string ::genesis::sequence::consensus_sequence_with_threshold (SiteCounts const & counts, double frequency_threshold=0.6, bool allow_gaps=true, bool use_ambiguities=true)")
    );
}
