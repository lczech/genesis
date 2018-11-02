/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_FUNCTIONS( sequence_functions_signatures_export, ::genesis::sequence, scope )
{

    scope.def(
        "kmer_string_overlapping",
        ( std::string ( * )( Sequence const &, SignatureSpecifications const & ))( &::genesis::sequence::kmer_string_overlapping ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
        get_docstring("std::string ::genesis::sequence::kmer_string_overlapping (Sequence const & sequence, SignatureSpecifications const & settings)")
    );

    scope.def(
        "signature_frequencies",
        ( std::vector< double > ( * )( Sequence const &, SignatureSpecifications const & ))( &::genesis::sequence::signature_frequencies ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
        get_docstring("std::vector< double > ::genesis::sequence::signature_frequencies (Sequence const & sequence, SignatureSpecifications const & settings)")
    );

    scope.def(
        "signature_frequency_ratios_1",
        ( std::vector< double > ( * )( Sequence const &, SignatureSpecifications const & ))( &::genesis::sequence::signature_frequency_ratios_1 ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
        get_docstring("std::vector< double > ::genesis::sequence::signature_frequency_ratios_1 (Sequence const & sequence, SignatureSpecifications const & settings)")
    );

    scope.def(
        "signature_frequency_ratios_2",
        ( std::vector< double > ( * )( Sequence const &, SignatureSpecifications const & ))( &::genesis::sequence::signature_frequency_ratios_2 ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
        get_docstring("std::vector< double > ::genesis::sequence::signature_frequency_ratios_2 (Sequence const & sequence, SignatureSpecifications const & settings)")
    );

    scope.def(
        "signature_jensen_shannon",
        ( std::vector< double > ( * )( Sequence const &, SignatureSpecifications const & ))( &::genesis::sequence::signature_jensen_shannon ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
        get_docstring("std::vector< double > ::genesis::sequence::signature_jensen_shannon (Sequence const & sequence, SignatureSpecifications const & settings)")
    );

    scope.def(
        "signature_maximal_complementarity_frequencies",
        ( std::vector< double > ( * )( Sequence const &, SignatureSpecifications const & ))( &::genesis::sequence::signature_maximal_complementarity_frequencies ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
        get_docstring("std::vector< double > ::genesis::sequence::signature_maximal_complementarity_frequencies (Sequence const & sequence, SignatureSpecifications const & settings)")
    );

    scope.def(
        "signature_minimal_complementarity_frequencies",
        ( std::vector< double > ( * )( Sequence const &, SignatureSpecifications const & ))( &::genesis::sequence::signature_minimal_complementarity_frequencies ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
        get_docstring("std::vector< double > ::genesis::sequence::signature_minimal_complementarity_frequencies (Sequence const & sequence, SignatureSpecifications const & settings)")
    );

    scope.def(
        "signature_reverse_identity_frequencies",
        ( std::vector< double > ( * )( Sequence const &, SignatureSpecifications const & ))( &::genesis::sequence::signature_reverse_identity_frequencies ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
        get_docstring("std::vector< double > ::genesis::sequence::signature_reverse_identity_frequencies (Sequence const & sequence, SignatureSpecifications const & settings)")
    );

    scope.def(
        "signature_symmetrized_frequencies",
        ( std::vector< double > ( * )( Sequence const &, SignatureSpecifications const & ))( &::genesis::sequence::signature_symmetrized_frequencies ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
        get_docstring("std::vector< double > ::genesis::sequence::signature_symmetrized_frequencies (Sequence const & sequence, SignatureSpecifications const & settings)")
    );

    scope.def(
        "signature_counts",
        ( std::vector< size_t > ( * )( Sequence const &, SignatureSpecifications const & ))( &::genesis::sequence::signature_counts ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
        get_docstring("std::vector< size_t > ::genesis::sequence::signature_counts (Sequence const & sequence, SignatureSpecifications const & settings)")
    );

    scope.def(
        "signature_ranks",
        ( std::vector< size_t > ( * )( Sequence const &, SignatureSpecifications const & ))( &::genesis::sequence::signature_ranks ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
        get_docstring("std::vector< size_t > ::genesis::sequence::signature_ranks (Sequence const & sequence, SignatureSpecifications const & settings)")
    );

    scope.def(
        "signature_symmetrized_counts",
        ( std::vector< size_t > ( * )( Sequence const &, SignatureSpecifications const & ))( &::genesis::sequence::signature_symmetrized_counts ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
        get_docstring("std::vector< size_t > ::genesis::sequence::signature_symmetrized_counts (Sequence const & sequence, SignatureSpecifications const & settings)")
    );

    scope.def(
        "signature_symmetrized_ranks",
        ( std::vector< size_t > ( * )( Sequence const &, SignatureSpecifications const & ))( &::genesis::sequence::signature_symmetrized_ranks ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
        get_docstring("std::vector< size_t > ::genesis::sequence::signature_symmetrized_ranks (Sequence const & sequence, SignatureSpecifications const & settings)")
    );

    scope.def(
        "kmer_strings_non_overlapping",
        ( std::vector< std::string > ( * )( Sequence const &, SignatureSpecifications const & ))( &::genesis::sequence::kmer_strings_non_overlapping ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
        get_docstring("std::vector< std::string > ::genesis::sequence::kmer_strings_non_overlapping (Sequence const & sequence, SignatureSpecifications const & settings)")
    );

    scope.def(
        "kmer_string_overlapping",
        ( void ( * )( Sequence const &, SignatureSpecifications const &, std::ostream & ))( &::genesis::sequence::kmer_string_overlapping ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
            pybind11::arg("out"),
        get_docstring("void ::genesis::sequence::kmer_string_overlapping (Sequence const & sequence, SignatureSpecifications const & settings, std::ostream & out)")
    );

    scope.def(
        "kmer_strings_non_overlapping",
        ( void ( * )( Sequence const &, SignatureSpecifications const &, std::ostream & ))( &::genesis::sequence::kmer_strings_non_overlapping ),
            pybind11::arg("sequence"),
            pybind11::arg("settings"),
            pybind11::arg("out"),
        get_docstring("void ::genesis::sequence::kmer_strings_non_overlapping (Sequence const & sequence, SignatureSpecifications const & settings, std::ostream & out)")
    );
}
