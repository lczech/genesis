/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

PYTHON_EXPORT_CLASS( ::genesis::tree::NewickBroker, scope )
{

    // -------------------------------------------------------------------
    //     Class NewickBroker
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::tree::NewickBroker, std::shared_ptr<::genesis::tree::NewickBroker> > ( scope, "NewickBroker" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< NewickBroker const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "assign_ranks",
            ( void ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::assign_ranks )
        )
        .def(
            "at",
            ( NewickBrokerElement & ( ::genesis::tree::NewickBroker::* )( std::size_t ))( &::genesis::tree::NewickBroker::at ),
            pybind11::arg("index")
        )
        .def(
            "at",
            ( NewickBrokerElement const & ( ::genesis::tree::NewickBroker::* )( std::size_t ) const )( &::genesis::tree::NewickBroker::at ),
            pybind11::arg("index")
        )
        .def(
            "bottom",
            ( NewickBrokerElement & ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::bottom )
        )
        .def(
            "bottom",
            ( NewickBrokerElement const & ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::bottom )
        )
        // .def(
        //     "cbegin",
        //     ( const_iterator ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::cbegin )
        // )
        // .def(
        //     "cend",
        //     ( const_iterator ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::cend )
        // )
        .def(
            "clear",
            ( void ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::clear )
        )
        // .def(
        //     "crbegin",
        //     ( const_reverse_iterator ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::crbegin )
        // )
        // .def(
        //     "crend",
        //     ( const_reverse_iterator ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::crend )
        // )
        .def(
            "dump",
            ( std::string ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::dump )
        )
        .def(
            "empty",
            ( bool ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::empty )
        )
        .def(
            "inner_count",
            ( int ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::inner_count )
        )
        .def(
            "is_bifurcating",
            ( bool ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::is_bifurcating )
        )
        .def(
            "leaf_count",
            ( int ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::leaf_count )
        )
        .def(
            "max_rank",
            ( int ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::max_rank )
        )
        .def(
            "node_count",
            ( int ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::node_count )
        )
        .def(
            "pop_bottom",
            ( void ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::pop_bottom )
        )
        .def(
            "pop_top",
            ( void ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::pop_top )
        )
        // .def(
        //     "push_bottom",
        //     ( void ( ::genesis::tree::NewickBroker::* )( NewickBrokerElement && ))( &::genesis::tree::NewickBroker::push_bottom ),
        //     pybind11::arg("node")
        // )
        .def(
            "push_bottom",
            ( void ( ::genesis::tree::NewickBroker::* )( NewickBrokerElement const & ))( &::genesis::tree::NewickBroker::push_bottom ),
            pybind11::arg("node")
        )
        // .def(
        //     "push_top",
        //     ( void ( ::genesis::tree::NewickBroker::* )( NewickBrokerElement && ))( &::genesis::tree::NewickBroker::push_top ),
        //     pybind11::arg("node")
        // )
        .def(
            "push_top",
            ( void ( ::genesis::tree::NewickBroker::* )( NewickBrokerElement const & ))( &::genesis::tree::NewickBroker::push_top ),
            pybind11::arg("node")
        )
        // .def(
        //     "rbegin",
        //     ( const_reverse_iterator ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::rbegin )
        // )
        // .def(
        //     "rbegin",
        //     ( reverse_iterator ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::rbegin )
        // )
        // .def(
        //     "rend",
        //     ( const_reverse_iterator ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::rend )
        // )
        // .def(
        //     "rend",
        //     ( reverse_iterator ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::rend )
        // )
        .def(
            "size",
            ( size_t ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::size )
        )
        .def(
            "top",
            ( NewickBrokerElement & ( ::genesis::tree::NewickBroker::* )(  ))( &::genesis::tree::NewickBroker::top )
        )
        .def(
            "top",
            ( NewickBrokerElement const & ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::top )
        )
        .def(
            "validate",
            ( bool ( ::genesis::tree::NewickBroker::* )(  ) const )( &::genesis::tree::NewickBroker::validate )
        )

        // Operators

        .def(
            "__getitem__",
            ( NewickBrokerElement & ( ::genesis::tree::NewickBroker::* )( std::size_t ))( &::genesis::tree::NewickBroker::operator[] ),
            pybind11::arg("index")
        )
        .def(
            "__getitem__",
            ( NewickBrokerElement const & ( ::genesis::tree::NewickBroker::* )( std::size_t ) const )( &::genesis::tree::NewickBroker::operator[] ),
            pybind11::arg("index")
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::tree::NewickBroker& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            pybind11::keep_alive<0, 1>()
        )
    ;
}
