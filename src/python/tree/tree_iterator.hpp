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

template<class TreeType, class TreeIterator>
struct TreeIteratorWrapper
{
    // TreeIterator it_;

    static TreeIterator init (TreeType& tree)
    {
        return TreeIterator(tree.root_link());
    }

    static TreeIterator next (TreeIterator& o)
    {
        TreeIterator result = o++;
        if (result == TreeIterator(nullptr)) {
            PyErr_SetString(PyExc_StopIteration, "No more data.");
            boost::python::throw_error_already_set();
        }
        return result;
    }

    static void wrap (const char* python_name)
    {
        boost::python::class_< TreeIterator > ( python_name, boost::python::no_init )
            // .def("__init__", init, boost::python::return_value_policy< boost::python::return_by_value >() )
            .def("__init__", init)
            .def("next", next)
            .def("__iter__", pass_through)
        ;
    }
};

template <class TreeType>
void BoostPythonExport_TreeIterator (std::string name)
{
    using namespace genesis;

    TreeIteratorWrapper< const TreeType, typename TreeType::IteratorEulertour >().wrap("Eulertour");


    // -------------------------------------------------------------------
    //     Class Tree
    // -------------------------------------------------------------------
/*
    boost::python::class_< TreeType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< const TreeType & >(( boost::python::arg("other") )) )

        // Public Member Functions


        // Iterators


        // ( ::genesis::TreeType::IteratorEulertour ( TreeType::* )(  ) )( &TreeType::begin_eulertour )
        // ( ::genesis::TreeType::IteratorEulertour ( TreeType::* )(  ) )( &TreeType::end_eulertour )
        .add_property(
            "eulertour",
            boost::python::range (
                &TreeType::begin_eulertour,
                &TreeType::end_eulertour
                // ( TreeType::ConstIteratorEulertour ( * )(  ) const )( &TreeType::begin_eulertour ),
                // ( TreeType::ConstIteratorEulertour ( TreeType::* )(  ) const )( &TreeType::end_eulertour )
            )
        )

    ;
*/
}

#endif // include guard
