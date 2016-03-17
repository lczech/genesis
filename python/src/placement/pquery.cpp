/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/pquery.hpp"

#include "lib/placement/placement_tree.hpp"

using namespace genesis::placement;

// -------------------------------------------------------------------
//     Class PqueryPlacement
// -------------------------------------------------------------------

PYTHON_EXPORT_CLASS (PqueryPlacement, "placement")
{

    boost::python::class_< ::genesis::placement::PqueryPlacement > ( "PqueryPlacement", boost::python::init<  >(  ) )
        .def( boost::python::init< const PqueryPlacement & >(( boost::python::arg("other") )) )

        // Public Member Functions

        .def(
            "edge",
            ( const PlacementTree::EdgeType & ( ::genesis::placement::PqueryPlacement::* )(  ) const )( &::genesis::placement::PqueryPlacement::edge ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        // .def(
        //     "pquery",
        //     ( const Pquery & ( ::genesis::placement::PqueryPlacement::* )(  ) const )( &::genesis::placement::PqueryPlacement::get_pquery ),
        //     boost::python::return_value_policy<boost::python::reference_existing_object>()
        // )

        // Public Member Variables

        .def_readonly("edge_num", &::genesis::placement::PqueryPlacement::edge_num)
        .def_readonly("likelihood", &::genesis::placement::PqueryPlacement::likelihood)
        .def_readonly("like_weight_ratio", &::genesis::placement::PqueryPlacement::like_weight_ratio)
        .def_readonly("proximal_length", &::genesis::placement::PqueryPlacement::proximal_length)
        .def_readonly("pendant_length", &::genesis::placement::PqueryPlacement::pendant_length)
        .def_readonly("parsimony", &::genesis::placement::PqueryPlacement::parsimony)
    ;
}

// -------------------------------------------------------------------
//     Class PqueryName
// -------------------------------------------------------------------

PYTHON_EXPORT_CLASS (PqueryName, "placement")
{
    boost::python::class_< ::genesis::placement::PqueryName > ( "PqueryName", boost::python::init< boost::python::optional< std::string > >(( boost::python::arg("name")=(std::string)("") )) )
        .def( boost::python::init< const PqueryName & >(( boost::python::arg("other") )) )

        // Public Member Variables

        .def_readonly("name", &::genesis::placement::PqueryName::name)
        .def_readonly("multiplicity", &::genesis::placement::PqueryName::multiplicity)
    ;
}

// -------------------------------------------------------------------
//     Class Pquery
// -------------------------------------------------------------------

PYTHON_EXPORT_CLASS (Pquery, "placement")
{
    // boost::python::class_<std::vector<PqueryPlacement> >("VectorPqueryPlacement")
    //     .def(boost::python::vector_indexing_suite<std::vector<PqueryPlacement> >())
    // ;
    //
    // boost::python::class_<std::vector<PqueryName> >("VectorPqueryName")
    //     .def(boost::python::vector_indexing_suite<std::vector<PqueryName> >())
    // ;

    PYTHON_REQUIRES_CLASS(PqueryPlacement)
    PYTHON_REQUIRES_CLASS(PqueryName)

    boost::python::class_< ::genesis::placement::Pquery, boost::noncopyable > ( "Pquery" )

        // Public Member Functions

        // .def(
        //     "add_name",
        //     ( PqueryName * ( ::genesis::placement::Pquery::* )( std::string ))( &::genesis::placement::Pquery::add_name ),
        //     ( boost::python::arg("name")=(std::string)("") ),
        //     get_docstring("PqueryName * ::genesis::placement::Pquery::add_name (std::string name="")")
        // )
        // .def(
        //     "add_placement",
        //     ( PqueryPlacement * ( ::genesis::placement::Pquery::* )( PlacementTree::EdgeType * ))( &::genesis::placement::Pquery::add_placement ),
        //     ( boost::python::arg("edge") ),
        //     get_docstring("PqueryPlacement * ::genesis::placement::Pquery::add_placement (PlacementTree::EdgeType * edge)")
        // )
        .def(
            "name_at",
            ( PqueryName const & ( ::genesis::placement::Pquery::* )( const size_t ) const )( &::genesis::placement::Pquery::name_at ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            ( boost::python::arg("index") )
        )
        .def(
            "placement_at",
            ( PqueryPlacement const & ( ::genesis::placement::Pquery::* )( const size_t ) const )( &::genesis::placement::Pquery::placement_at ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            ( boost::python::arg("index") )
        )
        .def(
            "name_size",
            ( size_t ( ::genesis::placement::Pquery::* )(  ) const )( &::genesis::placement::Pquery::name_size )
        )
        .def(
            "placement_size",
            ( size_t ( ::genesis::placement::Pquery::* )(  ) const )( &::genesis::placement::Pquery::placement_size )
        )
    ;
}
