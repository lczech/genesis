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

// -------------------------------------------------------------------
//     Tree Iterator Eulertour
// -------------------------------------------------------------------

template <class TreeType>
void BoostPythonExport_TreeIteratorEulertour()
{
    typedef typename TreeType::EdgeType EdgeType;
    typedef typename TreeType::LinkType LinkType;
    typedef typename TreeType::NodeType NodeType;

    typedef typename TreeType::IteratorEulertour TreeIteratorEulertour;

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
    ;
}

// -------------------------------------------------------------------
//     Tree Iterator Wrapper
// -------------------------------------------------------------------

template<class TreeType, class TreeIterator>
struct TreeIteratorWrapper
{
    TreeIterator it_;

    TreeIteratorWrapper(         TreeType&           tree) : it_{ TreeIterator(tree.root_link()) } {}
    TreeIteratorWrapper(typename TreeType::LinkType& link) : it_{ TreeIterator(&link)            } {}
    TreeIteratorWrapper(typename TreeType::NodeType& node) : it_{ TreeIterator(node.link())      } {}

    TreeIterator next ()
    {
        if (it_ == TreeIterator(nullptr)) {
            PyErr_SetString(PyExc_StopIteration, "No more data.");
            boost::python::throw_error_already_set();
        }
        return it_++;
    }

    static inline TreeIteratorWrapper pass_through (const TreeIteratorWrapper& iter)
    {
        return iter;
    }

    static void wrap (const std::string& python_name)
    {
        typedef typename TreeType::EdgeType EdgeType;
        typedef typename TreeType::LinkType LinkType;
        typedef typename TreeType::NodeType NodeType;

        boost::python::class_< TreeIteratorWrapper > ( python_name.c_str(), boost::python::init< TreeType & >(( boost::python::arg("tree") )) )
            .def( boost::python::init< LinkType & >(( boost::python::arg("link") )) )
            .def( boost::python::init< NodeType & >(( boost::python::arg("node") )) )

            .def("__iter__", pass_through)
            .def("next", &TreeIteratorWrapper::next)
        ;
    }
};

// -------------------------------------------------------------------
//     Tree Iterator
// -------------------------------------------------------------------

template <class TreeType>
void BoostPythonExport_TreeIterators (const std::string& name)
{
    using namespace genesis;

    BoostPythonExport_TreeIteratorEulertour<TreeType>();
    TreeIteratorWrapper< const TreeType, typename TreeType::IteratorEulertour >::wrap(name + "Eulertour");
}

#endif // include guard
