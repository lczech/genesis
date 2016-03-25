/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "placement/pquery.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS (Pquery, "placement")
{

    // -------------------------------------------------------------------
    //     Class Pquery
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::Pquery > ( "Pquery", boost::python::init<  >(  ) )
        .def( boost::python::init< Pquery const & >(( boost::python::arg("") )) )
        .def( boost::python::init< Pquery && >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "add_name",
            ( PqueryName & ( ::genesis::placement::Pquery::* )( PqueryName const & ))( &::genesis::placement::Pquery::add_name ),
            ( boost::python::arg("other") ),
            get_docstring("PqueryName & ::genesis::placement::Pquery::add_name (PqueryName const & other)")
        )
        .def(
            "add_name",
            ( PqueryName & ( ::genesis::placement::Pquery::* )( std::string, double ))( &::genesis::placement::Pquery::add_name ),
            ( boost::python::arg("name")=(std::string)(""), boost::python::arg("multiplicity")=(double)(0.0) ),
            get_docstring("PqueryName & ::genesis::placement::Pquery::add_name (std::string name="", double multiplicity=0.0)")
        )
        .def(
            "add_placement",
            ( PqueryPlacement & ( ::genesis::placement::Pquery::* )( PlacementTreeEdge & ))( &::genesis::placement::Pquery::add_placement ),
            ( boost::python::arg("edge") ),
            get_docstring("PqueryPlacement & ::genesis::placement::Pquery::add_placement (PlacementTreeEdge & edge)")
        )
        .def(
            "add_placement",
            ( PqueryPlacement & ( ::genesis::placement::Pquery::* )( PlacementTreeEdge &, PqueryPlacement const & ))( &::genesis::placement::Pquery::add_placement ),
            ( boost::python::arg("edge"), boost::python::arg("val") ),
            get_docstring("PqueryPlacement & ::genesis::placement::Pquery::add_placement (PlacementTreeEdge & edge, PqueryPlacement const & val)")
        )
        .def(
            "add_placement",
            ( PqueryPlacement & ( ::genesis::placement::Pquery::* )( PqueryPlacement const & ))( &::genesis::placement::Pquery::add_placement ),
            ( boost::python::arg("val") ),
            get_docstring("PqueryPlacement & ::genesis::placement::Pquery::add_placement (PqueryPlacement const & val)")
        )
        .def(
            "clear",
            ( void ( ::genesis::placement::Pquery::* )(  ))( &::genesis::placement::Pquery::clear ),
            get_docstring("void ::genesis::placement::Pquery::clear ()")
        )
        .def(
            "clear_names",
            ( void ( ::genesis::placement::Pquery::* )(  ))( &::genesis::placement::Pquery::clear_names ),
            get_docstring("void ::genesis::placement::Pquery::clear_names ()")
        )
        .def(
            "clear_placements",
            ( void ( ::genesis::placement::Pquery::* )(  ))( &::genesis::placement::Pquery::clear_placements ),
            get_docstring("void ::genesis::placement::Pquery::clear_placements ()")
        )
        .def(
            "name_at",
            ( PqueryName & ( ::genesis::placement::Pquery::* )( size_t ))( &::genesis::placement::Pquery::name_at ),
            ( boost::python::arg("index") ),
            get_docstring("PqueryName & ::genesis::placement::Pquery::name_at (size_t index)")
        )
        .def(
            "name_at",
            ( PqueryName const & ( ::genesis::placement::Pquery::* )( size_t ) const )( &::genesis::placement::Pquery::name_at ),
            ( boost::python::arg("index") ),
            get_docstring("PqueryName const & ::genesis::placement::Pquery::name_at (size_t index) const")
        )
        .def(
            "name_size",
            ( size_t ( ::genesis::placement::Pquery::* )(  ) const )( &::genesis::placement::Pquery::name_size ),
            get_docstring("size_t ::genesis::placement::Pquery::name_size () const")
        )
        .def(
            "names",
            ( utils::Range< const_iterator_names > ( ::genesis::placement::Pquery::* )(  ) const )( &::genesis::placement::Pquery::names ),
            get_docstring("utils::Range< const_iterator_names > ::genesis::placement::Pquery::names () const")
        )
        .def(
            "names",
            ( utils::Range< iterator_names > ( ::genesis::placement::Pquery::* )(  ))( &::genesis::placement::Pquery::names ),
            get_docstring("utils::Range< iterator_names > ::genesis::placement::Pquery::names ()")
        )
        .def(
            "placement_at",
            ( PqueryPlacement & ( ::genesis::placement::Pquery::* )( size_t ))( &::genesis::placement::Pquery::placement_at ),
            ( boost::python::arg("index") ),
            get_docstring("PqueryPlacement & ::genesis::placement::Pquery::placement_at (size_t index)")
        )
        .def(
            "placement_at",
            ( PqueryPlacement const & ( ::genesis::placement::Pquery::* )( size_t ) const )( &::genesis::placement::Pquery::placement_at ),
            ( boost::python::arg("index") ),
            get_docstring("PqueryPlacement const & ::genesis::placement::Pquery::placement_at (size_t index) const")
        )
        .def(
            "placement_size",
            ( size_t ( ::genesis::placement::Pquery::* )(  ) const )( &::genesis::placement::Pquery::placement_size ),
            get_docstring("size_t ::genesis::placement::Pquery::placement_size () const")
        )
        .def(
            "placements",
            ( utils::Range< const_iterator_placements > ( ::genesis::placement::Pquery::* )(  ) const )( &::genesis::placement::Pquery::placements ),
            get_docstring("utils::Range< const_iterator_placements > ::genesis::placement::Pquery::placements () const")
        )
        .def(
            "placements",
            ( utils::Range< iterator_placements > ( ::genesis::placement::Pquery::* )(  ))( &::genesis::placement::Pquery::placements ),
            get_docstring("utils::Range< iterator_placements > ::genesis::placement::Pquery::placements ()")
        )
        .def(
            "remove_name_at",
            ( void ( ::genesis::placement::Pquery::* )( size_t ))( &::genesis::placement::Pquery::remove_name_at ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::placement::Pquery::remove_name_at (size_t index)")
        )
        .def(
            "remove_placement_at",
            ( void ( ::genesis::placement::Pquery::* )( size_t ))( &::genesis::placement::Pquery::remove_placement_at ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::placement::Pquery::remove_placement_at (size_t index)")
        )

        // Iterators

        .add_property(
            "names",
            boost::python::range ( &::genesis::placement::Pquery::begin_names, &::genesis::placement::Pquery::end_names )
        )
        .add_property(
            "placements",
            boost::python::range ( &::genesis::placement::Pquery::begin_placements, &::genesis::placement::Pquery::end_placements )
        )
    ;
}
