/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_CLASS( ::genesis::taxonomy::Taxonomy, scope )
{

    // -------------------------------------------------------------------
    //     Class Taxonomy
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::Taxonomy, std::shared_ptr<::genesis::taxonomy::Taxonomy> > ( scope, "Taxonomy" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< Taxonomy const & >(),
            pybind11::arg("arg"),
            get_docstring("::genesis::taxonomy::Taxonomy::Taxonomy (Taxonomy const & )")
        )

        // Public Member Functions

        .def(
            "add_child",
            ( Taxon & ( ::genesis::taxonomy::Taxonomy::* )( Taxon const &, bool ))( &::genesis::taxonomy::Taxonomy::add_child ),
            pybind11::arg("child"),
            pybind11::arg("merge_duplicates")=(bool)(true),
            get_docstring("Taxon & ::genesis::taxonomy::Taxonomy::add_child (Taxon const & child, bool merge_duplicates=true)")
        )
        .def(
            "add_child",
            ( Taxon & ( ::genesis::taxonomy::Taxonomy::* )( std::string const &, bool ))( &::genesis::taxonomy::Taxonomy::add_child ),
            pybind11::arg("name"),
            pybind11::arg("merge_duplicates")=(bool)(true),
            get_docstring("Taxon & ::genesis::taxonomy::Taxonomy::add_child (std::string const & name, bool merge_duplicates=true)")
        )
        .def(
            "at",
            ( Taxon & ( ::genesis::taxonomy::Taxonomy::* )( size_t ))( &::genesis::taxonomy::Taxonomy::at ),
            pybind11::arg("index"),
            get_docstring("Taxon & ::genesis::taxonomy::Taxonomy::at (size_t index)")
        )
        .def(
            "at",
            ( Taxon const & ( ::genesis::taxonomy::Taxonomy::* )( size_t ) const )( &::genesis::taxonomy::Taxonomy::at ),
            pybind11::arg("index"),
            get_docstring("Taxon const & ::genesis::taxonomy::Taxonomy::at (size_t index) const")
        )
        // .def(
        //     "cbegin",
        //     ( const_iterator ( ::genesis::taxonomy::Taxonomy::* )(  ) const )( &::genesis::taxonomy::Taxonomy::cbegin ),
        //     get_docstring("const_iterator ::genesis::taxonomy::Taxonomy::cbegin () const")
        // )
        // .def(
        //     "cend",
        //     ( const_iterator ( ::genesis::taxonomy::Taxonomy::* )(  ) const )( &::genesis::taxonomy::Taxonomy::cend ),
        //     get_docstring("const_iterator ::genesis::taxonomy::Taxonomy::cend () const")
        // )
        .def(
            "clear_children",
            ( void ( ::genesis::taxonomy::Taxonomy::* )(  ))( &::genesis::taxonomy::Taxonomy::clear_children ),
            get_docstring("void ::genesis::taxonomy::Taxonomy::clear_children ()")
        )
        .def(
            "get_child",
            ( Taxon & ( ::genesis::taxonomy::Taxonomy::* )( std::string ))( &::genesis::taxonomy::Taxonomy::get_child ),
            pybind11::arg("name"),
            get_docstring("Taxon & ::genesis::taxonomy::Taxonomy::get_child (std::string name)")
        )
        .def(
            "get_child",
            ( Taxon const & ( ::genesis::taxonomy::Taxonomy::* )( std::string ) const )( &::genesis::taxonomy::Taxonomy::get_child ),
            pybind11::arg("name"),
            get_docstring("Taxon const & ::genesis::taxonomy::Taxonomy::get_child (std::string name) const")
        )
        .def(
            "has_child",
            ( bool ( ::genesis::taxonomy::Taxonomy::* )( std::string ) const )( &::genesis::taxonomy::Taxonomy::has_child ),
            pybind11::arg("name"),
            get_docstring("bool ::genesis::taxonomy::Taxonomy::has_child (std::string name) const")
        )
        .def(
            "index_of",
            ( size_t ( ::genesis::taxonomy::Taxonomy::* )( std::string const & ) const )( &::genesis::taxonomy::Taxonomy::index_of ),
            pybind11::arg("name")
        )
        .def(
            "remove_at",
            ( void ( ::genesis::taxonomy::Taxonomy::* )( size_t ))( &::genesis::taxonomy::Taxonomy::remove_at ),
            pybind11::arg("index"),
            get_docstring("void ::genesis::taxonomy::Taxonomy::remove_at (size_t index)")
        )
        .def(
            "remove_child",
            ( void ( ::genesis::taxonomy::Taxonomy::* )( std::string const & ))( &::genesis::taxonomy::Taxonomy::remove_child ),
            pybind11::arg("name"),
            get_docstring("void ::genesis::taxonomy::Taxonomy::remove_child (std::string const & name)")
        )
        .def(
            "size",
            ( size_t ( ::genesis::taxonomy::Taxonomy::* )(  ) const )( &::genesis::taxonomy::Taxonomy::size ),
            get_docstring("size_t ::genesis::taxonomy::Taxonomy::size () const")
        )
        // .def(
        //     "sort",
        //     ( void ( ::genesis::taxonomy::Taxonomy::* )( Compare ))( &::genesis::taxonomy::Taxonomy::sort ),
        //     pybind11::arg("comp"),
        //     get_docstring("void ::genesis::taxonomy::Taxonomy::sort (Compare comp)")
        // )

        // Operators

        .def(
            "__getitem__",
            ( Taxon const & ( ::genesis::taxonomy::Taxonomy::* )( std::string ) const )( &::genesis::taxonomy::Taxonomy::operator[] ),
            pybind11::arg("name"),
            get_docstring("Taxon const & ::genesis::taxonomy::Taxonomy::operator[] (std::string name) const")
        )
        .def(
            "__getitem__",
            ( Taxon & ( ::genesis::taxonomy::Taxonomy::* )( std::string ))( &::genesis::taxonomy::Taxonomy::operator[] ),
            pybind11::arg("name"),
            get_docstring("Taxon & ::genesis::taxonomy::Taxonomy::operator[] (std::string name)")
        )
        .def(
            "__getitem__",
            ( Taxon const & ( ::genesis::taxonomy::Taxonomy::* )( size_t ) const )( &::genesis::taxonomy::Taxonomy::operator[] ),
            pybind11::arg("index"),
            get_docstring("Taxon const & ::genesis::taxonomy::Taxonomy::operator[] (size_t index) const")
        )
        .def(
            "__getitem__",
            ( Taxon & ( ::genesis::taxonomy::Taxonomy::* )( size_t ))( &::genesis::taxonomy::Taxonomy::operator[] ),
            pybind11::arg("index"),
            get_docstring("Taxon & ::genesis::taxonomy::Taxonomy::operator[] (size_t index)")
        )
        .def(
            "__str__",
            []( ::genesis::taxonomy::Taxonomy const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::taxonomy::Taxonomy& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            }
            // ,
            // py::keep_alive<0, 1>()
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS( taxonomy_taxonomy_export, ::genesis::taxonomy, scope )
{

    scope.def(
        "swap",
        ( void ( * )( Taxonomy &, Taxonomy & ))( &::genesis::taxonomy::swap ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );
}
