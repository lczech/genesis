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
void PythonExportClass_IteratorPostorder(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IteratorPostorder
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using IteratorPostorderType = IteratorPostorder<typename LinkType, typename NodeType, typename EdgeType>;

    boost::python::class_< IteratorPostorderType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< TreeType & >(( boost::python::arg("tree") )) )
        .def( boost::python::init< TreeType const & >(( boost::python::arg("tree") )) )
        .def( boost::python::init< NodeType & >(( boost::python::arg("node") )) )
        .def( boost::python::init< LinkType & >(( boost::python::arg("link") )) )
        .def( boost::python::init< IteratorPostorder const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "edge",
            ( EdgeType & ( IteratorPostorderType::* )(  ) const )( &IteratorPostorderType::edge ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "is_last_iteration",
            ( bool ( IteratorPostorderType::* )(  ) const )( &IteratorPostorderType::is_last_iteration )
        )
        .def(
            "link",
            ( LinkType & ( IteratorPostorderType::* )(  ) const )( &IteratorPostorderType::link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "node",
            ( NodeType & ( IteratorPostorderType::* )(  ) const )( &IteratorPostorderType::node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "start_link",
            ( LinkType & ( IteratorPostorderType::* )(  ) const )( &IteratorPostorderType::start_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "start_node",
            ( NodeType & ( IteratorPostorderType::* )(  ) const )( &IteratorPostorderType::start_node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )

        // Operators

        .def( boost::python::self != boost::python::self )
        .def( boost::python::self == boost::python::self )
    ;
}

template<typename ElementType>
void python_export_function_tree_iterator_postorder_ElementType ()
{
    boost::python::def(
        "postorder",
        ( utils::Range< IteratorPostorder< typename ElementType::LinkType const, typename ElementType::NodeType const, typename ElementType::EdgeType const  > > ( * )( ElementType const & ))( &::genesis::tree::postorder ),
        ( boost::python::arg("element") )
    );

    boost::python::def(
        "postorder",
        ( utils::Range< IteratorPostorder< typename ElementType::LinkType, typename ElementType::NodeType, typename ElementType::EdgeType > > ( * )( ElementType & ))( &::genesis::tree::postorder ),
        ( boost::python::arg("element") )
    );
}

