/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_CLASS (SequenceSet, "sequence")
{

    // -------------------------------------------------------------------
    //     Class SequenceSet
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::sequence::SequenceSet > ( "SequenceSet", boost::python::init<  >(  ) )
        .def( boost::python::init< SequenceSet const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "cbegin",
            ( ::genesis::sequence::SequenceSet::const_iterator ( ::genesis::sequence::SequenceSet::* )(  ) const )( &::genesis::sequence::SequenceSet::cbegin )
        )
        .def(
            "cend",
            ( ::genesis::sequence::SequenceSet::const_iterator ( ::genesis::sequence::SequenceSet::* )(  ) const )( &::genesis::sequence::SequenceSet::cend )
        )
        .def(
            "clear",
            ( void ( ::genesis::sequence::SequenceSet::* )(  ))( &::genesis::sequence::SequenceSet::clear ),
            get_docstring("void ::genesis::sequence::SequenceSet::clear ()")
        )
        // .def(
        //     "push_back",
        //     ( void ( ::genesis::sequence::SequenceSet::* )( Sequence && ))( &::genesis::sequence::SequenceSet::push_back ),
        //     ( boost::python::arg("s") )
        // )
        .def(
            "push_back",
            ( void ( ::genesis::sequence::SequenceSet::* )( Sequence const & ))( &::genesis::sequence::SequenceSet::push_back ),
            ( boost::python::arg("s") )
        )
        .def(
            "size",
            ( size_t ( ::genesis::sequence::SequenceSet::* )(  ) const )( &::genesis::sequence::SequenceSet::size )
        )
        .def(
            "swap",
            ( void ( ::genesis::sequence::SequenceSet::* )( SequenceSet & ))( &::genesis::sequence::SequenceSet::swap ),
            ( boost::python::arg("other") )
        )

        // Operators

        // .def(
        //     "__getitem__",
        //     ( reference ( ::genesis::sequence::SequenceSet::* )( size_t ))( &::genesis::sequence::SequenceSet::operator[] ),
        //     ( boost::python::arg("index") )
        // )
        // .def(
        //     "__getitem__",
        //     ( const_reference ( ::genesis::sequence::SequenceSet::* )( size_t ) const )( &::genesis::sequence::SequenceSet::operator[] ),
        //     ( boost::python::arg("index") )
        // )

        // Iterators

        // .def(
        //     "__iter__",
        //     boost::python::range ( &::genesis::sequence::SequenceSet::begin, &::genesis::sequence::SequenceSet::end )
        // )
    ;
}
