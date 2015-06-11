/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "placement/placement_map_set.hpp"

const char* get_docstring (const std::string& signature);

void BoostPythonExport_PlacementMapSet()
{
    using namespace genesis;

    boost::python::class_< ::genesis::PlacementMapSet > ( "PlacementMapSet" )

        // Public Member Functions

        .def(
            "add",
            ( void ( ::genesis::PlacementMapSet::* )( const std::string &, PlacementMap * ))( &::genesis::PlacementMapSet::add ),
            ( boost::python::arg("name"), boost::python::arg("map") )
        )
        // .def(
        //     "cbegin",
        //     ( const_iterator ( ::genesis::PlacementMapSet::* )(  ) const )( &::genesis::PlacementMapSet::cbegin )
        // )
        // .def(
        //     "cend",
        //     ( const_iterator ( ::genesis::PlacementMapSet::* )(  ) const )( &::genesis::PlacementMapSet::cend )
        // )
        .def(
            "clear",
            ( void ( ::genesis::PlacementMapSet::* )(  ))( &::genesis::PlacementMapSet::clear )
        )
        .def(
            "dump",
            ( std::string ( ::genesis::PlacementMapSet::* )( bool ))( &::genesis::PlacementMapSet::dump ),
            ( boost::python::arg("full")=(bool)(false) )
        )
        .def(
            "empty",
            ( bool ( ::genesis::PlacementMapSet::* )(  ) const )( &::genesis::PlacementMapSet::empty ),
            get_docstring("bool ::genesis::PlacementMapSet::empty () const")
        )
        .def(
            "get_first",
            ( std::shared_ptr< PlacementMap > ( ::genesis::PlacementMapSet::* )( const std::string & ))( &::genesis::PlacementMapSet::get_first ),
            ( boost::python::arg("name") )
        )
        .def(
            "merge_all",
            ( PlacementMap ( ::genesis::PlacementMapSet::* )(  ))( &::genesis::PlacementMapSet::merge_all ),
            get_docstring("PlacementMap ::genesis::PlacementMapSet::merge_all ()")
        )
        .def(
            "size",
            ( size_t ( ::genesis::PlacementMapSet::* )(  ) const )( &::genesis::PlacementMapSet::size ),
            get_docstring("size_t ::genesis::PlacementMapSet::size () const")
        )

        // Operators

        .def(
            "__len__",
            (::size_t ( ::genesis::PlacementMapSet::* )(  ) const)( &::genesis::PlacementMapSet::size ),
            get_docstring("size_t ::genesis::PlacementMapSet::size () const")
        )
        // .def(
        //     "__getitem__",
        //     ( const ::genesis::PlacementMapSet::NamedMap & ( ::genesis::PlacementMapSet::* )( const std::size_t ) const )( &::genesis::PlacementMapSet::operator[] ),
        //     ( boost::python::arg("index") )
        // )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::PlacementMapSet::begin, &::genesis::PlacementMapSet::end )
        )
    ;
}
