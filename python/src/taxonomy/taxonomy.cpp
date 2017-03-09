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
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "add_child",
            ( Taxon & ( ::genesis::taxonomy::Taxonomy::* )( Taxon const & ))( &::genesis::taxonomy::Taxonomy::add_child ),
            pybind11::arg("child")
        )
        .def(
            "add_child",
            ( Taxon & ( ::genesis::taxonomy::Taxonomy::* )( std::string const & ))( &::genesis::taxonomy::Taxonomy::add_child ),
            pybind11::arg("name")
        )
        .def(
            "at",
            ( Taxon & ( ::genesis::taxonomy::Taxonomy::* )( size_t ))( &::genesis::taxonomy::Taxonomy::at ),
            pybind11::arg("index")
        )
        .def(
            "at",
            ( Taxon const & ( ::genesis::taxonomy::Taxonomy::* )( size_t ) const )( &::genesis::taxonomy::Taxonomy::at ),
            pybind11::arg("index")
        )
        // .def(
        //     "cbegin",
        //     ( const_iterator ( ::genesis::taxonomy::Taxonomy::* )(  ) const )( &::genesis::taxonomy::Taxonomy::cbegin )
        // )
        // .def(
        //     "cend",
        //     ( const_iterator ( ::genesis::taxonomy::Taxonomy::* )(  ) const )( &::genesis::taxonomy::Taxonomy::cend )
        // )
        .def(
            "clear_children",
            ( void ( ::genesis::taxonomy::Taxonomy::* )(  ))( &::genesis::taxonomy::Taxonomy::clear_children )
        )
        .def(
            "get_child",
            ( Taxon & ( ::genesis::taxonomy::Taxonomy::* )( std::string ))( &::genesis::taxonomy::Taxonomy::get_child ),
            pybind11::arg("name")
        )
        .def(
            "get_child",
            ( Taxon const & ( ::genesis::taxonomy::Taxonomy::* )( std::string ) const )( &::genesis::taxonomy::Taxonomy::get_child ),
            pybind11::arg("name")
        )
        .def(
            "has_child",
            ( bool ( ::genesis::taxonomy::Taxonomy::* )( std::string ) const )( &::genesis::taxonomy::Taxonomy::has_child ),
            pybind11::arg("name")
        )
        .def(
            "remove_child",
            ( void ( ::genesis::taxonomy::Taxonomy::* )( std::string const & ))( &::genesis::taxonomy::Taxonomy::remove_child ),
            pybind11::arg("name")
        )
        .def(
            "size",
            ( size_t ( ::genesis::taxonomy::Taxonomy::* )(  ) const )( &::genesis::taxonomy::Taxonomy::size )
        )

        // Operators

        .def(
            "__getitem__",
            ( Taxon const & ( ::genesis::taxonomy::Taxonomy::* )( std::string ) const )( &::genesis::taxonomy::Taxonomy::operator[] ),
            pybind11::arg("name")
        )
        .def(
            "__getitem__",
            ( Taxon & ( ::genesis::taxonomy::Taxonomy::* )( std::string ))( &::genesis::taxonomy::Taxonomy::operator[] ),
            pybind11::arg("name")
        )
        .def(
            "__getitem__",
            ( Taxon const & ( ::genesis::taxonomy::Taxonomy::* )( size_t ) const )( &::genesis::taxonomy::Taxonomy::operator[] ),
            pybind11::arg("index")
        )
        .def(
            "__getitem__",
            ( Taxon & ( ::genesis::taxonomy::Taxonomy::* )( size_t ))( &::genesis::taxonomy::Taxonomy::operator[] ),
            pybind11::arg("index")
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
            },
            pybind11::keep_alive<0, 1>()
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
