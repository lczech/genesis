/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_CLASS( ::genesis::sequence::SequenceCounts, scope )
{

    // -------------------------------------------------------------------
    //     Class SequenceCounts
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::sequence::SequenceCounts, std::shared_ptr<::genesis::sequence::SequenceCounts> > ( scope, "SequenceCounts" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< std::string const &, size_t >(),
            pybind11::arg("characters"),
            pybind11::arg("length")
        )
        .def(
            pybind11::init< SequenceCounts const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "add_sequence",
            ( void ( ::genesis::sequence::SequenceCounts::* )( Sequence const & ))( &::genesis::sequence::SequenceCounts::add_sequence ),
            pybind11::arg("sequence")
        )
        .def(
            "add_sequence",
            ( void ( ::genesis::sequence::SequenceCounts::* )( std::string const & ))( &::genesis::sequence::SequenceCounts::add_sequence ),
            pybind11::arg("sites")
        )
        .def(
            "add_sequences",
            ( void ( ::genesis::sequence::SequenceCounts::* )( SequenceSet const & ))( &::genesis::sequence::SequenceCounts::add_sequences ),
            pybind11::arg("sequences")
        )
        .def(
            "added_sequences_count",
            ( ::genesis::sequence::SequenceCounts::CountsIntType ( ::genesis::sequence::SequenceCounts::* )(  ) const )( &::genesis::sequence::SequenceCounts::added_sequences_count )
        )
        .def(
            "characters",
            ( std::string ( ::genesis::sequence::SequenceCounts::* )(  ) const )( &::genesis::sequence::SequenceCounts::characters )
        )
        .def(
            "clear",
            ( void ( ::genesis::sequence::SequenceCounts::* )(  ))( &::genesis::sequence::SequenceCounts::clear )
        )
        .def(
            "clear_counts",
            ( void ( ::genesis::sequence::SequenceCounts::* )(  ))( &::genesis::sequence::SequenceCounts::clear_counts )
        )
        .def(
            "count_at",
            ( ::genesis::sequence::SequenceCounts::CountsIntType ( ::genesis::sequence::SequenceCounts::* )( size_t, char ) const )( &::genesis::sequence::SequenceCounts::count_at ),
            pybind11::arg("site_index"),
            pybind11::arg("character")
        )
        .def(
            "count_at",
            ( ::genesis::sequence::SequenceCounts::CountsIntType ( ::genesis::sequence::SequenceCounts::* )( size_t, size_t ) const )( &::genesis::sequence::SequenceCounts::count_at ),
            pybind11::arg("site_index"),
            pybind11::arg("character_index")
        )
        .def(
            "length",
            ( size_t ( ::genesis::sequence::SequenceCounts::* )(  ) const )( &::genesis::sequence::SequenceCounts::length )
        )
    ;
}
