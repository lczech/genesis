/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::TreeSet, scope )
{

    // -------------------------------------------------------------------
    //     Class TreeSet
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::TreeSet, std::shared_ptr<::genesis::tree::TreeSet> > ( scope, "TreeSet" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< TreeSet const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "add",
            ( void ( ::genesis::tree::TreeSet::* )( std::string const &, Tree const & ))( &::genesis::tree::TreeSet::add ),
            pybind11::arg("name"),
            pybind11::arg("tree")
        )
        .def(
            "at",
            ( ::genesis::tree::TreeSet::NamedTree & ( ::genesis::tree::TreeSet::* )( size_t ))( &::genesis::tree::TreeSet::at ),
            pybind11::arg("index")
        )
        .def(
            "at",
            ( const ::genesis::tree::TreeSet::NamedTree & ( ::genesis::tree::TreeSet::* )( size_t ) const )( &::genesis::tree::TreeSet::at ),
            pybind11::arg("index")
        )
        .def(
            "clear",
            ( void ( ::genesis::tree::TreeSet::* )(  ))( &::genesis::tree::TreeSet::clear )
        )
        .def(
            "empty",
            ( bool ( ::genesis::tree::TreeSet::* )(  ) const )( &::genesis::tree::TreeSet::empty )
        )
        .def(
            "remove_at",
            ( void ( ::genesis::tree::TreeSet::* )( size_t ))( &::genesis::tree::TreeSet::remove_at ),
            pybind11::arg("index")
        )
        .def(
            "size",
            ( size_t ( ::genesis::tree::TreeSet::* )(  ) const )( &::genesis::tree::TreeSet::size )
        )
        .def(
            "swap",
            ( void ( ::genesis::tree::TreeSet::* )( TreeSet & ))( &::genesis::tree::TreeSet::swap ),
            pybind11::arg("other")
        )

        // Operators

        .def(
            "__getitem__",
            ( ::genesis::tree::TreeSet::NamedTree & ( ::genesis::tree::TreeSet::* )( const std::size_t ))( &::genesis::tree::TreeSet::operator[] ),
            pybind11::arg("index")
        )
        .def(
            "__getitem__",
            ( const ::genesis::tree::TreeSet::NamedTree & ( ::genesis::tree::TreeSet::* )( const std::size_t ) const )( &::genesis::tree::TreeSet::operator[] ),
            pybind11::arg("index")
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::tree::TreeSet& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            pybind11::keep_alive<0, 1>()
        )
    ;
}
