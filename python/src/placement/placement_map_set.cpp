/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/placement_map_set.hpp"

using namespace genesis::placement;

PYTHON_EXPORT_CLASS (PlacementMapSet, "placement")
{
    boost::python::class_< ::genesis::placement::PlacementMapSet > ( "PlacementMapSet" )

        // Public Member Functions

        .def(
            "add",
            ( void ( ::genesis::placement::PlacementMapSet::* )( const std::string &, PlacementMap& ))( &::genesis::placement::PlacementMapSet::add ),
            ( boost::python::arg("name"), boost::python::arg("map") )
        )
        // .def(
        //     "cbegin",
        //     ( const_iterator ( ::genesis::placement::PlacementMapSet::* )(  ) const )( &::genesis::placement::PlacementMapSet::cbegin )
        // )
        // .def(
        //     "cend",
        //     ( const_iterator ( ::genesis::placement::PlacementMapSet::* )(  ) const )( &::genesis::placement::PlacementMapSet::cend )
        // )
        .def(
            "clear",
            ( void ( ::genesis::placement::PlacementMapSet::* )(  ))( &::genesis::placement::PlacementMapSet::clear )
        )
        .def(
            "dump",
            ( std::string ( ::genesis::placement::PlacementMapSet::* )( bool ))( &::genesis::placement::PlacementMapSet::dump ),
            ( boost::python::arg("full")=(bool)(false) )
        )
        .def(
            "empty",
            ( bool ( ::genesis::placement::PlacementMapSet::* )(  ) const )( &::genesis::placement::PlacementMapSet::empty ),
            get_docstring("bool ::genesis::placement::PlacementMapSet::empty () const")
        )
        // .def(
        //     "get_first",
        //     ( std::shared_ptr< PlacementMap > ( ::genesis::placement::PlacementMapSet::* )( const std::string & ))( &::genesis::placement::PlacementMapSet::get_first ),
        //     ( boost::python::arg("name") )
        // )
        .def(
            "merge_all",
            ( PlacementMap ( ::genesis::placement::PlacementMapSet::* )(  ))( &::genesis::placement::PlacementMapSet::merge_all ),
            get_docstring("PlacementMap ::genesis::placement::PlacementMapSet::merge_all ()")
        )
        .def(
            "size",
            ( size_t ( ::genesis::placement::PlacementMapSet::* )(  ) const )( &::genesis::placement::PlacementMapSet::size ),
            get_docstring("size_t ::genesis::placement::PlacementMapSet::size () const")
        )

        // Operators

        .def(
            "__len__",
            (::size_t ( ::genesis::placement::PlacementMapSet::* )(  ) const)( &::genesis::placement::PlacementMapSet::size ),
            get_docstring("size_t ::genesis::placement::PlacementMapSet::size () const")
        )
        // .def(
        //     "__getitem__",
        //     ( const ::genesis::placement::PlacementMapSet::NamedMap & ( ::genesis::placement::PlacementMapSet::* )( const std::size_t ) const )( &::genesis::placement::PlacementMapSet::operator[] ),
        //     ( boost::python::arg("index") )
        // )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::placement::PlacementMapSet::begin, &::genesis::placement::PlacementMapSet::end )
        )
    ;
}
