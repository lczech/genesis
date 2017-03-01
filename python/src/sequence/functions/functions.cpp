/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_FUNCTIONS( sequence_functions_functions_export, ::genesis::sequence, scope )
{

    scope.def(
        "is_alignment",
        ( bool ( * )( SequenceSet const & ))( &::genesis::sequence::is_alignment ),
            pybind11::arg("set"),
        get_docstring("bool ::genesis::sequence::is_alignment (SequenceSet const & set)")
    );

    scope.def(
        "validate_chars",
        ( bool ( * )( SequenceSet const &, std::string const & ))( &::genesis::sequence::validate_chars ),
            pybind11::arg("set"),
            pybind11::arg("chars"),
        get_docstring("bool ::genesis::sequence::validate_chars (SequenceSet const & set, std::string const & chars)")
    );

    scope.def(
        "longest_sequence_length",
        ( size_t ( * )( SequenceSet const & ))( &::genesis::sequence::longest_sequence_length ),
            pybind11::arg("set"),
        get_docstring("size_t ::genesis::sequence::longest_sequence_length (SequenceSet const & set)")
    );

    scope.def(
        "total_length",
        ( size_t ( * )( SequenceSet const & ))( &::genesis::sequence::total_length ),
            pybind11::arg("set"),
        get_docstring("size_t ::genesis::sequence::total_length (SequenceSet const & set)")
    );

    scope.def(
        "gap_sites",
        ( utils::Bitvector ( * )( Sequence const &, std::string const & ))( &::genesis::sequence::gap_sites ),
            pybind11::arg("seq"),
            pybind11::arg("gap_chars"),
        get_docstring("utils::Bitvector ::genesis::sequence::gap_sites (Sequence const & seq, std::string const & gap_chars)")
    );

    scope.def(
        "gap_sites",
        ( utils::Bitvector ( * )( SequenceSet const &, std::string const & ))( &::genesis::sequence::gap_sites ),
            pybind11::arg("set"),
            pybind11::arg("gap_chars"),
        get_docstring("utils::Bitvector ::genesis::sequence::gap_sites (SequenceSet const & set, std::string const & gap_chars)")
    );

    scope.def(
        "filter_max_sequence_length",
        ( void ( * )( SequenceSet &, size_t ))( &::genesis::sequence::filter_max_sequence_length ),
            pybind11::arg("set"),
            pybind11::arg("max_length"),
        get_docstring("void ::genesis::sequence::filter_max_sequence_length (SequenceSet & set, size_t max_length)")
    );

    scope.def(
        "filter_min_max_sequence_length",
        ( void ( * )( SequenceSet &, size_t, size_t ))( &::genesis::sequence::filter_min_max_sequence_length ),
            pybind11::arg("set"),
            pybind11::arg("min_length"),
            pybind11::arg("max_length"),
        get_docstring("void ::genesis::sequence::filter_min_max_sequence_length (SequenceSet & set, size_t min_length, size_t max_length)")
    );

    scope.def(
        "filter_min_sequence_length",
        ( void ( * )( SequenceSet &, size_t ))( &::genesis::sequence::filter_min_sequence_length ),
            pybind11::arg("set"),
            pybind11::arg("min_length"),
        get_docstring("void ::genesis::sequence::filter_min_sequence_length (SequenceSet & set, size_t min_length)")
    );

    scope.def(
        "merge_duplicate_sequences",
        ( void ( * )( SequenceSet &, MergeDuplicateSequencesCountPolicy, std::string const & ))( &::genesis::sequence::merge_duplicate_sequences ),
            pybind11::arg("set"),
            pybind11::arg("count_policy"),
            pybind11::arg("counter_prefix")=(std::string const &)("_"),
        get_docstring("void ::genesis::sequence::merge_duplicate_sequences (SequenceSet & set, MergeDuplicateSequencesCountPolicy count_policy, std::string const & counter_prefix=\"_\")")
    );

    scope.def(
        "remove_characters",
        ( void ( * )( Sequence &, std::string const & ))( &::genesis::sequence::remove_characters ),
            pybind11::arg("seq"),
            pybind11::arg("search"),
        get_docstring("void ::genesis::sequence::remove_characters (Sequence & seq, std::string const & search)")
    );

    scope.def(
        "remove_characters",
        ( void ( * )( SequenceSet &, std::string const & ))( &::genesis::sequence::remove_characters ),
            pybind11::arg("set"),
            pybind11::arg("search"),
        get_docstring("void ::genesis::sequence::remove_characters (SequenceSet & set, std::string const & search)")
    );

    scope.def(
        "remove_gaps",
        ( void ( * )( Sequence &, std::string const & ))( &::genesis::sequence::remove_gaps ),
            pybind11::arg("seq"),
            pybind11::arg("gap_chars"),
        get_docstring("void ::genesis::sequence::remove_gaps (Sequence & seq, std::string const & gap_chars)")
    );

    scope.def(
        "remove_gaps",
        ( void ( * )( SequenceSet &, std::string const & ))( &::genesis::sequence::remove_gaps ),
            pybind11::arg("set"),
            pybind11::arg("gap_chars"),
        get_docstring("void ::genesis::sequence::remove_gaps (SequenceSet & set, std::string const & gap_chars)")
    );

    scope.def(
        "remove_sites",
        ( void ( * )( Sequence &, utils::Bitvector ))( &::genesis::sequence::remove_sites ),
            pybind11::arg("seq"),
            pybind11::arg("sites"),
        get_docstring("void ::genesis::sequence::remove_sites (Sequence & seq, utils::Bitvector sites)")
    );

    scope.def(
        "remove_sites",
        ( void ( * )( SequenceSet &, utils::Bitvector ))( &::genesis::sequence::remove_sites ),
            pybind11::arg("set"),
            pybind11::arg("sites"),
        get_docstring("void ::genesis::sequence::remove_sites (SequenceSet & set, utils::Bitvector sites)")
    );

    scope.def(
        "replace_characters",
        ( void ( * )( Sequence &, std::string const &, char ))( &::genesis::sequence::replace_characters ),
            pybind11::arg("seq"),
            pybind11::arg("search"),
            pybind11::arg("replacement"),
        get_docstring("void ::genesis::sequence::replace_characters (Sequence & seq, std::string const & search, char replacement)")
    );

    scope.def(
        "replace_characters",
        ( void ( * )( SequenceSet &, std::string const &, char ))( &::genesis::sequence::replace_characters ),
            pybind11::arg("set"),
            pybind11::arg("search"),
            pybind11::arg("replacement"),
        get_docstring("void ::genesis::sequence::replace_characters (SequenceSet & set, std::string const & search, char replacement)")
    );

    scope.def(
        "replace_t_with_u",
        ( void ( * )( Sequence & ))( &::genesis::sequence::replace_t_with_u ),
            pybind11::arg("seq"),
        get_docstring("void ::genesis::sequence::replace_t_with_u (Sequence & seq)")
    );

    scope.def(
        "replace_t_with_u",
        ( void ( * )( SequenceSet & ))( &::genesis::sequence::replace_t_with_u ),
            pybind11::arg("set"),
        get_docstring("void ::genesis::sequence::replace_t_with_u (SequenceSet & set)")
    );

    scope.def(
        "replace_u_with_t",
        ( void ( * )( Sequence & ))( &::genesis::sequence::replace_u_with_t ),
            pybind11::arg("seq"),
        get_docstring("void ::genesis::sequence::replace_u_with_t (Sequence & seq)")
    );

    scope.def(
        "replace_u_with_t",
        ( void ( * )( SequenceSet & ))( &::genesis::sequence::replace_u_with_t ),
            pybind11::arg("set"),
        get_docstring("void ::genesis::sequence::replace_u_with_t (SequenceSet & set)")
    );
}
