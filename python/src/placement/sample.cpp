/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS( ::genesis::placement::Sample, scope )
{

    // -------------------------------------------------------------------
    //     Class Sample
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::Sample, std::shared_ptr<::genesis::placement::Sample> > ( scope, "Sample" )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::placement::Sample::Sample ()")
        )
        .def(
            pybind11::init< PlacementTree const & >(),
            pybind11::arg("tree"),
            get_docstring("::genesis::placement::Sample::Sample (PlacementTree const & tree)")
        )
        .def(
            pybind11::init< Sample const & >(),
            pybind11::arg("arg"),
            get_docstring("::genesis::placement::Sample::Sample (Sample const & )")
        )

        // Public Member Functions

        .def(
            "add",
            ( Pquery & ( ::genesis::placement::Sample::* )(  ))( &::genesis::placement::Sample::add ),
            get_docstring("Pquery & ::genesis::placement::Sample::add ()")
        )
        .def(
            "add",
            ( Pquery & ( ::genesis::placement::Sample::* )( Pquery const & ))( &::genesis::placement::Sample::add ),
            pybind11::arg("other"),
            get_docstring("Pquery & ::genesis::placement::Sample::add (Pquery const & other)")
        )
        .def(
            "at",
            ( Pquery & ( ::genesis::placement::Sample::* )( size_t ))( &::genesis::placement::Sample::at ),
            pybind11::arg("index"),
            get_docstring("Pquery & ::genesis::placement::Sample::at (size_t index)")
        )
        .def(
            "at",
            ( Pquery const & ( ::genesis::placement::Sample::* )( size_t ) const )( &::genesis::placement::Sample::at ),
            pybind11::arg("index"),
            get_docstring("Pquery const & ::genesis::placement::Sample::at (size_t index) const")
        )
        .def(
            "clear",
            ( void ( ::genesis::placement::Sample::* )(  ))( &::genesis::placement::Sample::clear ),
            get_docstring("void ::genesis::placement::Sample::clear ()")
        )
        .def(
            "clear_pqueries",
            ( void ( ::genesis::placement::Sample::* )(  ))( &::genesis::placement::Sample::clear_pqueries ),
            get_docstring("void ::genesis::placement::Sample::clear_pqueries ()")
        )
        .def(
            "empty",
            ( bool ( ::genesis::placement::Sample::* )(  ) const )( &::genesis::placement::Sample::empty ),
            get_docstring("bool ::genesis::placement::Sample::empty () const")
        )
        // .def(
        //     "pqueries",
        //     ( utils::Range< const_iterator_pqueries > ( ::genesis::placement::Sample::* )(  ) const )( &::genesis::placement::Sample::pqueries ),
        //     get_docstring("utils::Range< const_iterator_pqueries > ::genesis::placement::Sample::pqueries () const")
        // )
        // .def(
        //     "pqueries",
        //     ( utils::Range< iterator_pqueries > ( ::genesis::placement::Sample::* )(  ))( &::genesis::placement::Sample::pqueries ),
        //     get_docstring("utils::Range< iterator_pqueries > ::genesis::placement::Sample::pqueries ()")
        // )
        // .def(
        //     "remove",
        //     ( void ( ::genesis::placement::Sample::* )( iterator_pqueries ))( &::genesis::placement::Sample::remove ),
        //     pybind11::arg("position"),
        //     get_docstring("void ::genesis::placement::Sample::remove (iterator_pqueries position)")
        // )
        // .def(
        //     "remove",
        //     ( void ( ::genesis::placement::Sample::* )( iterator_pqueries, iterator_pqueries ))( &::genesis::placement::Sample::remove ),
        //     pybind11::arg("first"),
        //     pybind11::arg("last"),
        //     get_docstring("void ::genesis::placement::Sample::remove (iterator_pqueries first, iterator_pqueries last)")
        // )
        .def(
            "remove",
            ( void ( ::genesis::placement::Sample::* )( size_t ))( &::genesis::placement::Sample::remove ),
            pybind11::arg("index"),
            get_docstring("void ::genesis::placement::Sample::remove (size_t index)")
        )
        .def(
            "remove",
            ( void ( ::genesis::placement::Sample::* )( size_t, size_t ))( &::genesis::placement::Sample::remove ),
            pybind11::arg("first_index"),
            pybind11::arg("last_index"),
            get_docstring("void ::genesis::placement::Sample::remove (size_t first_index, size_t last_index)")
        )
        .def(
            "size",
            ( size_t ( ::genesis::placement::Sample::* )(  ) const )( &::genesis::placement::Sample::size ),
            get_docstring("size_t ::genesis::placement::Sample::size () const")
        )
        .def(
            "swap",
            ( void ( ::genesis::placement::Sample::* )( Sample & ))( &::genesis::placement::Sample::swap ),
            pybind11::arg("other"),
            get_docstring("void ::genesis::placement::Sample::swap (Sample & other)")
        )
        .def(
            "tree",
            ( PlacementTree & ( ::genesis::placement::Sample::* )(  ))( &::genesis::placement::Sample::tree ),
            get_docstring("PlacementTree & ::genesis::placement::Sample::tree ()")
        )
        .def(
            "tree",
            ( PlacementTree const & ( ::genesis::placement::Sample::* )(  ) const )( &::genesis::placement::Sample::tree ),
            get_docstring("PlacementTree const & ::genesis::placement::Sample::tree () const")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::placement::Sample const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::placement::Sample& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            pybind11::keep_alive<0, 1>()
        )
    ;
}
