/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/sample_set.hpp"

using namespace genesis::placement;

PYTHON_EXPORT_CLASS (SampleSet, "placement")
{
    boost::python::class_< ::genesis::placement::SampleSet > ( "SampleSet" )

        // Public Member Functions

        .def(
            "add",
            ( void ( ::genesis::placement::SampleSet::* )( const std::string &, Sample& ))( &::genesis::placement::SampleSet::add ),
            ( boost::python::arg("name"), boost::python::arg("map") )
        )
        // .def(
        //     "cbegin",
        //     ( const_iterator ( ::genesis::placement::SampleSet::* )(  ) const )( &::genesis::placement::SampleSet::cbegin )
        // )
        // .def(
        //     "cend",
        //     ( const_iterator ( ::genesis::placement::SampleSet::* )(  ) const )( &::genesis::placement::SampleSet::cend )
        // )
        .def(
            "clear",
            ( void ( ::genesis::placement::SampleSet::* )(  ))( &::genesis::placement::SampleSet::clear )
        )
        .def(
            "dump",
            ( std::string ( ::genesis::placement::SampleSet::* )( bool ))( &::genesis::placement::SampleSet::dump ),
            ( boost::python::arg("full")=(bool)(false) )
        )
        .def(
            "empty",
            ( bool ( ::genesis::placement::SampleSet::* )(  ) const )( &::genesis::placement::SampleSet::empty ),
            get_docstring("bool ::genesis::placement::SampleSet::empty () const")
        )
        // .def(
        //     "get_first",
        //     ( std::shared_ptr< Sample > ( ::genesis::placement::SampleSet::* )( const std::string & ))( &::genesis::placement::SampleSet::get_first ),
        //     ( boost::python::arg("name") )
        // )
        .def(
            "merge_all",
            ( Sample ( ::genesis::placement::SampleSet::* )(  ))( &::genesis::placement::SampleSet::merge_all ),
            get_docstring("Sample ::genesis::placement::SampleSet::merge_all ()")
        )
        .def(
            "size",
            ( size_t ( ::genesis::placement::SampleSet::* )(  ) const )( &::genesis::placement::SampleSet::size ),
            get_docstring("size_t ::genesis::placement::SampleSet::size () const")
        )

        // Operators

        .def(
            "__len__",
            (::size_t ( ::genesis::placement::SampleSet::* )(  ) const)( &::genesis::placement::SampleSet::size ),
            get_docstring("size_t ::genesis::placement::SampleSet::size () const")
        )
        // .def(
        //     "__getitem__",
        //     ( const ::genesis::placement::SampleSet::NamedMap & ( ::genesis::placement::SampleSet::* )( const std::size_t ) const )( &::genesis::placement::SampleSet::operator[] ),
        //     ( boost::python::arg("index") )
        // )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::placement::SampleSet::begin, &::genesis::placement::SampleSet::end )
        )
    ;
}
