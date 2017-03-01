/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_FUNCTIONS( sequence_functions_labels_export, ::genesis::sequence, scope )
{

    scope.def(
        "find_sequence",
        ( Sequence const * ( * )( SequenceSet const &, std::string const & ))( &::genesis::sequence::find_sequence ),
            pybind11::arg("set"),
            pybind11::arg("label"),
        get_docstring("Sequence const * ::genesis::sequence::find_sequence (SequenceSet const & set, std::string const & label)")
    );

    scope.def(
        "has_unique_labels",
        ( bool ( * )( SequenceSet const &, bool ))( &::genesis::sequence::has_unique_labels ),
            pybind11::arg("set"),
            pybind11::arg("case_sensitive")=(bool)(true),
        get_docstring("bool ::genesis::sequence::has_unique_labels (SequenceSet const & set, bool case_sensitive=true)")
    );

    scope.def(
        "has_valid_label",
        ( bool ( * )( Sequence const & ))( &::genesis::sequence::has_valid_label ),
            pybind11::arg("seq"),
        get_docstring("bool ::genesis::sequence::has_valid_label (Sequence const & seq)")
    );

    scope.def(
        "has_valid_labels",
        ( bool ( * )( SequenceSet const & ))( &::genesis::sequence::has_valid_labels ),
            pybind11::arg("set"),
        get_docstring("bool ::genesis::sequence::has_valid_labels (SequenceSet const & set)")
    );

    scope.def(
        "is_valid_label",
        ( bool ( * )( std::string const & ))( &::genesis::sequence::is_valid_label ),
            pybind11::arg("label"),
        get_docstring("bool ::genesis::sequence::is_valid_label (std::string const & label)")
    );

    scope.def(
        "sanitize_label",
        ( std::string ( * )( std::string const & ))( &::genesis::sequence::sanitize_label ),
            pybind11::arg("label"),
        get_docstring("std::string ::genesis::sequence::sanitize_label (std::string const & label)")
    );

    scope.def(
        "labels",
        ( std::unordered_set< std::string > ( * )( SequenceSet const & ))( &::genesis::sequence::labels ),
            pybind11::arg("set"),
        get_docstring("std::unordered_set< std::string > ::genesis::sequence::labels (SequenceSet const & set)")
    );

    scope.def(
        "filter_by_label_list",
        ( void ( * )( SequenceSet &, std::unordered_set< std::string > const &, bool ))( &::genesis::sequence::filter_by_label_list ),
            pybind11::arg("set"),
            pybind11::arg("labels"),
            pybind11::arg("invert")=(bool)(false),
        get_docstring("void ::genesis::sequence::filter_by_label_list (SequenceSet & set, std::unordered_set< std::string > const & labels, bool invert=false)")
    );

    scope.def(
        "relabel_sha1",
        ( void ( * )( Sequence & ))( &::genesis::sequence::relabel_sha1 ),
            pybind11::arg("seq"),
        get_docstring("void ::genesis::sequence::relabel_sha1 (Sequence & seq)")
    );

    scope.def(
        "relabel_sha1",
        ( void ( * )( SequenceSet & ))( &::genesis::sequence::relabel_sha1 ),
            pybind11::arg("set"),
        get_docstring("void ::genesis::sequence::relabel_sha1 (SequenceSet & set)")
    );

    scope.def(
        "sanitize_label",
        ( void ( * )( Sequence & ))( &::genesis::sequence::sanitize_label ),
            pybind11::arg("seq"),
        get_docstring("void ::genesis::sequence::sanitize_label (Sequence & seq)")
    );

    scope.def(
        "sanitize_labels",
        ( void ( * )( SequenceSet & ))( &::genesis::sequence::sanitize_labels ),
            pybind11::arg("set"),
        get_docstring("void ::genesis::sequence::sanitize_labels (SequenceSet & set)")
    );
}
