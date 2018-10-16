/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_CLASS( ::genesis::sequence::SiteCounts, scope )
{

    // -------------------------------------------------------------------
    //     Class SiteCounts
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::sequence::SiteCounts, std::shared_ptr<::genesis::sequence::SiteCounts> > ( scope, "SiteCounts" )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::sequence::SiteCounts::SiteCounts ()")
        )
        .def(
            pybind11::init< std::string const &, size_t >(),
            pybind11::arg("characters"),
            pybind11::arg("length"),
            get_docstring("::genesis::sequence::SiteCounts::SiteCounts (std::string const & characters, size_t length)")
        )
        .def(
            pybind11::init< SiteCounts const & >(),
            pybind11::arg("arg"),
            get_docstring("::genesis::sequence::SiteCounts::SiteCounts (SiteCounts const & )")
        )

        // Public Member Functions

        .def(
            "add_sequence",
            ( void ( ::genesis::sequence::SiteCounts::* )( Sequence const &, bool ))( &::genesis::sequence::SiteCounts::add_sequence ),
            pybind11::arg("sequence"),
            pybind11::arg("use_abundance")=(bool)(true),
            get_docstring("void ::genesis::sequence::SiteCounts::add_sequence (Sequence const & sequence, bool use_abundance=true)")
        )
        .def(
            "add_sequence",
            ( void ( ::genesis::sequence::SiteCounts::* )( std::string const &, genesis::sequence::SiteCounts::CountsIntType ))( &::genesis::sequence::SiteCounts::add_sequence ),
            pybind11::arg("sites"),
            pybind11::arg("weight")=(genesis::sequence::SiteCounts::CountsIntType)(1),
            get_docstring("void ::genesis::sequence::SiteCounts::add_sequence (std::string const & sites, CountsIntType weight=1)")
        )
        .def(
            "add_sequences",
            ( void ( ::genesis::sequence::SiteCounts::* )( SequenceSet const & ))( &::genesis::sequence::SiteCounts::add_sequences ),
            pybind11::arg("sequences"),
            get_docstring("void ::genesis::sequence::SiteCounts::add_sequences (SequenceSet const & sequences)")
        )
        .def(
            "added_sequences_count",
            ( genesis::sequence::SiteCounts::CountsIntType ( ::genesis::sequence::SiteCounts::* )(  ) const )( &::genesis::sequence::SiteCounts::added_sequences_count ),
            get_docstring("CountsIntType ::genesis::sequence::SiteCounts::added_sequences_count () const")
        )
        .def(
            "characters",
            ( std::string ( ::genesis::sequence::SiteCounts::* )(  ) const )( &::genesis::sequence::SiteCounts::characters ),
            get_docstring("std::string ::genesis::sequence::SiteCounts::characters () const")
        )
        .def(
            "clear",
            ( void ( ::genesis::sequence::SiteCounts::* )(  ))( &::genesis::sequence::SiteCounts::clear ),
            get_docstring("void ::genesis::sequence::SiteCounts::clear ()")
        )
        .def(
            "clear_counts",
            ( void ( ::genesis::sequence::SiteCounts::* )(  ))( &::genesis::sequence::SiteCounts::clear_counts ),
            get_docstring("void ::genesis::sequence::SiteCounts::clear_counts ()")
        )
        .def(
            "count_at",
            ( genesis::sequence::SiteCounts::CountsIntType ( ::genesis::sequence::SiteCounts::* )( size_t, size_t ) const )( &::genesis::sequence::SiteCounts::count_at ),
            pybind11::arg("character_index"),
            pybind11::arg("site_index"),
            get_docstring("CountsIntType ::genesis::sequence::SiteCounts::count_at (size_t character_index, size_t site_index) const")
        )
        .def(
            "count_of",
            ( genesis::sequence::SiteCounts::CountsIntType ( ::genesis::sequence::SiteCounts::* )( char, size_t ) const )( &::genesis::sequence::SiteCounts::count_of ),
            pybind11::arg("character"),
            pybind11::arg("site_index"),
            get_docstring("CountsIntType ::genesis::sequence::SiteCounts::count_of (char character, size_t site_index) const")
        )
        .def(
            "length",
            ( size_t ( ::genesis::sequence::SiteCounts::* )(  ) const )( &::genesis::sequence::SiteCounts::length ),
            get_docstring("size_t ::genesis::sequence::SiteCounts::length () const")
        )
    ;
}
