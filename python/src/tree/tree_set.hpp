/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <class TreeType>
void PythonExportClass_TreeSet(std::string name)
{

    // -------------------------------------------------------------------
    //     Class TreeSet
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using TreeSetType = TreeSet<class TreeType>;

    boost::python::class_< TreeSetType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< TreeSet const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "add",
            ( void ( TreeSetType::* )( std::string const &, TreeType const & ))( &TreeSetType::add ),
            ( boost::python::arg("name"), boost::python::arg("tree") ),
            get_docstring("void ::genesis::tree::TreeSet::add (std::string const & name, TreeType const & tree)")
        )
        .def(
            "at",
            ( NamedTree & ( TreeSetType::* )( size_t ))( &TreeSetType::at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "at",
            ( const NamedTree & ( TreeSetType::* )( size_t ) const )( &TreeSetType::at ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "clear",
            ( void ( TreeSetType::* )(  ))( &TreeSetType::clear ),
            get_docstring("void ::genesis::tree::TreeSet::clear ()")
        )
        .def(
            "empty",
            ( bool ( TreeSetType::* )(  ) const )( &TreeSetType::empty ),
            get_docstring("bool ::genesis::tree::TreeSet::empty () const")
        )
        .def(
            "remove_at",
            ( void ( TreeSetType::* )( size_t ))( &TreeSetType::remove_at ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::tree::TreeSet::remove_at (size_t index)")
        )
        .def(
            "size",
            ( size_t ( TreeSetType::* )(  ) const )( &TreeSetType::size ),
            get_docstring("size_t ::genesis::tree::TreeSet::size () const")
        )
        .def(
            "swap",
            ( void ( TreeSetType::* )( TreeSet & ))( &TreeSetType::swap ),
            ( boost::python::arg("other") )
        )

        // Operators

        .def(
            "__getitem__",
            ( NamedTree & ( TreeSetType::* )( const std::size_t ))( &TreeSetType::operator[] ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "__getitem__",
            ( const NamedTree & ( TreeSetType::* )( const std::size_t ) const )( &TreeSetType::operator[] ),
            ( boost::python::arg("index") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &TreeSetType::begin, &TreeSetType::end )
        )
    ;
}
