/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_CLASS( ::genesis::sequence::SequenceSet, scope )
{

    // -------------------------------------------------------------------
    //     Class SequenceSet
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::sequence::SequenceSet, std::shared_ptr<::genesis::sequence::SequenceSet> > ( scope, "SequenceSet" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< SequenceSet const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        // .def(
        //     "add",
        //     ( reference ( ::genesis::sequence::SequenceSet::* )( Sequence && ))( &::genesis::sequence::SequenceSet::add ),
        //     pybind11::arg("s"),
        //     get_docstring("reference ::genesis::sequence::SequenceSet::add (Sequence && s)")
        // )
        .def(
            "add",
            ( ::genesis::sequence::SequenceSet::reference ( ::genesis::sequence::SequenceSet::* )( Sequence const & ))( &::genesis::sequence::SequenceSet::add ),
            pybind11::arg("s"),
            get_docstring("reference ::genesis::sequence::SequenceSet::add (Sequence const & s)")
        )
        .def(
            "at",
            ( ::genesis::sequence::SequenceSet::const_reference ( ::genesis::sequence::SequenceSet::* )( size_t ) const )( &::genesis::sequence::SequenceSet::at ),
            pybind11::arg("index")
        )
        .def(
            "at",
            ( ::genesis::sequence::SequenceSet::reference ( ::genesis::sequence::SequenceSet::* )( size_t ))( &::genesis::sequence::SequenceSet::at ),
            pybind11::arg("index")
        )
        // .def(
        //     "cbegin",
        //     ( const_iterator ( ::genesis::sequence::SequenceSet::* )(  ) const )( &::genesis::sequence::SequenceSet::cbegin )
        // )
        // .def(
        //     "cend",
        //     ( const_iterator ( ::genesis::sequence::SequenceSet::* )(  ) const )( &::genesis::sequence::SequenceSet::cend )
        // )
        .def(
            "clear",
            ( void ( ::genesis::sequence::SequenceSet::* )(  ))( &::genesis::sequence::SequenceSet::clear ),
            get_docstring("void ::genesis::sequence::SequenceSet::clear ()")
        )
        .def(
            "empty",
            ( bool ( ::genesis::sequence::SequenceSet::* )(  ) const )( &::genesis::sequence::SequenceSet::empty )
        )
        // .def(
        //     "remove",
        //     ( void ( ::genesis::sequence::SequenceSet::* )( iterator ))( &::genesis::sequence::SequenceSet::remove ),
        //     pybind11::arg("position"),
        //     get_docstring("void ::genesis::sequence::SequenceSet::remove (iterator position)")
        // )
        // .def(
        //     "remove",
        //     ( void ( ::genesis::sequence::SequenceSet::* )( iterator, iterator ))( &::genesis::sequence::SequenceSet::remove ),
        //     pybind11::arg("first"),
        //     pybind11::arg("last"),
        //     get_docstring("void ::genesis::sequence::SequenceSet::remove (iterator first, iterator last)")
        // )
        .def(
            "remove",
            ( void ( ::genesis::sequence::SequenceSet::* )( size_t ))( &::genesis::sequence::SequenceSet::remove ),
            pybind11::arg("index"),
            get_docstring("void ::genesis::sequence::SequenceSet::remove (size_t index)")
        )
        .def(
            "remove",
            ( void ( ::genesis::sequence::SequenceSet::* )( size_t, size_t ))( &::genesis::sequence::SequenceSet::remove ),
            pybind11::arg("first_index"),
            pybind11::arg("last_index"),
            get_docstring("void ::genesis::sequence::SequenceSet::remove (size_t first_index, size_t last_index)")
        )
        .def(
            "size",
            ( size_t ( ::genesis::sequence::SequenceSet::* )(  ) const )( &::genesis::sequence::SequenceSet::size )
        )

        // Operators

        .def(
            "__getitem__",
            ( ::genesis::sequence::SequenceSet::reference ( ::genesis::sequence::SequenceSet::* )( size_t ))( &::genesis::sequence::SequenceSet::operator[] ),
            pybind11::arg("index")
        )
        .def(
            "__getitem__",
            ( ::genesis::sequence::SequenceSet::const_reference ( ::genesis::sequence::SequenceSet::* )( size_t ) const )( &::genesis::sequence::SequenceSet::operator[] ),
            pybind11::arg("index")
        )
        .def(
            "__str__",
            []( ::genesis::sequence::SequenceSet const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::sequence::SequenceSet& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            pybind11::keep_alive<0, 1>()
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS( sequence_sequence_set_export, ::genesis::sequence, scope )
{

    scope.def(
        "swap",
        ( void ( * )( SequenceSet &, SequenceSet & ))( &::genesis::sequence::swap ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );
}
