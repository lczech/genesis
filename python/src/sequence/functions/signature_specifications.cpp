/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_CLASS( ::genesis::sequence::SignatureSpecifications, scope )
{

    // -------------------------------------------------------------------
    //     Class SignatureSpecifications
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::sequence::SignatureSpecifications, std::shared_ptr<::genesis::sequence::SignatureSpecifications> > ( scope, "SignatureSpecifications" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< std::string const &, size_t >(),
            pybind11::arg("alphabet"),
            pybind11::arg("k")
        )
        .def(
            pybind11::init< SignatureSpecifications const & >(),
            pybind11::arg("arg"),
            get_docstring("::genesis::sequence::SignatureSpecifications::SignatureSpecifications (SignatureSpecifications const & )")
        )

        // Public Member Functions

        .def(
            "alphabet",
            ( std::string const & ( ::genesis::sequence::SignatureSpecifications::* )(  ) const )( &::genesis::sequence::SignatureSpecifications::alphabet )
        )
        .def(
            "char_index",
            ( size_t ( ::genesis::sequence::SignatureSpecifications::* )( char ) const )( &::genesis::sequence::SignatureSpecifications::char_index ),
            pybind11::arg("c"),
            get_docstring("size_t ::genesis::sequence::SignatureSpecifications::char_index (char c) const")
        )
        .def(
            "is_nucleic_acids",
            ( bool ( ::genesis::sequence::SignatureSpecifications::* )(  ) const )( &::genesis::sequence::SignatureSpecifications::is_nucleic_acids ),
            get_docstring("bool ::genesis::sequence::SignatureSpecifications::is_nucleic_acids () const")
        )
        .def(
            "k",
            ( size_t ( ::genesis::sequence::SignatureSpecifications::* )(  ) const )( &::genesis::sequence::SignatureSpecifications::k )
        )
        .def(
            "kmer_combined_reverse_complement_map",
            ( std::vector< size_t > const & ( ::genesis::sequence::SignatureSpecifications::* )(  ) const )( &::genesis::sequence::SignatureSpecifications::kmer_combined_reverse_complement_map ),
            get_docstring("std::vector< size_t > const & ::genesis::sequence::SignatureSpecifications::kmer_combined_reverse_complement_map () const")
        )
        .def(
            "kmer_list",
            ( std::vector< std::string > const & ( ::genesis::sequence::SignatureSpecifications::* )(  ) const )( &::genesis::sequence::SignatureSpecifications::kmer_list ),
            get_docstring("std::vector< std::string > const & ::genesis::sequence::SignatureSpecifications::kmer_list () const")
        )
        .def(
            "kmer_list_size",
            ( size_t ( ::genesis::sequence::SignatureSpecifications::* )(  ) const )( &::genesis::sequence::SignatureSpecifications::kmer_list_size )
        )
        .def(
            "kmer_reverse_complement_indices",
            ( std::vector< size_t > const & ( ::genesis::sequence::SignatureSpecifications::* )(  ) const )( &::genesis::sequence::SignatureSpecifications::kmer_reverse_complement_indices ),
            get_docstring("std::vector< size_t > const & ::genesis::sequence::SignatureSpecifications::kmer_reverse_complement_indices () const")
        )
        .def(
            "kmer_reverse_complement_list",
            ( std::vector< std::string > const & ( ::genesis::sequence::SignatureSpecifications::* )(  ) const )( &::genesis::sequence::SignatureSpecifications::kmer_reverse_complement_list )
        )
        .def(
            "kmer_reverse_complement_list_size",
            ( size_t ( ::genesis::sequence::SignatureSpecifications::* )( bool ) const )( &::genesis::sequence::SignatureSpecifications::kmer_reverse_complement_list_size ),
            pybind11::arg("with_palindromes")=(bool)(true)
        )
        .def(
            "unknown_char_behavior",
            ( ::genesis::sequence::SignatureSpecifications & ( ::genesis::sequence::SignatureSpecifications::* )( ::genesis::sequence::SignatureSpecifications::UnknownCharBehavior ))( &::genesis::sequence::SignatureSpecifications::unknown_char_behavior ),
            pybind11::arg("value")
        )
        .def(
            "unknown_char_behavior",
            ( ::genesis::sequence::SignatureSpecifications::UnknownCharBehavior ( ::genesis::sequence::SignatureSpecifications::* )(  ) const )( &::genesis::sequence::SignatureSpecifications::unknown_char_behavior )
        )
    ;
}
