#ifndef GENESIS_BOOST_PYTHON_EXPORT_TREE_ITERATOR_H_
#define GENESIS_BOOST_PYTHON_EXPORT_TREE_ITERATOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "tree/tree.hpp"
#include "tree/tree_iterator.hpp"

const char* get_docstring (const std::string& signature);
inline boost::python::object pass_through (boost::python::object const& o) { return o; }

// -------------------------------------------------------------------
//     Class TreeIteratorEulertour
// -------------------------------------------------------------------

template <class TreeType>
void BoostPythonExport_TreeIteratorEulertour()
{
    typedef typename TreeType::IteratorEulertour TreeIteratorEulertour;
    typedef typename TreeType::EdgeType EdgeType;
    typedef typename TreeType::LinkType LinkType;
    typedef typename TreeType::NodeType NodeType;

    boost::python::class_< TreeIteratorEulertour > ( "TreeIteratorEulertour", boost::python::init< LinkType* >(( boost::python::arg("link") )) )

        // Public Member Functions

        .def(
            "edge",
            ( EdgeType* ( TreeIteratorEulertour::* )(  ) const )( &TreeIteratorEulertour::edge ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "link",
            ( LinkType* ( TreeIteratorEulertour::* )(  ) const )( &TreeIteratorEulertour::link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "node",
            ( NodeType* ( TreeIteratorEulertour::* )(  ) const )( &TreeIteratorEulertour::node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "start_link",
            ( LinkType* ( TreeIteratorEulertour::* )(  ) const )( &TreeIteratorEulertour::start_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "start_node",
            ( NodeType* ( TreeIteratorEulertour::* )(  ) const )( &TreeIteratorEulertour::start_node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )

        // Operators

        .def( boost::python::self == boost::python::self )
        .def( boost::python::self != boost::python::self )
    ;
}

// -------------------------------------------------------------------
//     Tree Iterator Wrapper
// -------------------------------------------------------------------

template<class TreeType, class TreeIterator>
struct TreeIteratorWrapper
{
    TreeIterator it_;

    TreeIteratorWrapper(TreeType& tree) : it_( TreeIterator(tree.root_link()) )
    {}

    // static TreeIterator init (TreeType& tree)
    // {
    //     return TreeIterator(tree.root_link());
    // }

    TreeIterator next ()
    {
        ++it_;
        if (it_ == TreeIterator(nullptr)) {
            PyErr_SetString(PyExc_StopIteration, "No more data.");
            boost::python::throw_error_already_set();
        }
        return it_;
    }

    static void wrap (const char* python_name)
    {
        boost::python::class_< TreeIteratorWrapper > ( python_name, boost::python::init< TreeType & >(( boost::python::arg("tree") )) )
            // .def("__init__", init, boost::python::return_value_policy< boost::python::return_by_value >() )
            // .def("__init__", init)
            .def("next", &TreeIteratorWrapper::next)
            .def("__iter__", pass_through)
        ;
    }
};

// -------------------------------------------------------------------
//     Tree Iterator
// -------------------------------------------------------------------

template <class TreeType>
void BoostPythonExport_TreeIterators (std::string name)
{
    using namespace genesis;

    BoostPythonExport_TreeIteratorEulertour<TreeType>();
    TreeIteratorWrapper< const TreeType, typename TreeType::IteratorEulertour >::wrap("Eulertour");
}

#endif // include guard
