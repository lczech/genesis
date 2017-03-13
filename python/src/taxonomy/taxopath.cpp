/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::taxonomy;

PYTHON_EXPORT_CLASS( ::genesis::taxonomy::Taxopath, scope )
{

    // -------------------------------------------------------------------
    //     Class Taxopath
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::taxonomy::Taxopath, std::shared_ptr<::genesis::taxonomy::Taxopath> > ( scope, "Taxopath" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< std::vector< std::string > const & >(),
            pybind11::arg("elements")
        )
        // .def(
        //     pybind11::init< std::vector< std::string > && >(),
        //     pybind11::arg("elements")
        // )
        .def(
            pybind11::init< Taxopath const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "assign",
            ( void ( ::genesis::taxonomy::Taxopath::* )( std::vector< std::string > const & ))( &::genesis::taxonomy::Taxopath::assign ),
            pybind11::arg("from")
        )
        .def(
            "at",
            ( std::string & ( ::genesis::taxonomy::Taxopath::* )( size_t ))( &::genesis::taxonomy::Taxopath::at ),
            pybind11::arg("index")
        )
        .def(
            "at",
            ( std::string const & ( ::genesis::taxonomy::Taxopath::* )( size_t ) const )( &::genesis::taxonomy::Taxopath::at ),
            pybind11::arg("index")
        )
        .def(
            "clear",
            ( void ( ::genesis::taxonomy::Taxopath::* )(  ))( &::genesis::taxonomy::Taxopath::clear )
        )
        .def(
            "elements",
            ( std::vector< std::string > & ( ::genesis::taxonomy::Taxopath::* )(  ))( &::genesis::taxonomy::Taxopath::elements )
        )
        .def(
            "elements",
            ( std::vector< std::string > const & ( ::genesis::taxonomy::Taxopath::* )(  ) const )( &::genesis::taxonomy::Taxopath::elements )
        )
        .def(
            "empty",
            ( bool ( ::genesis::taxonomy::Taxopath::* )(  ) const )( &::genesis::taxonomy::Taxopath::empty )
        )
        .def(
            "pop_back",
            ( std::string ( ::genesis::taxonomy::Taxopath::* )(  ))( &::genesis::taxonomy::Taxopath::pop_back )
        )
        // .def(
        //     "push_back",
        //     ( void ( ::genesis::taxonomy::Taxopath::* )( std::string && ))( &::genesis::taxonomy::Taxopath::push_back ),
        //     pybind11::arg("value")
        // )
        .def(
            "push_back",
            ( void ( ::genesis::taxonomy::Taxopath::* )( std::string const & ))( &::genesis::taxonomy::Taxopath::push_back ),
            pybind11::arg("value")
        )
        // .def(
        //     "rbegin",
        //     ( const_reverse_iterator ( ::genesis::taxonomy::Taxopath::* )(  ) const )( &::genesis::taxonomy::Taxopath::rbegin )
        // )
        // .def(
        //     "rbegin",
        //     ( reverse_iterator ( ::genesis::taxonomy::Taxopath::* )(  ))( &::genesis::taxonomy::Taxopath::rbegin )
        // )
        // .def(
        //     "rend",
        //     ( const_reverse_iterator ( ::genesis::taxonomy::Taxopath::* )(  ) const )( &::genesis::taxonomy::Taxopath::rend )
        // )
        // .def(
        //     "rend",
        //     ( reverse_iterator ( ::genesis::taxonomy::Taxopath::* )(  ))( &::genesis::taxonomy::Taxopath::rend )
        // )
        .def(
            "size",
            ( size_t ( ::genesis::taxonomy::Taxopath::* )(  ) const )( &::genesis::taxonomy::Taxopath::size )
        )
        .def(
            "swap",
            ( void ( ::genesis::taxonomy::Taxopath::* )( Taxopath & ))( &::genesis::taxonomy::Taxopath::swap ),
            pybind11::arg("other")
        )

        // Operators

        .def(
            "__getitem__",
            ( std::string const & ( ::genesis::taxonomy::Taxopath::* )( size_t ) const )( &::genesis::taxonomy::Taxopath::operator[] ),
            pybind11::arg("index")
        )
        .def(
            "__getitem__",
            ( std::string & ( ::genesis::taxonomy::Taxopath::* )( size_t ))( &::genesis::taxonomy::Taxopath::operator[] ),
            pybind11::arg("index")
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::taxonomy::Taxopath& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            pybind11::keep_alive<0, 1>()
        )
    ;
}
