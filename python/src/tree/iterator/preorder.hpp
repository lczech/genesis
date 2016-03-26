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
void PythonExportClass_IteratorPreorder(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IteratorPreorder
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using IteratorPreorderType = IteratorPreorder<typename LinkType, typename NodeType, typename EdgeType>;

    boost::python::class_< IteratorPreorderType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< TreeType & >(( boost::python::arg("tree") )) )
        .def( boost::python::init< TreeType const & >(( boost::python::arg("tree") )) )
        .def( boost::python::init< NodeType & >(( boost::python::arg("node") )) )
        .def( boost::python::init< LinkType & >(( boost::python::arg("link") )) )
        .def( boost::python::init< IteratorPreorder const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "edge",
            ( EdgeType & ( IteratorPreorderType::* )(  ) const )( &IteratorPreorderType::edge ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "is_first_iteration",
            ( bool ( IteratorPreorderType::* )(  ) const )( &IteratorPreorderType::is_first_iteration )
        )
        .def(
            "link",
            ( LinkType & ( IteratorPreorderType::* )(  ) const )( &IteratorPreorderType::link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "node",
            ( NodeType & ( IteratorPreorderType::* )(  ) const )( &IteratorPreorderType::node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "start_link",
            ( LinkType & ( IteratorPreorderType::* )(  ) const )( &IteratorPreorderType::start_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "start_node",
            ( NodeType & ( IteratorPreorderType::* )(  ) const )( &IteratorPreorderType::start_node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )

        // Operators

        .def( boost::python::self != boost::python::self )
        .def( boost::python::self == boost::python::self )
    ;
}

template<typename ElementType>
void python_export_function_tree_iterator_preorder_ElementType ()
{
    boost::python::def(
        "preorder",
        ( utils::Range< IteratorPreorder< typename ElementType::LinkType const, typename ElementType::NodeType const, typename ElementType::EdgeType const  > > ( * )( ElementType const & ))( &::genesis::tree::preorder ),
        ( boost::python::arg("element") )
    );

    boost::python::def(
        "preorder",
        ( utils::Range< IteratorPreorder< typename ElementType::LinkType, typename ElementType::NodeType, typename ElementType::EdgeType > > ( * )( ElementType & ))( &::genesis::tree::preorder ),
        ( boost::python::arg("element") )
    );
}

