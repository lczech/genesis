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
void PythonExportClass_IteratorLevelorder(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IteratorLevelorder
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using IteratorLevelorderType = IteratorLevelorder<typename LinkType, typename NodeType, typename EdgeType>;

    boost::python::class_< IteratorLevelorderType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< TreeType & >(( boost::python::arg("tree") )) )
        .def( boost::python::init< TreeType const & >(( boost::python::arg("tree") )) )
        .def( boost::python::init< NodeType & >(( boost::python::arg("node") )) )
        .def( boost::python::init< LinkType & >(( boost::python::arg("link") )) )
        .def( boost::python::init< IteratorLevelorder const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "depth",
            ( int ( IteratorLevelorderType::* )(  ) const )( &IteratorLevelorderType::depth )
        )
        .def(
            "edge",
            ( EdgeType & ( IteratorLevelorderType::* )(  ) const )( &IteratorLevelorderType::edge ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "is_first_iteration",
            ( bool ( IteratorLevelorderType::* )(  ) const )( &IteratorLevelorderType::is_first_iteration )
        )
        .def(
            "link",
            ( LinkType & ( IteratorLevelorderType::* )(  ) const )( &IteratorLevelorderType::link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "node",
            ( NodeType & ( IteratorLevelorderType::* )(  ) const )( &IteratorLevelorderType::node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "start_link",
            ( LinkType & ( IteratorLevelorderType::* )(  ) const )( &IteratorLevelorderType::start_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "start_node",
            ( NodeType & ( IteratorLevelorderType::* )(  ) const )( &IteratorLevelorderType::start_node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )

        // Operators

        .def( boost::python::self != boost::python::self )
        .def( boost::python::self == boost::python::self )
    ;
}

template<typename ElementType>
void python_export_function_tree_iterator_levelorder_ElementType ()
{
    boost::python::def(
        "levelorder",
        ( utils::Range< IteratorLevelorder< typename ElementType::LinkType const, typename ElementType::NodeType const, typename ElementType::EdgeType const  > > ( * )( ElementType const & ))( &::genesis::tree::levelorder ),
        ( boost::python::arg("element") )
    );

    boost::python::def(
        "levelorder",
        ( utils::Range< IteratorLevelorder< typename ElementType::LinkType, typename ElementType::NodeType, typename ElementType::EdgeType > > ( * )( ElementType & ))( &::genesis::tree::levelorder ),
        ( boost::python::arg("element") )
    );
}

