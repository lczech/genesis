/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::tree;

template <typename LinkType, typename NodeType, typename EdgeType>
void PythonExportClass_IteratorNodeLinks(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IteratorNodeLinks
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using IteratorNodeLinksType = IteratorNodeLinks<typename LinkType, typename NodeType, typename EdgeType>;

    boost::python::class_< IteratorNodeLinksType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< NodeType & >(( boost::python::arg("node") )) )
        .def( boost::python::init< LinkType & >(( boost::python::arg("link") )) )
        .def( boost::python::init< IteratorNodeLinks const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "edge",
            ( EdgeType & ( IteratorNodeLinksType::* )(  ) const )( &IteratorNodeLinksType::edge ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "is_first_iteration",
            ( bool ( IteratorNodeLinksType::* )(  ) const )( &IteratorNodeLinksType::is_first_iteration )
        )
        .def(
            "link",
            ( LinkType & ( IteratorNodeLinksType::* )(  ) const )( &IteratorNodeLinksType::link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "node",
            ( NodeType & ( IteratorNodeLinksType::* )(  ) const )( &IteratorNodeLinksType::node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "start_link",
            ( LinkType & ( IteratorNodeLinksType::* )(  ) const )( &IteratorNodeLinksType::start_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )

        // Operators

        .def( boost::python::self != boost::python::self )
        .def( boost::python::self == boost::python::self )
    ;
}

template<typename ElementType>
void python_export_function_tree_iterator_node_links_ElementType ()
{
    boost::python::def(
        "node_links",
        ( utils::Range< IteratorNodeLinks< typename ElementType::LinkType const, typename ElementType::NodeType const, typename ElementType::EdgeType const  > > ( * )( ElementType const & ))( &::genesis::tree::node_links ),
        ( boost::python::arg("element") )
    );

    boost::python::def(
        "node_links",
        ( utils::Range< IteratorNodeLinks< typename ElementType::LinkType, typename ElementType::NodeType, typename ElementType::EdgeType > > ( * )( ElementType & ))( &::genesis::tree::node_links ),
        ( boost::python::arg("element") )
    );
}

