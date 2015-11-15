/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/pquery.hpp"

#include "lib/placement/placement_tree.hpp"

// -------------------------------------------------------------------
//     Class PqueryPlacement
// -------------------------------------------------------------------

PYTHON_EXPORT_CLASS (PqueryPlacement, "placement")
{

    boost::python::class_< ::genesis::PqueryPlacement > ( "PqueryPlacement", boost::python::init<  >(  ) )
        .def( boost::python::init< const PqueryPlacement & >(( boost::python::arg("other") )) )

        // Public Member Functions

        .def(
            "edge",
            ( const PlacementTree::EdgeType & ( ::genesis::PqueryPlacement::* )(  ) const )( &::genesis::PqueryPlacement::get_edge ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "pquery",
            ( const Pquery & ( ::genesis::PqueryPlacement::* )(  ) const )( &::genesis::PqueryPlacement::get_pquery ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )

        // Public Member Variables

        .def_readonly("edge_num", &::genesis::PqueryPlacement::edge_num)
        .def_readonly("likelihood", &::genesis::PqueryPlacement::likelihood)
        .def_readonly("like_weight_ratio", &::genesis::PqueryPlacement::like_weight_ratio)
        .def_readonly("proximal_length", &::genesis::PqueryPlacement::proximal_length)
        .def_readonly("pendant_length", &::genesis::PqueryPlacement::pendant_length)
        .def_readonly("parsimony", &::genesis::PqueryPlacement::parsimony)
    ;
}

// -------------------------------------------------------------------
//     Class PqueryName
// -------------------------------------------------------------------

PYTHON_EXPORT_CLASS (PqueryName, "placement")
{
    boost::python::class_< ::genesis::PqueryName > ( "PqueryName", boost::python::init< boost::python::optional< std::string > >(( boost::python::arg("name")=(std::string)("") )) )
        .def( boost::python::init< const PqueryName & >(( boost::python::arg("other") )) )

        // Public Member Variables

        .def_readonly("name", &::genesis::PqueryName::name)
        .def_readonly("multiplicity", &::genesis::PqueryName::multiplicity)
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

    boost::python::class_< ::genesis::Pquery, boost::noncopyable > ( "Pquery" )

        // Public Member Functions

        // .def(
        //     "add_name",
        //     ( PqueryName * ( ::genesis::Pquery::* )( std::string ))( &::genesis::Pquery::add_name ),
        //     ( boost::python::arg("name")=(std::string)("") ),
        //     get_docstring("PqueryName * ::genesis::Pquery::add_name (std::string name="")")
        // )
        // .def(
        //     "add_placement",
        //     ( PqueryPlacement * ( ::genesis::Pquery::* )( PlacementTree::EdgeType * ))( &::genesis::Pquery::add_placement ),
        //     ( boost::python::arg("edge") ),
        //     get_docstring("PqueryPlacement * ::genesis::Pquery::add_placement (PlacementTree::EdgeType * edge)")
        // )
        .def(
            "name_at",
            ( PqueryName const & ( ::genesis::Pquery::* )( const size_t ) const )( &::genesis::Pquery::name_at ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            ( boost::python::arg("index") )
        )
        .def(
            "placement_at",
            ( PqueryPlacement const & ( ::genesis::Pquery::* )( const size_t ) const )( &::genesis::Pquery::placement_at ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            ( boost::python::arg("index") )
        )
        .def(
            "name_size",
            ( size_t ( ::genesis::Pquery::* )(  ) const )( &::genesis::Pquery::name_size )
        )
        .def(
            "placement_size",
            ( size_t ( ::genesis::Pquery::* )(  ) const )( &::genesis::Pquery::placement_size )
        )
    ;
}
